#pragma once
#include "../IRender.h"
#include "IApplet.h"
#include "WelcomeApplet.h"
#include "LoginApplet.h"
#include "LockApplet.h"
#include "MainApplet.h"
#include "ProcessApplet.h"
#include "ErrorApplet.h"

class MainWindow : public IRender
{
private:
	enum class RenderContent
	{
		Welcome,
		GlobalProcess,
		CriticalError,
		LoginChallenge,
		MainView,
		LockSetup,

		DemoWindow,
	};

	RenderContent content;
	IApplet* applet;

	WelcomeApplet welcome;
	LoginApplet login;
	LockApplet lock;
	MainApplet main;
	ProcessApplet process;
	ErrorApplet error;
	
	bool openConfirmExitModal;
	void RenderConfirmExitModal();

	void SwitchApplet(RenderContent content, IApplet* applet);
	void SwitchToGlobalProcess();
	void SwitchToCriticalError();

public:
	MainWindow();
	~MainWindow();

	void Initialize();
	void Render() override;

	void SaveVault();
	void CloseVault();

	void SwitchToWelcome();
	void SwitchToLoginChallenge();
	void SwitchToMainView();
	void SwitchToLockSetup();

	void ShowError(const std::string_view& text, bool critical);
	void OpenConfirmExitModal();

	void ProcessVaultTask(std::future<uint64_t>&& task, const std::string_view& title = {});
	uint64_t ProcessVaultResponse(std::future<uint64_t>& task);

	enum TaskRet
	{
		TR_Failed,
		TR_Success,
		_TR_NoAction, // reserved for MainWindow
		TR_SwitchToWelcome,
		TR_SwitchToLogin,
		TR_SwitchToMainView,
		TR_SwitchToLockSetup,
		TR_FetchNextHint,
		TR_CriticalError,
		TR_CloseVault,
	};
};
