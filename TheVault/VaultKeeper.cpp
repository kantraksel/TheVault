#include <optional>
#include "VaultKeeper.h"
#include "Game.h"
#include "Crypto.h"
#include "Engine/Logger.h"
#include "Utility/StringUtils.h"

extern Game game;
using Future = VaultKeeper::Future;
using TaskRet = MainWindow::TaskRet;

struct Task
{
	std::function<uint64_t()> func;
	std::function<uint64_t(const SecureArray&)> func2;
	std::function<uint64_t(const std::wstring&)> func3;
	std::function<uint64_t(const std::string&)> func4;
	std::promise<uint64_t> promise;
	SecureArray arg;
	std::optional<std::wstring> arg2;
	std::optional<std::string> arg3;
};

VaultKeeper::VaultKeeper()
{
	file = L"vault.bin";
	mChain.reserve(16);
}

VaultKeeper::~VaultKeeper()
{
	Shutdown();
}

void VaultKeeper::Init()
{
	if (thread.joinable())
		return;
	using namespace std::placeholders;
	thread = std::jthread(std::bind(&VaultKeeper::Run, this, _1));
}

void VaultKeeper::Shutdown()
{
	thread.request_stop();
	threadCvar.notify_one();

	if (thread.joinable())
		thread.join();
	thread = {};

	tasks.clear();
}

void VaultKeeper::Run(std::stop_token token)
{
	std::unique_lock lock(taskMutex);

	while (true)
	{
		if (token.stop_requested())
			return;

		if (tasks.empty())
		{
			threadCvar.wait(lock);
			if (tasks.empty())
				continue;

			if (token.stop_requested())
				return;
		}
		auto& task = tasks.front();

		lock.unlock();
		uint64_t value = 0;
		if (task.func)
			value = task.func();
		else if (task.func2)
			value = task.func2(task.arg);
		else if (task.func3)
			value = task.func3(*task.arg2);
		else if (task.func4)
			value = task.func4(*task.arg3);
		lock.lock();

		task.promise.set_value(value);
		tasks.pop_front();
	}
}

Future VaultKeeper::SendCmd(const std::function<uint64_t()>& f)
{
	Task task{ f };
	auto future = task.promise.get_future();

	{
		std::lock_guard lock(taskMutex);
		tasks.push_back(std::move(task));
	}
	threadCvar.notify_one();
	return future;
}

Future VaultKeeper::SendCmd(const std::function<uint64_t(const SecureArray&)>& f, const SecureArray& arg)
{
	auto mem = Crypto::CopyMemory(arg);
	if (!mem)
		return {};

	Task task;
	task.arg = std::move(mem);
	task.func2 = f;
	auto future = task.promise.get_future();

	{
		std::lock_guard lock(taskMutex);
		tasks.push_back(std::move(task));
	}
	threadCvar.notify_one();
	return future;
}

Future VaultKeeper::SendCmd(const std::function<uint64_t(const std::wstring&)>& f, const std::wstring_view& arg)
{
	Task task;
	task.arg2 = arg;
	task.func3 = f;
	auto future = task.promise.get_future();

	{
		std::lock_guard lock(taskMutex);
		tasks.push_back(std::move(task));
	}
	threadCvar.notify_one();
	return future;
}

Future VaultKeeper::SendCmd(const std::function<uint64_t(const std::string&)>& f, const std::string_view& arg)
{
	Task task;
	task.arg3 = arg;
	task.func4 = f;
	auto future = task.promise.get_future();

	{
		std::lock_guard lock(taskMutex);
		tasks.push_back(std::move(task));
	}
	threadCvar.notify_one();
	return future;
}

Future VaultKeeper::OpenVault(const std::wstring_view& file)
{
	using namespace std::placeholders;
	return SendCmd(std::bind(&VaultKeeper::OpenVaultDeferred, this, _1), file);
}

