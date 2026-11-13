#pragma once
#include "IRender.h"
#include "Applets/IApplet.h"
#include "Applets/MainApplet.h"
#include "Modals.hpp"
#include "ReactiveScheduler.hpp"

class MainWindow : public IRender
{
private:
	enum class RenderContent
	{
		MainView,
	};

	RenderContent content;
	IApplet* applet;

	MainApplet main;

	Modals modals;

	ReactiveScheduler scheduler;

	void SwitchApplet(RenderContent content, IApplet* applet);

	void RenderAppletBar();

public:
	MainWindow();
	~MainWindow();

	void Initialize();
	void Render() override;

	void CreateVault(const std::wstring& file);
	void OpenVault(const std::wstring& file);
	void SaveVault(bool closeVault = false, bool exitOnConfirm = false);
	void CloseVault(bool exitOnConfirm = false);

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
