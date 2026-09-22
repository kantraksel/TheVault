cmake_minimum_required(VERSION 3.21)

project(GhostFries)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(ryml CONFIG REQUIRED)

add_definitions(-DUNICODE -D_UNICODE)
add_library(GhostFriesCore STATIC
			src/Engine/GhostFries.cpp
			src/Engine/Logger.cpp
			src/Engine/version.cpp

			src/Systems/GraphicsEngine.cpp
			src/Systems/WindowManager.cpp
			src/Systems/FileSystem.cpp
			src/Systems/AppData.cpp
			src/Systems/Configurations.cpp
			src/Systems/YamlDoc.cpp
			src/Systems/YamlNode.cpp

			src/Files/FileStream.cpp
			src/Files/MemoryStream.cpp
			src/Files/DirectStream.cpp
			src/Files/FileReader.cpp
			src/Files/FileWriter.cpp
			src/Files/Container.cpp

			src/Utility/StringUtils.cpp
			)
target_include_directories(GhostFriesCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(GhostFriesCore PUBLIC
			ryml::ryml
			)
add_library(GhostFries::Core ALIAS GhostFriesCore)
