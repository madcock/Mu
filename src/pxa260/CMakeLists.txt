# PXA260 Module
add_library(MuCorePxa260Experimental STATIC
	pxa260.c
	pxa260_DMA.c
	pxa260_DSP.c
	pxa260_GPIO.c
	pxa260_IC.c
	pxa260_LCD.c
	pxa260_PwrClk.c
	pxa260_RTC.c
	pxa260_TIMR.c
	pxa260_UART.c
	pxa260I2c.c
	pxa260Memctrl.c
	pxa260Ssp.c
	pxa260Timing.c
	pxa260Udc.c)

# Bring all the sub-modules as needed
target_link_libraries(MuCorePxa260Experimental
	MuCoreArmV5TEExperimental)

# Some defines are required for this to function properly
target_compile_definitions(MuCorePxa260Experimental
	PUBLIC EMU_SUPPORT_PALM_OS5=1

	# forces the dynarec to use accurate mode and disable Nspire OS hacks
	PUBLIC SUPPORT_LINUX=1)

# Includes for the project
target_include_directories(MuCorePxa260Experimental PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/include/pxa260")
