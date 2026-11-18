#pragma once
#include "Applets/LockApplet.h"
#include "Applets/LoginApplet.h"
#include "Applets/WelcomeApplet.h"
#include "Applets/ProcessApplet.h"
#include "Modals/AddFileModal.hpp"
#include "Modals/AddPasswordModal.hpp"
#include "Modals/ChangeFileModal.hpp"
#include "Modals/ChangeNameModal.hpp"
#include "Modals/ChangePasswordModal.hpp"
#include "Modals/ConfirmExitModal.hpp"
#include "Modals/DeleteEntryModal.hpp"
#include "Modals/ErrorModal.hpp"
#include "Modals/ShowPasswordModal.hpp"
#include "Modals/ShowPasswordWarningModal.hpp"

class MainWindow
{
private:
	LockApplet lockApplet;
	LoginApplet loginApplet;
	WelcomeApplet welcomeApplet;
	ProcessApplet processApplet;
	AddPasswordModal addPasswordModal;
	AddFileModal addFileModal;
	ShowPasswordWarningModal showPasswordWarningModal;
	ShowPasswordModal showPasswordModal;
	ChangePasswordModal changePasswordModal;
	ChangeFileModal changeFileModal;
	ChangeNameModal changeNameModal;
	DeleteEntryModal deleteEntryModal;
	ConfirmExitModal confirmExitModal;
	ErrorModal errorModal;
	int disabledCount;

	void RenderMainArea();
	void RenderMenuBar();
	void ProcessShortcuts();
	void RenderDemoBar();

public:
	static MainWindow& Get();

	MainWindow();
	~MainWindow();

	void Render();
	void DisableInteractions();
	void EnableInteractions();

	auto& GetLockApplet()
	{
		return lockApplet;
	}

	auto& GetLoginApplet()
	{
		return loginApplet;
	}

	auto& GetWelcomeApplet()
	{
		return welcomeApplet;
	}

	auto& GetProcessApplet()
	{
		return processApplet;
	}

	auto& GetErrorModal()
	{
		return errorModal;
	}

	auto& GetConfirmExitModal()
	{
		return confirmExitModal;
	}
};
