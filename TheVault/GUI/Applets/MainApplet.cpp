#include <filesystem>
#include "Utils/ImGuiUtils.h"
#include "Utility/StringUtils.h"
#include "MainApplet.h"
#include "App/Game.h"
#include "Utils/WinApi.h"

extern Game game;

MainApplet::MainApplet()
{
	openAddTextModal = false;
	openAddFileModal = false;
	openShowTextBlockedModal = false;
	openShowTextModal = false;
	openChangeTextModal = false;
	openChangeFileModal = false;
	openChangeNameModal = false;
	openDeleteModal = false;
	openErrorModal = false;
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
	RenderAddTextModal();
	RenderAddFileModal();
	RenderShowTextBlockModal();
	RenderShowTextModal();
	RenderChangeTextModal();
	RenderChangeFileModal();
	RenderChangeNameModal();
	RenderDeleteModal();
	RenderErrorModal();
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

void MainApplet::OpenErrorModal(const std::string_view& msg)
{
	message = msg;
	openErrorModal = true;
}

void MainApplet::RenderMain()
{
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
	if (ImGui::BeginTable("MainContent", 2, flags, ImVec2(0, 0), ImGuiTableFlags_ScrollY))
	{
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Actions");
		ImGui::TableHeadersRow();

		auto& store = game.GetContentStore();
		int hintCount = store.GetCount();
		for (int i = 0; i < hintCount; ++i)
		{
			ImGui::PushID(i);
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			Text(store.GetName(i));

			ImGui::TableNextColumn();
			if (store.IsText(i))
			{
				if (ImGui::Button("Show"))
					OpenShowTextModal(i);
				ImGui::SameLine();
				if (ImGui::Button("Copy"))
				{
					WinApi::SetClipboardText(store.GetText(i));
				}
				ImGui::SameLine();
				if (ImGui::Button("Change"))
					OpenChangeTextModal(i);
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
					wBuffer = StringUtils::Utf8ToWideString(store.GetName(i));
					if (WinApi::SaveFileDialog(L"Extract file", wBuffer, wBuffer))
					{
						if (!store.ExtractFile(i, wBuffer))
						{
							OpenErrorModal("Failed to extract file - check permissions or logs for more details");
						}
					}
					wBuffer.clear();
				}
				ImGui::SameLine();
				if (ImGui::Button("Change"))
					OpenChangeFileModal(i);
			}

			ImGui::SameLine();
			if (ImGui::Button("Delete"))
				OpenDeleteModal(i);

			ImGui::PopID();
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		if (ImGui::Button("Add Password"))
			OpenAddTextModal();
		ImGui::SameLine();
		if (ImGui::Button("Add File"))
			OpenAddFileModal();

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
}

void MainApplet::RenderAddTextModal()
{
	if (openAddTextModal)
	{
		openAddTextModal = false;
		ImGui::OpenPopup("Add Password");
	}

	if (ImGui::BeginPopupModal("Add Password", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		Text("Name:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputText("##Name", nameInput.data(), nameInput.capacity() - 1, ImGuiInputTextFlags_NoUndoRedo);

		Text("Value:");
		ImGui::SameLine();
		constexpr auto flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo;
		bool submit = ImGui::InputText("##Password", pwdInput.data(), pwdInput.capacity() - 1, flags);
		bool submit2 = ImGui::Button("Add");

		if (submit || submit2)
		{
			auto name = std::string_view(nameInput.data());
			auto pwd = std::string_view(pwdInput.data());
			game.GetContentStore().AddText(name, pwd);
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
		ImGui::OpenPopup("Add File");
	}

	if (ImGui::BeginPopupModal("Add File", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		Text("Name:");
		ImGui::SameLine();
		ImGui::InputText("##Name", nameInput.data(), nameInput.capacity() - 1, ImGuiInputTextFlags_NoUndoRedo);
		if (ImGui::Button("Select File"))
		{
			if (WinApi::OpenFileDialog(L"Place new file in the vault", L"", wBuffer))
				if (std::string_view(nameInput.data()).empty())
					nameInput = StringUtils::WideStringToUtf8(std::filesystem::path(wBuffer).filename().native());
		}
		ImGui::SameLine();
		if (wBuffer.empty())
			Text("(not selected)");
		else
			Text(StringUtils::WideStringToUtf8(std::filesystem::path(wBuffer).filename().native()));

		if (ImGui::Button("Add"))
		{
			if (!wBuffer.empty())
			{
				auto name = std::string_view(nameInput.data());
				if (!game.GetContentStore().AddFile(name, wBuffer))
				{
					OpenErrorModal("Failed to read file - check permissions or logs for more details");
				}
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
		ImGui::OpenPopup("Password Details - Warning");
	}

	if (ImGui::BeginPopupModal("Password Details - Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		Text("Are you sure you want to show the password?\nBe sure to not expose it via screen sharing!");
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
		ImGui::OpenPopup("Password Details");
	}

	if (ImGui::BeginPopupModal("Password Details", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		auto& store = game.GetContentStore();
		Text("Name:");
		ImGui::SameLine();
		Text(store.GetName(modalIdx));
		Text("Value:");
		ImGui::SameLine();
		Text(store.GetText(modalIdx));

		if (ImGui::Button("Copy"))
		{
			WinApi::SetClipboardText(store.GetText(modalIdx));
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
		ImGui::OpenPopup("Change Password");
	}

	if (ImGui::BeginPopupModal("Change Password", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		auto& store = game.GetContentStore();
		Text("Name:");
		ImGui::SameLine();
		Text(store.GetName(modalIdx));

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		constexpr auto flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo;
		bool submit = ImGui::InputText("##Password", pwdInput.data(), pwdInput.capacity() - 1, flags);
		bool submit2 = ImGui::Button("Change");

		if (submit || submit2)
		{
			auto pwd = std::string_view(pwdInput.data());
			store.SetText(modalIdx, pwd);
			memset(pwdInput.data(), 0, pwdInput.capacity());
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Change Name"))
		{
			ImGui::CloseCurrentPopup();
			OpenChangeNameModal(modalIdx);
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			memset(pwdInput.data(), 0, pwdInput.capacity());
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void MainApplet::RenderChangeFileModal()
{
	if (openChangeFileModal)
	{
		openChangeFileModal = false;
		ImGui::OpenPopup("Change File");
	}

	if (ImGui::BeginPopupModal("Change File", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static std::wstring file;

		auto& store = game.GetContentStore();
		Text("Name:");
		ImGui::SameLine();
		Text(store.GetName(modalIdx));

		if (ImGui::Button("Select File"))
		{
			wBuffer = StringUtils::Utf8ToWideString(store.GetName(modalIdx));
			WinApi::OpenFileDialog(L"Replace file in vault", wBuffer, file);
			wBuffer.clear();
		}
		ImGui::SameLine();
		if (file.empty())
			Text("(not selected)");
		else
			Text(StringUtils::WideStringToUtf8(std::filesystem::path(file).filename().native()));

		if (ImGui::Button("Change"))
		{
			if (!file.empty())
			{
				if (!store.SetFile(modalIdx, file))
				{
					OpenErrorModal("Failed to read file - check permissions or logs for more details");
				}
				file.clear();
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Change Name"))
		{
			ImGui::CloseCurrentPopup();
			OpenChangeNameModal(modalIdx);
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			file.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void MainApplet::RenderChangeNameModal()
{
	if (openChangeNameModal)
	{
		openChangeNameModal = false;
		ImGui::OpenPopup("Change Name");

		nameInput = game.GetContentStore().GetName(modalIdx);
	}

	if (ImGui::BeginPopupModal("Change Name", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("##Name", nameInput.data(), nameInput.capacity() - 1, ImGuiInputTextFlags_NoUndoRedo);

		if (ImGui::Button("Change"))
		{
			auto name = std::string_view(nameInput.data());
			game.GetContentStore().SetName(modalIdx, name);
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
		ImGui::OpenPopup("Delete Password");
	}

	if (ImGui::BeginPopupModal("Delete Password", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		Text("Are you sure you want to delete the password?");
		Text("Name:");
		ImGui::SameLine();

		auto& store = game.GetContentStore();
		Text(store.GetName(modalIdx));
		if (ImGui::Button("Yes"))
		{
			store.Remove(modalIdx);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("No"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}

void MainApplet::RenderErrorModal()
{
	if (openErrorModal)
	{
		openErrorModal = false;
		ImGui::OpenPopup("Error");
	}

	if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		Text(message);
		if (ImGui::Button("Cancel"))
		{
			message.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}