uint64_t VaultKeeper::OpenVaultDeferred(const std::wstring& file)
{
	auto& vault = game.GetVault();
	if (!vault.Open(file))
	{
		RaiseError(std::format("Failed to open vault {}", StringUtils::WideStringToUtf8(file)));
		return TaskRet::TR_SwitchToWelcome;
	}
	Logger::Log(L"Opened vault {}", file);

	this->file = file;
	{
		auto& hint = vault.GetStepName();

		std::lock_guard lock(chainMutex);
		mChain.push_back(Layer
			{
				.hint = std::string(hint.str(), hint.size()),
			});
	}
	Logger::Log("Unlocked first hint");
	return TaskRet::TR_SwitchToLogin;
}

Future VaultKeeper::CreateVault(const std::wstring_view& file)
{
	using namespace std::placeholders;
	return SendCmd(std::bind(&VaultKeeper::CreateVaultDeferred, this, _1), file);
}

uint64_t VaultKeeper::CreateVaultDeferred(const std::wstring& file)
{
	auto& vault = game.GetVault();
	Logger::Log("Preparing new vault");

	this->file = file;
	Logger::Log(L"Set vault path to {}", file);
	vault.Reset();

	Logger::Log("Prepared new vault");
	game.GetUnsavedState().NotifyChange();
	return TaskRet::TR_SwitchToLockSetup;
}

Future VaultKeeper::CloseVault()
{
	return SendCmd(std::bind(&VaultKeeper::CloseVaultDeferred, this));
}

uint64_t VaultKeeper::CloseVaultDeferred()
{
	//if changed, save

	{
		std::lock_guard lock(chainMutex);
		mChain.clear();
	}
	
	game.GetVault().Reset();
	game.GetPassManager().Reset();
	this->file = L"vault.bin";
	Logger::Log("Closed vault");

	game.GetUnsavedState().ClearChange();
	return TaskRet::TR_SwitchToWelcome;
}

void VaultKeeper::GetLastHint(std::string& str)
{
	std::lock_guard lock(chainMutex);
	if (!mChain.empty())
		str = mChain.back().hint;
}

Future VaultKeeper::SubmitPassword(const SecureArray& password)
{
	using namespace std::placeholders;
	return SendCmd(std::bind(&VaultKeeper::SubmitPasswordDeferred, this, _1), password);
}

uint64_t VaultKeeper::SubmitPasswordDeferred(const SecureArray& password)
{
	auto size = strnlen_s(password.str(), password.size());
	auto pass = std::string_view(password.str(), size);
	if (pass.empty())
		return TaskRet::TR_Failed;

	Logger::Log("Creating password key");
	auto& vault = game.GetVault();
	auto salt = Crypto::CopyMemory(vault.GetStepSalt());
	auto key = vault.CreateKey(pass, salt);
	if (!key)
		return RaiseError("Failed to create password key");

	Logger::Log("Unlocking next hint");
	
	auto& currentStep = mChain.back();
	if (!vault.UnlockStep(key))
		return RaiseError("Failed to unlock next hint");

	{
		auto& hint = vault.GetStepName();

		std::lock_guard lock(chainMutex);
		currentStep.key = std::move(key);
		currentStep.salt = std::move(salt);
		if (!vault.GetBlock())
		{
			mChain.push_back(Layer
				{
					.hint = std::string(hint.str(), hint.size()),
				});
		}
	}
	Logger::Log("Unlocked next hint");

	auto& block = vault.GetBlock();
	if (block)
	{
		Logger::Log("Deserializing content");
		auto& passMgr = game.GetPassManager();
		if (!passMgr.Deserialize(std::string_view(block.str(), block.size())))
			return RaiseError("Failed to deserialize content", true);
		Logger::Log("Opened vault");

		vault.Reset();
		game.GetUnsavedState().ClearChange();
		return TaskRet::TR_SwitchToMainView;
	}
	return TaskRet::TR_FetchNextHint;
}

void VaultKeeper::AddHint(const std::string_view& hint)
{
	std::lock_guard lock(chainMutex);
	mChain.push_back(Layer
		{
			.hint = std::string(hint),
		});

	game.GetUnsavedState().NotifyChange();
}

void VaultKeeper::RemoveHint(int i)
{
	if (i >= mChain.size() || i < 0)
		return;

	auto it1 = mChain.begin();
	for (int k = 1; k <= i; ++k)
	{
		++it1;
	}

	mChain.erase(it1);

	game.GetUnsavedState().NotifyChange();
}

