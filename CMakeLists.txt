cmake_minimum_required (VERSION 3.13)
project(Mu
	VERSION 1.2.0
	DESCRIPTION "Classic Palm OS Emulator."
	HOMEPAGE_URL https://github.com/libretro/Mu
	LANGUAGES C CXX)

# Build properties for Mu
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Main project sources
add_subdirectory(src)

# LibRetro Build
add_subdirectory(libretroBuildSystem)
