#include <optional>
#include "VaultKeeper.h"
#include "WorkerThread.h"
#include "Vault.h"
#include "App/ContentStore.h"
#include "GUI/VaultCommander.hpp"
#include "Crypto.h"
#include "Engine/Logger.h"
#include "Utility/StringUtils.h"

using Future = VaultKeeper::Future;

VaultKeeper::VaultKeeper(ContentStore& store, UnsavedState& unsavedState) : store{store}, unsavedState{unsavedState}
{
	file = L"vault.bin";
	mChain.reserve(16);
}

VaultKeeper::~VaultKeeper()
{
}

void VaultKeeper::Initialize()
{
	worker.Initialize();
}

void VaultKeeper::Shutdown()
{
	worker.Shutdown();
}

Future VaultKeeper::OpenVault(const std::wstring& file)
{
	return worker.SendCmd([=, this]() -> uint64_t
		{
			ResetState();
			if (!vault.Open(file))
			{
				vault.Reset();
				RaiseError(std::format("Failed to open vault {}", StringUtils::WideStringToUtf8(file)));
				return Failed;
			}
			Logger::Log(L"Opened vault {}", file);

			auto& block = vault.GetBlock();
			if (block)
			{
				Logger::Log("Deserializing content");

				auto size = strnlen_s(reinterpret_cast<const char*>(block.data()), block.size());
				if (!store.Deserialize(block.span(0, size)))
					return RaiseError("Failed to deserialize content", true);
				Logger::Log("Opened vault");

				vault.Reset();
				unsavedState.ClearChange();
				return Success;
			}

			this->file = file;
			{
				auto& hint = vault.GetStepName();

				std::lock_guard lock(chainMutex);
				mChain.push_back(Layer
					{
						.hint = std::string(reinterpret_cast<char*>(hint.data()), hint.size()),
					});
			}
			return UnlockRequired;
		});
}

Future VaultKeeper::CreateVault(const std::wstring& file)
{
	return worker.SendCmd([=, this]
		{
			Logger::Log("Preparing new vault");

			ResetState();
			this->file = file;

			unsavedState.NotifyChange();
			return Success;
		});
}

Future VaultKeeper::CloseVault()
{
	return worker.SendCmd([=, this]
		{
			ResetState();
			Logger::Log("Closed vault");
			return Success;
		});
}

void VaultKeeper::ResetState()
{
	{
		std::lock_guard lock(chainMutex);
		mChain.clear();
	}

	vault.Reset();
	store.Reset();
	unsavedState.ClearChange();
	this->file = L"vault.bin";
}

void VaultKeeper::GetLastHint(std::string& str)
{
	std::lock_guard lock(chainMutex);
	if (!mChain.empty())
		str = mChain.back().hint;
}

Future VaultKeeper::SubmitPassword(const std::string_view& password)
{
	return worker.SendCmd([this](const SecureArray& password) -> uint64_t
		{
			auto pass = std::string_view(reinterpret_cast<const char*>(password.data()), password.size());
			if (pass.empty())
				return Failed;

			Logger::Log("Creating password key");
			auto salt = SecureArray::Copy(vault.GetStepSalt());
			auto key = vault.CreateKey(pass, salt);
			if (!key)
				return RaiseError("Failed to create password key");

			Logger::Log("Unlocking next layer");
			if (!vault.DecryptStep(key))
				return RaiseError("Invalid password");

			{
				std::lock_guard lock(chainMutex);
				auto& currentStep = mChain.back();
				currentStep.key = std::move(key);
				currentStep.salt = std::move(salt);
			}

			if (!vault.ReadStep())
				return RaiseError("Failed to read next layer", true);

			auto& block = vault.GetBlock();
			if (block)
			{
				Logger::Log("Deserializing content");

				auto size = strnlen_s(reinterpret_cast<const char*>(block.data()), block.size());
				if (!store.Deserialize(block.span(0, size)))
					return RaiseError("Failed to deserialize content", true);
				Logger::Log("Opened vault");

				vault.Reset();
				unsavedState.ClearChange();
				return Success;
			}

			Logger::Log("Preparing next layer");
			std::lock_guard lock(chainMutex);
			auto& hint = vault.GetStepName();
			mChain.push_back(Layer
				{
					.hint = std::string(reinterpret_cast<const char*>(hint.data()), hint.size()),
				});
			return UnlockRequired;
		}, SecureArray::CreateRef(password.data(), password.size()));
}

