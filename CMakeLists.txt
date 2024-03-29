cmake_minimum_required(VERSION 3.5)

project(xinlink_ui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(./)
include_directories(./core/include)
include_directories(./dsp)
include_directories(./dsp/moonlight)
include_directories(./dsp/usbip)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ./core/src/core.cpp
        ./core/src/event.cpp
        ./core/src/instruction.cpp
        ./core/src/network.cpp
        ./core/src/ll.cpp
        ./dsp/dsp.cpp
        ./dsp/moonlight/moonlight.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(xinlink_ui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET xinlink_ui APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(xinlink_ui SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(xinlink_ui
            ${PROJECT_SOURCES}

            xuisetting.h xuisetting.cpp xuisetting.ui
            xuidsp.h xuidsp.cpp xuidsp.ui
            xuiconsole.h xuiconsole.cpp xuiconsole.ui
            xuiabout.h xuiabout.cpp xuiabout.ui
            xuiconsoleball.h xuiconsoleball.cpp xuiconsoleball.ui

            xuipixmapitem.h xuipixmapitem.cpp
            dsp/monitor.h
            dsp/monitor.cpp
            dsp/moonlight/moonlight.cpp dsp/moonlight/moonlight.h
            core/src/device.cpp
            core/include/device.h
            dsp/usbip/usbip.h dsp/usbip/usbip.cpp


        )
    endif()
endif()

target_link_libraries(xinlink_ui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

find_package(Threads REQUIRED)
target_link_libraries(xinlink_ui PRIVATE Threads::Threads uv)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.xinlink_ui)
endif()
set_target_properties(xinlink_ui PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS xinlink_ui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(xinlink_ui)
endif()

