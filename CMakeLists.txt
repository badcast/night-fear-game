#get max version CMAKE
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(nightfear LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

find_library(TARGET_LIBS NAMES libSDL2 REQUIRED)

message("${TARGET_LIBS}")
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

#FUNCTIONS
include(cmake/functions.cmake)

add_subdirectory(source) #get sources and linking from the "source" directory

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})















if(UNIX)
    set(TARGET_DESTINATION_DIR "build/linux/")

   # custom_log("<This is Linux Runtime>")

elseif(WIN32)
    set(TARGET_DESTINATION_DIR "build/windows/")

   # custom_log("<This is Windows Runtime>")

endif()