int VaultKeeper::AddHint(const std::string_view& hint)
{
	std::lock_guard lock(chainMutex);

	int index = mChain.size();
	mChain.push_back(Layer
		{
			.hint = std::string(hint),
		});

	unsavedState.NotifyChange();
	return index;
}

void VaultKeeper::RemoveHint(int i)
{
	std::lock_guard lock(chainMutex);
	if (i >= mChain.size() || i < 0)
		return;

	auto it1 = mChain.begin();
	for (int k = 1; k <= i; ++k)
	{
		++it1;
	}

	mChain.erase(it1);

	unsavedState.NotifyChange();
}

Future VaultKeeper::SetHintKey(int i, const std::string_view& password)
{
	return worker.SendCmd([=, this](const SecureArray& password) -> uint64_t
		{
			Logger::Log("Creating password key");
			auto pass = std::string_view(reinterpret_cast<const char*>(password.data()), password.size());
			SecureArray salt;
			auto key = vault.CreateKey(pass, salt);
			if (!key)
				return RaiseError("Failed to create password key");

			std::lock_guard lock(chainMutex);
			if (i >= mChain.size() || i < 0)
				return Failed;

			mChain[i].key = std::move(key);
			mChain[i].salt = std::move(salt);
			Logger::Log("Set password key");

			unsavedState.NotifyChange();
			return Success;
		}, SecureArray::CreateRef(password.data(), password.size()));
}

Future VaultKeeper::SaveVault()
{
	return worker.SendCmd([=, this]() -> uint64_t
			{
				Logger::Log("Serializing content");
				auto content = store.Serialize();
				if (content.empty())
				{
					RaiseError("Failed to serialize content");
					return Failed;
				}
				if (content.size() > INT32_MAX)
				{
					RaiseError("Failed to save vault - too much data (max 2GB)");
					return Failed;
				}

				Logger::Log("Placing vault");

				vault.Reset();
				vault.ResetSteps();

				for (auto& layer : mChain)
				{
					auto hint = SecureArray::CreateRef(layer.hint.data(), layer.hint.size());

					if (!vault.AddStep(hint, layer.key, layer.salt))
					{
						vault.Reset();
						RaiseError("Failed to set vault layers");
						return Failed;
					}
				}

				auto& block = vault.GetBlock();
				block = SecureArray(content.size() + (1024 - content.size() % 1024) % 1024);
				Crypto::ZeroMemory(block);
				block.copyFrom(content);

				if (!vault.Place(file))
				{
					vault.Reset();
					RaiseError(std::format("Failed to place vault in {}", StringUtils::WideStringToUtf8(file)));
					return Failed;
				}

				Logger::Log(L"Placed vault at {}", file);

				vault.Reset();
				unsavedState.ClearChange();
				return Success;
			});
}

int VaultKeeper::DirectApi::GetHintCount()
{
	return static_cast<int>(keeper.mChain.size());
}

std::string_view VaultKeeper::DirectApi::GetHint(int i)
{
	if (i >= keeper.mChain.size() || i < 0)
		return {};
	
	return keeper.mChain[i].hint;
}

bool VaultKeeper::DirectApi::IsKeyAssigned(int i)
{
	if (i >= keeper.mChain.size() || i < 0)
		return false;

	return (bool)keeper.mChain[i].key;
}

uint64_t VaultKeeper::RaiseError(const std::string_view& msg, bool critical)
{
	Logger::LogError("[VaultKeeper] {}", msg);
	VaultCommander::Get().ShowError(msg, critical);
	return Failed;
}

VaultKeeper::DirectApi VaultKeeper::GetDirectApi()
{
	return { .keeper = *this, .lock = std::unique_lock{ chainMutex } };
}

void VaultKeeper::ChangeHint(int i, const std::string_view& hint)
{
	std::lock_guard lock(chainMutex);
	if (i >= mChain.size() || i < 0)
		return;

	mChain[i].hint = hint;
	unsavedState.NotifyChange();
}
