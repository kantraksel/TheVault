#pragma once
#include "IRender.h"
#include "Applets/IApplet.h"
#include "Applets/WelcomeApplet.h"
#include "Applets/MainApplet.h"
#include "Applets/ProcessApplet.h"
#include "Applets/ErrorApplet.h"
#include "Modals.hpp"
#include "ReactiveScheduler.hpp"

class MainWindow : public IRender
{
private:
	enum class RenderContent
	{
		Welcome,
		GlobalProcess,
		CriticalError,
		MainView,
	};

	RenderContent content;
	IApplet* applet;

	WelcomeApplet welcome;
	MainApplet main;
	ProcessApplet process;
	ErrorApplet error;

	Modals modals;

	ReactiveScheduler scheduler;

	void SwitchApplet(RenderContent content, IApplet* applet);
	void SwitchToGlobalProcess();
	void SwitchToCriticalError();

	void RenderAppletBar();

public:
	MainWindow();
	~MainWindow();

	void Initialize();
	void Render() override;

	void SaveVault(bool closeVault = false, bool exitOnConfirm = false);
	void CloseVault(bool exitOnConfirm = false);

	void SwitchToWelcome();
	void SwitchToMainView();

	void ShowError(const std::string_view& text, bool critical);

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
		TR_CloseApp,
	};

	auto& GetModals()
	{
		return modals;
	}
};
