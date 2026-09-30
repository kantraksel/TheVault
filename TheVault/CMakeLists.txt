add_definitions(-DUNICODE -D_UNICODE)
add_executable(TheVault WIN32
			App/main.cpp
			App/PassManager.cpp
			Utils/StringUtils.cpp
			Utils/WinApi.cpp
			Vault/Crypto.cpp
			Vault/Vault.cpp
			Vault/VaultKeeper.cpp
			Vault/WorkerThread.cpp
			GUI/GUIManager.cpp
			GUI/MainWindow.cpp
			GUI/Applets/ErrorApplet.cpp
			GUI/Applets/LockApplet.cpp
			GUI/Applets/LoginApplet.cpp
			GUI/Applets/MainApplet.cpp
			GUI/Applets/ProcessApplet.cpp
			GUI/Applets/WelcomeApplet.cpp
)
target_include_directories(TheVault PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TheVault PRIVATE
			GhostFries::Core
			imgui::imgui
			unofficial-sodium::sodium
)
