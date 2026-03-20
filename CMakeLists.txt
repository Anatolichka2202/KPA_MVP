# Главный CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(YAV_GUI VERSION 1.0.0)

# Подпроект core
add_subdirectory(core)

# Подпроект gui
add_subdirectory(gui)

set(SOURCES
    gui/main.cpp
    gui/mainwindow.cpp
    gui/blockwidget.cpp
    gui/bkdclient.cpp
    core/ylsemulator.cpp
    core/network.cpp
    core/logic.cpp
)

set(HEADERS
    gui/mainwindow.h
    gui/blockwidget.h
    gui/bkdclient.h
    core/ylsemulator.h
    core/network.h
    core/logic.h
    core/Protocol.h

)
