#sources and main.cpp 
custom_add_headers_recursive(Nightfear)
custom_enable_cxx17(Nightfear)
target_include_directories(Nightfear INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

