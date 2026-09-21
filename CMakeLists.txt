cmake_minimum_required(VERSION 3.21)

project(TheVault)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(ryml CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)

add_subdirectory(GhostFries)
add_subdirectory(TheVault)

install(TARGETS TheVault DESTINATION bin)
install(FILES $<TARGET_PDB_FILE:TheVault> DESTINATION bin OPTIONAL)
