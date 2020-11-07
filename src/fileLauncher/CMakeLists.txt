# m68k Module
add_library(MuCoreFileLauncher STATIC
	launcher.c)

# Includes for the project
target_include_directories(MuCoreFileLauncher PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/include/fileLauncher")