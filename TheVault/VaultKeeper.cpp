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
	mHints.reserve(16);
	mKeyChain.reserve(16);
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

	Logger::Log("Unlocking first hint");
	auto key = Crypto::CopyMemory(vault.GetFirstKey());
	if (!key)
		return RaiseError("Failed to allocate memory", true);

	SecureArray hint;
	if (!vault.UnlockStep(key, 0, hint))
	{
		RaiseError("Failed to unlock first hint");
		return CloseVaultDeferred();
	}

	this->file = file;
	{
		std::lock_guard lock(hintMutex);
		mHints.push_back(std::string(hint.str(), hint.size()));
		mKeyChain.push_back(std::move(key));
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
	vault.GenerateNew();

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
		std::lock_guard lock(hintMutex);
		mHints.clear();
		mKeyChain.clear();
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
	std::lock_guard lock(hintMutex);
	if (!mHints.empty())
		str = mHints.back();
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
	auto key = vault.CreateKey(pass);
	if (!key)
		return RaiseError("Failed to create password key");

	// check last hint (next hint doesn't exist, so we can't decrypt it)
	if (mKeyChain.size() == vault.GetLockSteps())
	{
		Logger::Log("Unlocking block");

		SecureArray master;
		{
			std::lock_guard lock(hintMutex);
			master = vault.CreateMasterKey(mKeyChain, key);
			if (!master)
				return RaiseError("Failed to create master key", true);
		}
		
		if (!vault.UnlockBlock(master))
			return RaiseError("Failed to unlock the block");

		{
			std::lock_guard lock(hintMutex);
			// delete predef key - create encryptor+hint pairs
			mKeyChain.erase(mKeyChain.begin());
			mKeyChain.push_back(std::move(key));
		}

		Logger::Log("Deserializing content");
		auto& passMgr = game.GetPassManager();
		auto& block = vault.GetBlock();
		if (!passMgr.Deserialize(std::string_view(block.str(), block.size())))
			return RaiseError("Failed to deserialize content", true);
		Logger::Log("Opened vault");
		
		vault.ResetCache();
		game.GetUnsavedState().ClearChange();
		return TaskRet::TR_SwitchToMainView;
	}
	Logger::Log("Unlocking next hint");
	
	SecureArray hint;
	if (!vault.UnlockStep(key, (int)mHints.size(), hint))
		return RaiseError("Failed to unlock next hint");

	{
		std::lock_guard lock(hintMutex);
		mHints.push_back(std::string(hint.str(), hint.size()));
		mKeyChain.push_back(std::move(key));
	}
	Logger::Log("Unlocked next hint");
	return TaskRet::TR_FetchNextHint;
}

void VaultKeeper::AddHint(const std::string_view& hint)
{
	std::lock_guard lock(hintMutex);
	mHints.push_back(std::string(hint));
	mKeyChain.resize(mHints.size());

	game.GetUnsavedState().NotifyChange();
}

void VaultKeeper::RemoveHint(int i)
{
	if (i >= mHints.size() || i < 0)
		return;

	auto it1 = mHints.begin();
	auto it2 = mKeyChain.begin();
	for (int k = 1; k <= i; ++k)
	{
		++it1;
		++it2;
	}

	mHints.erase(it1);
	mKeyChain.erase(it2);

	game.GetUnsavedState().NotifyChange();
}

Future VaultKeeper::SetHintKey(int i, const SecureArray& password)
{
	using namespace std::placeholders;
	return SendCmd(std::bind(&VaultKeeper::SetHintKeyDeferred, this, i, _1), password);
}

uint64_t VaultKeeper::SetHintKeyDeferred(int i, const SecureArray& password)
{
	if (i >= mKeyChain.size() || i < 0)
		return TaskRet::TR_Failed;

	//this check should be in window
	auto size = strnlen_s(password.str(), password.size());
	auto pass = std::string_view(password.str(), size);
	if (pass.empty())
		return TaskRet::TR_Failed;

	Logger::Log("Creating password key");
	auto key = game.GetVault().CreateKey(pass);
	if (!key)
		return RaiseError("Failed to create password key");

	mKeyChain[i] = std::move(key);
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
	if (mHints.empty())
		return RaiseError("Vault must be encrypted with at least one hint");

	for (auto& key : mKeyChain)
	{
		if (!key)
		{
			RaiseError("All hint keys must be set");
			return TaskRet::TR_SwitchToLockSetup;
		}
	}

	Logger::Log("Serializing content");
	auto content = game.GetPassManager().Serialize();
	if (content.empty())
		return RaiseError("Failed to serialize content");

	std::lock_guard lock(hintMutex);
	Logger::Log("Placing vault");

	auto& vault = game.GetVault();
	vault.ResetSteps();

	// move keys - create encryptor+next hint pairs
	auto key = Crypto::CopyMemory(vault.GetFirstKey());
	if (!key)
		return RaiseError("Failed to allocate memory", true);
	mKeyChain.insert(mKeyChain.begin(), std::move(key));

	for (int i = 0; i < mHints.size(); ++i)
	{
		auto& hint = mHints[i];
		auto shint = SecureArray::Wrap(hint.data(), hint.size(), nullptr);

		if (!vault.AddStep(shint, mKeyChain[i]))
		{
			// move keys - create encryptor+hint pairs
			mKeyChain.erase(mKeyChain.begin());

			RaiseError("Failed to lock hint");
			return TaskRet::TR_SwitchToLockSetup;
		}
	}

	key = vault.CreateMasterKey(mKeyChain, {});
	// move keys - create encryptor+hint pairs
	mKeyChain.erase(mKeyChain.begin());

	if (!key)
	{
		RaiseError("Failed to create master key");
		return TaskRet::TR_SwitchToLockSetup;
	}

	if (!vault.LockBlock(key, content))
	{
		RaiseError("Failed to lock block");
		return TaskRet::TR_SwitchToLockSetup;
	}

	if (!vault.Place(file))
	{
		RaiseError(std::format("Failed to place vault in {}", StringUtils::WideStringToUtf8(file)));
		return TaskRet::TR_SwitchToLockSetup;
	}

	Logger::Log(L"Placed vault at {}", file);

	vault.ResetCache();
	game.GetUnsavedState().ClearChange();
	if (close)
		return TaskRet::TR_CloseVault;
	return TaskRet::TR_SwitchToMainView;
}

int VaultKeeper::GetHintCount()
{
	return (int)mHints.size();
}

std::string_view VaultKeeper::GetHint(int i)
{
	if (i >= mHints.size() || i < 0)
		return {};
	
	return mHints[i];
}

bool VaultKeeper::IsKeyAssigned(int i)
{
	if (i >= mKeyChain.size() || i < 0)
		return false;

	return mKeyChain[i];
}

uint64_t VaultKeeper::RaiseError(const std::string_view& msg, bool critical)
{
	Logger::LogError(msg);
	game.GetMainWindow().ShowError(msg, critical);
	return critical ? TaskRet::TR_CriticalError : TaskRet::TR_Failed;
}

void VaultKeeper::LockDirectApi()
{
	hintMutex.lock();
}

void VaultKeeper::UnlockDirectApi()
{
	hintMutex.unlock();
}

void VaultKeeper::ChangeHint(int i, const std::string_view& hint)
{
	if (i >= mHints.size() || i < 0)
		return;

	mHints[i] = hint;
	game.GetUnsavedState().NotifyChange();
}

void VaultKeeper::ResetSalts()
{
	game.GetVault().GenerateNew();
	
	for (auto& key : mKeyChain)
	{
		key.reset();
	}

	game.GetUnsavedState().NotifyChange();
}
