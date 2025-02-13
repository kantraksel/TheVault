#include <filesystem>
#include "ImGuiUtils.h"
#include "Utility/StringUtils.h"
#include "MainApplet.h"
#include "../../Game.h"
#include "../../WinApi.h"
#include "../../StringUtils.h"

extern Game game;

MainApplet::MainApplet()
{
	openSelectAddModal = false;
	openAddTextModal = false;
	openAddFileModal = false;
	openShowTextBlockedModal = false;
	openShowTextModal = false;
	openChangeTextModal = false;
	openChangeFileModal = false;
	openChangeNameModal = false;
	openDeleteModal = false;
	modalIdx = 0;

	pwdInput.reserve(256);
	nameInput.reserve(256);
	wBuffer.reserve(256);
}

MainApplet::~MainApplet()
{
}

void MainApplet::Render()
{
	RenderMain();
	RenderSelectAddModal();
	RenderAddTextModal();
	RenderAddFileModal();
	RenderShowTextBlockModal();
	RenderShowTextModal();
	RenderChangeTextModal();
	RenderChangeFileModal();
	RenderChangeNameModal();
	RenderDeleteModal();
}

void MainApplet::OpenSelectAddModal()
{
	openSelectAddModal = true;
}

void MainApplet::OpenAddTextModal()
{
	openAddTextModal = true;
}

void MainApplet::OpenAddFileModal()
{
	openAddFileModal = true;
}

void MainApplet::OpenShowTextModal(int idx)
{
	modalIdx = idx;
	openShowTextBlockedModal = true;
}

void MainApplet::OpenShowTextUnblockedModal(int idx)
{
	modalIdx = idx;
	openShowTextModal = true;
}

void MainApplet::OpenChangeTextModal(int idx)
{
	modalIdx = idx;
	openChangeTextModal = true;
}

void MainApplet::OpenChangeFileModal(int idx)
{
	modalIdx = idx;
	openChangeFileModal = true;
}

void MainApplet::OpenChangeNameModal(int idx)
{
	modalIdx = idx;
	openChangeNameModal = true;
}

void MainApplet::OpenDeleteModal(int idx)
{
	modalIdx = idx;
	openDeleteModal = true;
}

void MainApplet::RenderMain()
{
	BeginVisibleChildWindow("##MainApplet", ImVec2(617, 200), ImGuiChildFlags_Border, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::MenuItem("Settings"))
			game.GetMainWindow().SwitchToLockSetup();
		if (ImGui::MenuItem("Save"))
			game.GetMainWindow().SaveVault();
		if (ImGui::MenuItem("Close"))
		{
			if (game.GetUnsavedState().HasChanged())
				game.GetMainWindow().OpenConfirmExitModal();
			else
				game.GetMainWindow().CloseVault();
		}
		ImGui::EndMenuBar();
	}

	constexpr auto flags = ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit;
	if (ImGui::BeginTable("MainContent", 3, flags, ImVec2(600, 100), ImGuiTableFlags_ScrollY))
	{
		ImGui::TableSetupColumn("No.");
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Password");
		ImGui::TableHeadersRow();

		auto& passMgr = game.GetPassManager();
		int hintCount = passMgr.GetCount();
		for (int i = 0; i < hintCount; ++i)
		{
			ImGui::PushID(i);
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			Text(StringUtils::ToString(i + 1));

			ImGui::TableNextColumn();
			auto name = passMgr.GetName(i);
			Text(name);

			ImGui::TableNextColumn();
			if (passMgr.IsPasswordText(i))
			{
				if (ImGui::Button("Show"))
					OpenShowTextModal(i);
				ImGui::SameLine();
				if (ImGui::Button("Copy"))
				{
					ImGui::SetClipboardText(passMgr.GetPassword(i).data());
				}
				ImGui::SameLine();
				if (ImGui::Button("Change"))
					OpenChangeTextModal(i);
			}
			else if (passMgr.IsPasswordFile(i))
			{
				if (ImGui::Button("Extract"))
				{
					wBuffer = StringUtils::Utf8ToWideString(name);
					if (WinApi::SaveFileDialog(L"Extract file", wBuffer, wBuffer))
						passMgr.ExtractFile(i, wBuffer);
					wBuffer.clear();
				}
				ImGui::SameLine();
				if (ImGui::Button("Update"))
					OpenChangeFileModal(i);
			}

			ImGui::SameLine();
			if (ImGui::Button("Delete"))
				OpenDeleteModal(i);

			ImGui::PopID();
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TableNextColumn();
		if (ImGui::Button("Add new password"))
			OpenSelectAddModal();
		ImGui::TableNextColumn();

		ImGui::EndTable();
	}
	ImGui::EndChild();
}

