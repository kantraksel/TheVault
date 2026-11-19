execute_process(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE COMMIT_HASH_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
configure_file(App/version.cpp.in App/version.cpp)

add_definitions(-DUNICODE -D_UNICODE)
add_executable(TheVault WIN32
			App/main.cpp
			App/ContentStore.cpp
			App/version.cpp
			Utils/StringUtils.cpp
			Utils/WinApi.cpp
			Vault/Crypto.cpp
			Vault/Vault.cpp
			Vault/VaultKeeper.cpp
			Vault/WorkerThread.cpp
			GUI/GUIManager.cpp
			GUI/MainWindow.cpp
			GUI/Applets/AboutApplet.cpp
			GUI/Applets/BlockingApplet.cpp
			GUI/Applets/LockApplet.cpp
			GUI/Applets/LoginApplet.cpp
			GUI/Applets/ProcessApplet.cpp
			GUI/Applets/WelcomeApplet.cpp
			GUI/Modals/BaseModal.cpp
			GUI/Modals/AddPasswordModal.cpp
			GUI/Modals/AddFileModal.cpp
			GUI/Modals/ShowPasswordModal.cpp
			GUI/Modals/BaseIndexModal.cpp
			GUI/Modals/ChangePasswordModal.cpp
			GUI/Modals/ChangeFileModal.cpp
			GUI/Modals/ChangeNameModal.cpp
			GUI/Modals/DeleteEntryModal.cpp
			GUI/Modals/ShowPasswordWarningModal.cpp
			GUI/Modals/ConfirmExitModal.cpp
			GUI/Modals/ErrorModal.cpp
			GUI/Modals/BaseDualIndexModal.cpp
			GUI/Modals/AddLockModal.cpp
			GUI/Modals/ChangeLockPasswordModal.cpp
			GUI/ReactiveScheduler.cpp
			GUI/VaultCommander.cpp
)
target_include_directories(TheVault PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TheVault PRIVATE
			GhostFries::Core
			imgui::imgui
			unofficial-sodium::sodium
)
