#include "MainWindow.h"
#include "Utils/ImGuiUtils.h"
#include "Utils/WinApi.h"
#include "Utility/StringUtils.h"
#include "App/Game.h"
#include "Engine/GhostFries.h"

extern Game game;

MainWindow::MainWindow()
{
	isDisabled = true;
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
		modals.Render();
		RenderDemoBar();
	}
	
	ImGui::End();

	game.GetVaultCommander().Update();
}

void MainWindow::RenderMainArea()
{
	bool hasDisabledStarted = false;
	if (isDisabled)
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

		auto& store = game.GetContentStore();
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
					Modals::GetShowPasswordWarningModal().Open(i);
				ImGui::SameLine();
				if (ImGui::Button("Copy"))
				{
					WinApi::SetClipboardText(store.GetText(i));
					ImGui::OpenPopup("password_copied");
				}
				ImGui::SameLine();
				if (ImGui::Button("Change"))
					Modals::GetChangePasswordModal().Open(i);

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
							Modals::GetErrorModal().Open("Failed to extract file - check permissions or logs for more details");
						}
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Change"))
					Modals::GetChangeFileModal().Open(i);
			}

			ImGui::SameLine();
			if (ImGui::Button("Delete"))
				Modals::GetDeleteEntryModal().Open(i);

			ImGui::PopID();
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		if (ImGui::Button("Add Password"))
			Modals::GetAddPasswordModal().Open();
		ImGui::SameLine();
		if (ImGui::Button("Add File"))
			Modals::GetAddFileModal().Open();

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
			if (ImGui::MenuItem("New", nullptr, false, false))
			{

			}

			if (ImGui::MenuItem("Open", "Ctrl+O", false, false))
			{

			}

			if (ImGui::MenuItem("Save", "Ctrl+S"))
				game.GetVaultCommander().SaveVault();
			if (ImGui::MenuItem("Close"))
				game.GetVaultCommander().TryCloseVault();

			ImGui::Separator();
			if (ImGui::MenuItem("Exit"))
				game.GetVaultCommander().TryCloseVault(true);

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
	{

	}

	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
		game.GetVaultCommander().SaveVault();
}

void MainWindow::DisableInteractions()
{
	isDisabled = true;
}

void MainWindow::EnableInteractions()
{
	isDisabled = false;
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