void MainApplet::RenderSelectAddModal()
{
	if (openSelectAddModal)
	{
		openSelectAddModal = false;
		ImGui::OpenPopup("Add password - select type");
	}

	if (ImGui::BeginPopupModal("Add password - select type", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		Text("Select password type");
		if (ImGui::Button("Text"))
		{
			ImGui::CloseCurrentPopup();
			OpenAddTextModal();
		}
		ImGui::SameLine();
		if (ImGui::Button("File"))
		{
			ImGui::CloseCurrentPopup();
			OpenAddFileModal();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}

void MainApplet::RenderAddTextModal()
{
	if (openAddTextModal)
	{
		openAddTextModal = false;
		ImGui::OpenPopup("Add password - TEXT");
	}

	if (ImGui::BeginPopupModal("Add password - TEXT", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("##Name", nameInput.data(), nameInput.capacity() - 1, ImGuiInputTextFlags_NoUndoRedo);
		constexpr auto flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo;
		bool submit = ImGui::InputText("##Password", pwdInput.data(), pwdInput.capacity() - 1, flags);
		bool submit2 = ImGui::Button("Set");

		if (submit || submit2)
		{
			auto name = std::string_view(nameInput.data());
			auto pwd = std::string_view(pwdInput.data());
			game.GetPassManager().Add(name, pwd);
			memset(pwdInput.data(), 0, pwdInput.capacity());
			nameInput.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			memset(pwdInput.data(), 0, pwdInput.capacity());
			nameInput.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void MainApplet::RenderAddFileModal()
{
	if (openAddFileModal)
	{
		openAddFileModal = false;
		ImGui::OpenPopup("Add password - FILE");
	}

	if (ImGui::BeginPopupModal("Add password - FILE", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("##Name", nameInput.data(), nameInput.capacity() - 1, ImGuiInputTextFlags_NoUndoRedo);
		if (ImGui::Button("Select file"))
		{
			if (WinApi::OpenFileDialog(L"Place new file in the vault", L"", wBuffer))
				if (nameInput.empty())
					nameInput = StringUtils::WideStringToUtf8(std::filesystem::path(wBuffer).filename().native());
		}
		Text(StringUtils::WideStringToUtf8(wBuffer));

		if (ImGui::Button("Set"))
		{
			if (!wBuffer.empty())
			{
				auto name = std::string_view(nameInput.data());
				game.GetPassManager().AddFile(name, wBuffer);
				wBuffer.clear();
				nameInput.clear();
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			wBuffer.clear();
			nameInput.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void MainApplet::RenderShowTextBlockModal()
{
	if (openShowTextBlockedModal)
	{
		openShowTextBlockedModal = false;
		ImGui::OpenPopup("Show password");
	}

	if (ImGui::BeginPopupModal("Show password", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		Text("Are you sure you want to show the password? Be sure not to expose it via screen sharing!");
		if (ImGui::Button("Yes"))
		{
			ImGui::CloseCurrentPopup();
			OpenShowTextUnblockedModal(modalIdx);
		}
		ImGui::SameLine();
		if (ImGui::Button("No"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}

void MainApplet::RenderShowTextModal()
{
	if (openShowTextModal)
	{
		openShowTextModal = false;
		ImGui::OpenPopup("Password details");
	}

	if (ImGui::BeginPopupModal("Password details", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		auto& passMgr = game.GetPassManager();
		auto pwd = passMgr.GetPassword(modalIdx);
		Text(passMgr.GetName(modalIdx));
		Text(pwd);

		if (ImGui::Button("Copy"))
		{
			ImGui::SetClipboardText(pwd.data());
		}
		ImGui::SameLine();
		if (ImGui::Button("Change"))
		{
			ImGui::CloseCurrentPopup();
			OpenChangeTextModal(modalIdx);
		}
		ImGui::SameLine();
		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}

void MainApplet::RenderChangeTextModal()
{
	if (openChangeTextModal)
	{
		openChangeTextModal = false;
		ImGui::OpenPopup("Change password - TEXT");
	}

	if (ImGui::BeginPopupModal("Change password - TEXT", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		auto& passMgr = game.GetPassManager();
		Text(passMgr.GetName(modalIdx));

		constexpr auto flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo;
		bool submit = ImGui::InputText("##Password", pwdInput.data(), pwdInput.capacity() - 1, flags);
		bool submit2 = ImGui::Button("Set");

		if (submit || submit2)
		{
			auto pwd = std::string_view(pwdInput.data());
			passMgr.Change(modalIdx, pwd);
			memset(pwdInput.data(), 0, pwdInput.capacity());
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			memset(pwdInput.data(), 0, pwdInput.capacity());
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Change name"))
		{
			ImGui::CloseCurrentPopup();
			OpenChangeNameModal(modalIdx);
		}

		ImGui::EndPopup();
	}
}

void MainApplet::RenderChangeFileModal()
{
	if (openChangeFileModal)
	{
		openChangeFileModal = false;
		ImGui::OpenPopup("Change password - FILE");
	}

	if (ImGui::BeginPopupModal("Change password - FILE", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static std::wstring file;

		auto& passMgr = game.GetPassManager();
		auto name = passMgr.GetName(modalIdx);
		Text(name);

		if (ImGui::Button("Select file"))
		{
			wBuffer = StringUtils::Utf8ToWideString(name);
			WinApi::OpenFileDialog(L"Replace file in vault", wBuffer, file);
			wBuffer.clear();
		}
		Text(StringUtils::WideStringToUtf8(file));

		if (ImGui::Button("Set"))
		{
			if (!file.empty())
			{
				passMgr.ChangeFile(modalIdx, file);
				file.clear();
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			file.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Change name"))
		{
			ImGui::CloseCurrentPopup();
			OpenChangeNameModal(modalIdx);
		}

		ImGui::EndPopup();
	}
}

void MainApplet::RenderChangeNameModal()
{
	if (openChangeNameModal)
	{
		openChangeNameModal = false;
		ImGui::OpenPopup("Change password name");

		nameInput = game.GetPassManager().GetName(modalIdx);
	}

	if (ImGui::BeginPopupModal("Change password name", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("##Name", nameInput.data(), nameInput.capacity() - 1, ImGuiInputTextFlags_NoUndoRedo);

		if (ImGui::Button("Set"))
		{
			auto name = std::string_view(nameInput.data());
			game.GetPassManager().ChangeName(modalIdx, name);
			nameInput.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			nameInput.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void MainApplet::RenderDeleteModal()
{
	if (openDeleteModal)
	{
		openDeleteModal = false;
		ImGui::OpenPopup("Delete password");
	}

	if (ImGui::BeginPopupModal("Delete password", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		Text("Are you sure you want to delete the password?");
		Text("Name: ");
		ImGui::SameLine();

		auto& passMgr = game.GetPassManager();
		Text(passMgr.GetName(modalIdx));
		if (ImGui::Button("Yes"))
		{
			passMgr.Remove(modalIdx);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("No"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}
