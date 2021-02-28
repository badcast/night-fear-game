#sources and main.cpp 
custom_add_headers_recursive(${PROJECT_NAME})
custom_enable_cxx17(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ../main.cpp)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

