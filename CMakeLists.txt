cmake_minimum_required(VERSION 3.10)
project(XenoraOS VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Core Gui Network X11Extras REQUIRED)
find_package(X11 REQUIRED)

set(SOURCES
    src/main.cpp
    src/desktopenv/DesktopEnvironment.cpp
    src/desktopenv/WindowManager.cpp
    src/desktopenv/Panel.cpp
    src/desktopenv/AppLauncher.cpp
    src/desktopenv/SystemTray.cpp
    src/desktopenv/Desktop.cpp
    src/desktopenv/SettingsWindow.cpp
    src/desktopenv/StyleManager.cpp
    src/widgets/XenoraButton.cpp
    src/widgets/XenoraPanel.cpp
    src/widgets/XenoraWindow.cpp
    src/widgets/XenoraPopup.cpp
    resources/XenoraOS.qrc
)

set(HEADERS
    src/desktopenv/DesktopEnvironment.h
    src/desktopenv/WindowManager.h
    src/desktopenv/Panel.h
    src/desktopenv/AppLauncher.h
    src/desktopenv/SystemTray.h
    src/desktopenv/Desktop.h
    src/desktopenv/SettingsWindow.h
    src/desktopenv/StyleManager.h
    src/widgets/XenoraButton.h
    src/widgets/XenoraPanel.h
    src/widgets/XenoraWindow.h
    src/widgets/XenoraPopup.h
)

add_executable(xenora-ui ${SOURCES} ${HEADERS})

target_link_libraries(xenora-ui PRIVATE
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Network
    Qt5::X11Extras
    ${X11_LIBRARIES}
)

install(TARGETS xenora-ui DESTINATION bin)
