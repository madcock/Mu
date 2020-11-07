add_library(mu-libretro SHARED
	libretro.c
	cursors.c
	libretro-common/compat/compat_strl.c
	libretro-common/compat/compat_posix_string.c
	libretro-common/compat/fopen_utf8.c
	libretro-common/encodings/encoding_utf.c
	libretro-common/memmap/memmap.c
	libretro-common/streams/file_stream.c
	libretro-common/string/stdstring.c
	libretro-common/vfs/vfs_implementation.c)

# Remove the "lib" prefix on Windows, it is incorrect
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
	set_target_properties(mu-libretro PROPERTIES PREFIX "")
endif()

# Bring all the sub-modules as needed
target_link_libraries(mu-libretro
	MuCore)

# Include the required includes
target_include_directories(mu-libretro PUBLIC
	"${PROJECT_SOURCE_DIR}/include"
	"libretro-common/include")

# Custom launching the core, tries to find RetroArch on the system
## Determine RetroArch directory
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
	set(MU_LIBRETRO_EXTENSION ".exe")

	if (EXISTS "$ENV{APPDATA}/RetroArch")
		set(MU_LIBRETRO_DIR "$ENV{APPDATA}/RetroArch")
	endif()
else()
	set(MU_LIBRETRO_EXTENSION "")
endif()

## Target to run RetroArch with the SquirrelJME Core
if(DEFINED MU_LIBRETRO_DIR)
	add_custom_target(RetroArch
		DEPENDS mu-libretro
		COMMAND "${MU_LIBRETRO_DIR}/retroarch${MU_LIBRETRO_EXTENSION}" "-v" "-L" "$<TARGET_FILE:mu-libretro>"
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		COMMENT "Starting RetroArch with Mu")
endif()
