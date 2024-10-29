cmake_minimum_required(VERSION 3.16.0)
list(APPEND EXTRA_COMPONENT_DIRS ./include/)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(new_pio_cam)