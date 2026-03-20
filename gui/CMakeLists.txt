# Главный CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(YAV_GUI VERSION 1.0.0)

# Подпроект core
add_subdirectory(core)

# Подпроект gui
add_subdirectory(gui)