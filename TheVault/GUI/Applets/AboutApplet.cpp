#include "AboutApplet.hpp"
#include "App/version.hpp"
#include "Utils/ImGuiUtils.h"

AboutApplet::AboutApplet(MainWindow& mainWindow) : BlockingApplet(mainWindow)
{
}

AboutApplet::~AboutApplet()
{
}

void AboutApplet::OnRender()
{
	bool isWindowOpen = isOpen;
	ImGui::SetNextWindowSize(ImVec2(350, 0));
	CenterNextWindow();
	ImGui::Begin("About TheVault", &isWindowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	ImGui::Text("TheVault version %s", Version::FullString);
	ImGui::Text("Built on %s", Version::BuildTime);
	Text("Homepage:");
	ImGui::SameLine();
	ImGui::TextLinkOpenURL("https://github.com/kantraksel/TheVault");
	ImGui::Separator();
	Text("Licenses:");
	ImGui::Bullet();
	Text("TheVault:");
	ImGui::SameLine();
	ImGui::TextLinkOpenURL("The MIT License##thevaultmit", "https://github.com/kantraksel/TheVault/blob/507dab1a2a8c85b1df22463708cda6f29fec812a/LICENSE.md");
	ImGui::Bullet();
	Text("libsodium:");
	ImGui::SameLine();
	ImGui::TextLinkOpenURL("ISC License##libsodiumisc", "https://github.com/jedisct1/libsodium/blob/master/LICENSE");
	ImGui::Bullet();
	Text("Dear ImGui:");
	ImGui::SameLine();
	ImGui::TextLinkOpenURL("The MIT License##imguimit", "https://github.com/ocornut/imgui/blob/master/LICENSE.txt");
	ImGui::Bullet();
	Text("Rapid YAML:");
	ImGui::SameLine();
	ImGui::TextLinkOpenURL("The MIT License##rymlmit", "https://github.com/biojppm/rapidyaml/blob/master/LICENSE.txt");

	if (!isWindowOpen || !ImGui::IsWindowFocused())
		Close();
	ImGui::End();
}
