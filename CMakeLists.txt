cmake_minimum_required(VERSION 3.10)
project(XenoraOS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Core Gui Network X11Extras REQUIRED)
find_package(X11 REQUIRED)

include_directories(${X11_INCLUDE_DIR})

# QRC dosyasını devre dışı bırakıyoruz
# set(RESOURCE_FILES
#     resources/resources.qrc
# )

set(SOURCE_FILES
    src/main.cpp
    src/xenorasystem.cpp
    src/xenorawindow.cpp
    src/panel/panel.cpp
    src/panel/startmenu.cpp
    src/panel/systray.cpp
    src/desktop/desktop.cpp
    src/desktop/desktopiconview.cpp
    src/windowmanager/windowmanager.cpp
    src/settings/settingsmanager.cpp
)

set(HEADER_FILES
    src/xenorasystem.h
    src/xenorawindow.h
    src/panel/panel.h
    src/panel/startmenu.h
    src/panel/systray.h
    src/desktop/desktop.h
    src/desktop/desktopiconview.h
    src/windowmanager/windowmanager.h
    src/settings/settingsmanager.h
)

# QRC kaldırıldı
add_executable(xenora-ui ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(xenora-ui
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Network
    Qt5::X11Extras
    ${X11_LIBRARIES}
    Xcomposite
    Xdamage
    Xrender
)

# Tüm kaynak dosyalarını kopyala
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
