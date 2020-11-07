# m68k Module
add_library(MuCoreM68k STATIC
	m68kcpu.c
	m68kdasm.c
	m68kopac.c
	m68kopdm.c
	m68kopnz.c
	m68kops.c)

# Includes for the project
target_include_directories(MuCoreM68k PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/include")
