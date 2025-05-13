cmake_minimum_required(VERSION 3.10)
project(XenoraOS VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt5 bileşenlerini bul
find_package(Qt5 COMPONENTS Widgets X11Extras REQUIRED)
find_package(X11 REQUIRED)

# Derleme seçenekleri
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Kaynak dosyaları
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/desktopmanager.cpp
    src/desktopicon.cpp
    src/panel.cpp
    src/taskbar.cpp
    src/systemtray.cpp
    src/launcher.cpp
    src/filemanager.cpp
    src/systemsettings.cpp
    resources/resources.qrc
)

# Header dosyaları
set(HEADERS
    src/mainwindow.h
    src/desktopmanager.h
    src/desktopicon.h
    src/panel.h
    src/taskbar.h
    src/systemtray.h
    src/launcher.h
    src/filemanager.h
    src/systemsettings.h
)

# Çalıştırılabilir dosya
add_executable(xenora-ui ${SOURCES} ${HEADERS})

# Bağımlılıklar
target_link_libraries(xenora-ui PRIVATE
    Qt5::Widgets
    Qt5::X11Extras
    ${X11_LIBRARIES}
)

# Include klasörü
include_directories(include)

# Hedef dizinler
install(TARGETS xenora-ui DESTINATION bin)
install(FILES resources/xeonaos.desktop DESTINATION /usr/share/xsessions)
install(FILES resources/Xenoraos-logo.png DESTINATION /usr/share/pixmaps)