Future VaultKeeper::SetHintKey(int i, const SecureArray& password)
{
	using namespace std::placeholders;
	return SendCmd(std::bind(&VaultKeeper::SetHintKeyDeferred, this, i, _1), password);
}

uint64_t VaultKeeper::SetHintKeyDeferred(int i, const SecureArray& password)
{
	if (i >= mChain.size() || i < 0)
		return TaskRet::TR_Failed;

	//this check should be in window
	auto size = strnlen_s(password.str(), password.size());
	auto pass = std::string_view(password.str(), size);
	if (pass.empty())
		return TaskRet::TR_Failed;

	Logger::Log("Creating password key");
	SecureArray salt;
	auto key = game.GetVault().CreateKey(pass, salt);
	if (!key)
		return RaiseError("Failed to create password key");

	mChain[i].key = std::move(key);
	mChain[i].salt = std::move(salt);
	Logger::Log("Added hint key");

	game.GetUnsavedState().NotifyChange();
	return TaskRet::TR_Success;
}

Future VaultKeeper::SaveVault()
{
	return SendCmd(std::bind(&VaultKeeper::SaveVaultDeferred, this, false));
}

Future VaultKeeper::SaveCloseVault()
{
	return SendCmd(std::bind(&VaultKeeper::SaveVaultDeferred, this, true));
}

uint64_t VaultKeeper::SaveVaultDeferred(bool close)
{
	//these checks should be in window
	if (mChain.empty())
		return RaiseError("Vault must be encrypted with at least one hint");

	for (auto& key : mChain)
	{
		if (!key.key || !key.salt)
		{
			RaiseError("All hint keys must be set");
			return TaskRet::TR_SwitchToLockSetup;
		}
	}

	Logger::Log("Serializing content");
	auto content = game.GetPassManager().Serialize();
	if (content.empty())
		return RaiseError("Failed to serialize content");

	std::lock_guard lock(chainMutex);
	Logger::Log("Placing vault");

	auto& vault = game.GetVault();
	vault.ResetSteps();

	for (int i = 0; i < mChain.size(); ++i)
	{
		auto& layer = mChain[i];
		auto hint = SecureArray::Wrap(layer.hint.data(), layer.hint.size(), nullptr);

		if (!vault.AddStep(hint, layer.key, layer.salt))
		{
			RaiseError("Failed to lock hint");
			return TaskRet::TR_SwitchToLockSetup;
		}
	}

	vault.GetBlock() = SecureArray::Wrap(content.data(), content.size(), nullptr);
	if (!vault.Place(file))
	{
		RaiseError(std::format("Failed to place vault in {}", StringUtils::WideStringToUtf8(file)));
		return TaskRet::TR_SwitchToLockSetup;
	}

	Logger::Log(L"Placed vault at {}", file);

	vault.Reset();
	game.GetUnsavedState().ClearChange();
	if (close)
		return TaskRet::TR_CloseVault;
	return TaskRet::TR_SwitchToMainView;
}

int VaultKeeper::GetHintCount()
{
	return static_cast<int>(mChain.size());
}

std::string_view VaultKeeper::GetHint(int i)
{
	if (i >= mChain.size() || i < 0)
		return {};
	
	return mChain[i].hint;
}

bool VaultKeeper::IsKeyAssigned(int i)
{
	if (i >= mChain.size() || i < 0)
		return false;

	return mChain[i].key;
}

uint64_t VaultKeeper::RaiseError(const std::string_view& msg, bool critical)
{
	Logger::LogError(msg);
	game.GetMainWindow().ShowError(msg, critical);
	return critical ? TaskRet::TR_CriticalError : TaskRet::TR_Failed;
}

void VaultKeeper::LockDirectApi()
{
	chainMutex.lock();
}

void VaultKeeper::UnlockDirectApi()
{
	chainMutex.unlock();
}

void VaultKeeper::ChangeHint(int i, const std::string_view& hint)
{
	if (i >= mChain.size() || i < 0)
		return;

	mChain[i].hint = hint;
	game.GetUnsavedState().NotifyChange();
}
