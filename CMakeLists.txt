#get max version CMAKE
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project("NightFear" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

if(UNIX)
#set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} / build / linux)

#custom_log("<This is Linux Runtime>")
    message("<This is Linux Runtime>")
elseif(WIN32)    #set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/windows)

#custom_log("<This is Windows Runtime>")
    message("<This is Windows Runtime>")
endif()

    add_subdirectory(ronin-engine)

file(GLOB_RECURSE HCPPFILES src/*.cpp src/*.hpp src/*.h)

set(RUNTIME_OUTPUT_NAME "NightFear")

add_executable(${PROJECT_NAME} ${HCPPFILES})

target_link_libraries(${PROJECT_NAME} RoninEngineLibrary)
