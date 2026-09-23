![GitHub License](https://img.shields.io/github/license/kantraksel/TheVault?style=for-the-badge)
![GitHub Release](https://img.shields.io/github/v/release/kantraksel/TheVault?style=for-the-badge)
![GitHub branch status](https://img.shields.io/github/checks-status/kantraksel/TheVault/master?style=for-the-badge&label=Build)

# Overview
TheVault is a secure data container, primarily designed for passwords. Think about this like a container for root passwords and keys - intended for long-term storage with infrequent access.

## The Vault Structure, Security and Passwords
The topic is described in separate document [Security and Whatnot](SECURITY_AND_WHATNOT.md)

## How To Build
### Prerequisities
- vcpkg (newer than 2026-06-24 - see baseline in vcpkg.json)
- CMake 3.21 or later
- MSVC with Toolchain v145

### Build Steps
Instructions are assumed to be executed in Command Prompt (cmd.exe)
1. Set VCPKG_ROOT to vcpkg directory: `set VCPKG_ROOT=D:/path/to/vcpkg`
2. Run CMake configure for windows-vcpkg preset: `cmake --preset windows-vcpkg .`
3. Run build command: `cmake --build ./Binary/cmake-windows --config RelWithDebInfo`
4. Run CMake install: `cmake --install ./Binary/cmake-windows`
5. The app can be found at `TheVault/Binary/cmake-install` as standalone executable file
