# We use these to automatically generate UI headers and otherwise in Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

# We need all the Qt5 libraries
find_package(Qt5 OPTIONAL_COMPONENTS Core Gui Widgets Multimedia Svg)

# Setup executable for running
add_executable(QtMu
	debugviewer.cpp
	debugviewer.h
	debugviewer.ui
	emuwrapper.cpp
	emuwrapper.h
	main.cpp
	mainwindow.cpp
	mainwindow.h
	mainwindow.qrc
	mainwindow.ui
	settingsmanager.cpp
	settingsmanager.h
	settingsmanager.ui
	statemanager.cpp
	statemanager.ui
	touchscreen.h
	touchscreen.cpp)

# The Qt build requires modern-ish C++11
set_target_properties(QtMu PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS YES)

# Include the required includes
target_include_directories(QtMu PUBLIC
	"${PROJECT_SOURCE_DIR}/include"

	# When we auto-generate the UI code, we refer to includes that are in
	# the Qt source directory
	"${PROJECT_SOURCE_DIR}/qtBuildSystem/Mu")

# Used to indicate some things for compatibility
add_definitions(-DBUILT_FROM_CMAKE)

# Annoying Visual Studio Warnings
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# Need to link into all these Qt components
target_link_libraries(QtMu
	MuCore
	Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Multimedia Qt5::Svg)

# Deployment for Windows
if(WIN32)
	# Retrieve the absolute path to qmake and then use that path to find
	# the windeployqt executable
	get_target_property(QMAKE_EXE Qt5::qmake IMPORTED_LOCATION)
	get_filename_component(QT_BIN_DIR "${QMAKE_EXE}" DIRECTORY)

	find_program(WINDEPLOYQT_ENV_SETUP qtenv2.bat HINTS "${QT_BIN_DIR}")
	find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")

	# Run windeployqt immediately after build
	add_custom_command(TARGET QtMu
		POST_BUILD
		COMMAND "${WINDEPLOYQT_ENV_SETUP}" && "${WINDEPLOYQT_EXECUTABLE}" \"$<TARGET_FILE:QtMu>\"
	)
endif()
