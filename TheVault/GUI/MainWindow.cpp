#include "MainWindow.h"
#include "VaultCommander.hpp"
#include "App/ContentStore.h"
#include "Utils/ImGuiUtils.h"
#include "Utils/WinApi.h"
#include "Utility/StringUtils.h"

MainWindow::MainWindow() : lockApplet(deleteEntryModal, changeNameModal), loginApplet(*this), welcomeApplet(*this),
                           processApplet(*this), showPasswordWarningModal(showPasswordModal),
                           showPasswordModal(changePasswordModal), changePasswordModal(changeNameModal),
                           changeFileModal(changeNameModal)
{
	disabledCount = 0;
	welcomeApplet.Open();
}

MainWindow::~MainWindow()
{
}

void MainWindow::Render()
{
	auto* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
	if (ImGui::Begin("TheVault##MainWindow", nullptr, flags))
	{
		RenderMainArea();
		loginApplet.Render();
		welcomeApplet.Render();
		processApplet.Render();
		addPasswordModal.Render();
		addFileModal.Render();
		showPasswordWarningModal.Render();
		showPasswordModal.Render();
		changePasswordModal.Render();
		changeFileModal.Render();
		changeNameModal.Render();
		deleteEntryModal.Render();
		confirmExitModal.Render();
		errorModal.Render();
		RenderDemoBar();
	}
	
	ImGui::End();

	VaultCommander::Get().Update();
}

void MainWindow::RenderMainArea()
{
	bool hasDisabledStarted = false;
	if (disabledCount)
	{
		hasDisabledStarted = true;
		ImGui::BeginDisabled();
	}
	RenderMenuBar();
	ProcessShortcuts();

	constexpr auto flags = ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY;
	if (ImGui::BeginTable("MainContent", 2, flags))
	{
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Actions");
		ImGui::TableHeadersRow();

		auto& store = ContentStore::Get();
		auto lock = store.AcquireAccessLock();
		int hintCount = store.GetCount();
		for (int i = 0; i < hintCount; ++i)
		{
			ImGui::PushID(i);
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			TextWidget(store.GetName(i));

			ImGui::TableNextColumn();
			if (store.IsText(i))
			{
				if (ImGui::Button("Show"))
					showPasswordWarningModal.Open(i);
				ImGui::SameLine();
				if (ImGui::Button("Copy"))
				{
					WinApi::SetClipboardText(store.GetText(i));
					ImGui::OpenPopup("password_copied");
				}
				ImGui::SameLine();
				if (ImGui::Button("Change"))
					changePasswordModal.Open(i);

				if (ImGui::BeginPopup("password_copied"))
				{
					Text("Copied to clipboard");
					ImGui::EndPopup();
				}
			}
			else if (store.IsFile(i))
			{
				auto& style = ImGui::GetStyle();

				auto textSize = ImGui::CalcTextSize("Show", nullptr, true);
				float width = textSize.x + style.FramePadding.x * 2.0f;
				textSize = ImGui::CalcTextSize("Copy", nullptr, true);
				width += textSize.x + style.FramePadding.x * 2.0f;
				width += style.ItemSpacing.x;

				if (ImGui::Button("Extract", ImVec2(width, 0)))
				{
					auto fileName = StringUtils::Utf8ToWideString(store.GetName(i));
					if (WinApi::SaveFileDialog(L"Extract file", fileName, fileName))
					{
						if (!store.ExtractFile(i, fileName))
						{
							errorModal.Open("Failed to extract file - check permissions or logs for more details");
						}
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Change"))
					changeFileModal.Open(i);
			}

			ImGui::SameLine();
			if (ImGui::Button("Delete"))
				deleteEntryModal.Open(i);

			ImGui::PopID();
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		if (ImGui::Button("Add Password"))
			addPasswordModal.Open();
		ImGui::SameLine();
		if (ImGui::Button("Add File"))
			addFileModal.Open();

		ImGui::TableNextColumn();
		ImGui::BeginDisabled();
		ImGui::Button("Show");
		ImGui::SameLine();
		ImGui::Button("Copy");
		ImGui::SameLine();
		ImGui::Button("Change");
		ImGui::SameLine();
		ImGui::Button("Delete");
		ImGui::EndDisabled();

		ImGui::EndTable();
	}

	lockApplet.Render();
	if (hasDisabledStarted)
		ImGui::EndDisabled();
}

void MainWindow::RenderMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
				VaultCommander::Get().CreateVault();

			if (ImGui::MenuItem("Open", "Ctrl+O"))
				VaultCommander::Get().OpenVault();

			if (ImGui::MenuItem("Save", "Ctrl+S"))
				VaultCommander::Get().SaveVault();
			if (ImGui::MenuItem("Close"))
				VaultCommander::Get().TryCloseVault();

			ImGui::Separator();
			if (ImGui::MenuItem("Exit"))
				VaultCommander::Get().TryCloseVault(true);

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Locks", nullptr, lockApplet.IsOpen()))
		{
			if (lockApplet.IsOpen())
				lockApplet.Close();
			else
				lockApplet.Open();
		}

		if (ImGui::MenuItem("About", nullptr, false, false))
		{

		}

		ImGui::EndMenuBar();
	}
}

void MainWindow::ProcessShortcuts()
{
	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O))
		VaultCommander::Get().OpenVault();

	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
		VaultCommander::Get().SaveVault();
}

void MainWindow::DisableInteractions()
{
	++disabledCount;
}

void MainWindow::EnableInteractions()
{
	if (disabledCount)
		--disabledCount;
}

void MainWindow::RenderDemoBar()
{
#if _DEBUG
	static bool showDemoWindow = false;
	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Debug"))
		{
			if (ImGui::MenuItem("DemoWindow", nullptr, showDemoWindow))
				showDemoWindow = !showDemoWindow;

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
#endif
}
