# m68k Module
add_library(MuCoreAudio STATIC
	blip_buf.c)

# Includes for the project
target_include_directories(MuCoreAudio PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/include")