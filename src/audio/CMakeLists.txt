# m68k Module
add_library(MuCoreAudio STATIC
	blip_buf.c)

# Make this position independent so it can be linked into shared libraries
set_property(TARGET MuCoreAudio
	PROPERTY POSITION_INDEPENDENT_CODE ON)

# Includes for the project
target_include_directories(MuCoreAudio PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/include")