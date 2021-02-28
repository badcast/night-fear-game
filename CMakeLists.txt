#get max version CMAKE
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(Nightfear LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BINARY_DIR build)

find_package(SDL2 REQUIRED)

include(cmake/functions.cmake)

add_subdirectory(source) #get sources and linking from the "source" directory

add_executable(Nightfear main.cpp)

#target_include_directories(${ProjectName} PRIVATE ${SDL2_INCLUDE_DIR}
#                                     ${SDL2_IMAGE_INCLUDE_DIR}
#                                     ${SDL2_TTF_INCLUDE_DIR})

#target_link_libraries(${ProjectName} ${SDL2_LIBRARIES})

#if(UNIX)
# set(TARGET_DESTINATION_DIR "build/linux/")
#elseif(WIN32)
# set(TARGET_DESTINATION_DIR "build/windows/")

#install(TARGETS ${PROJECT_NAME} DESTINATION build)
