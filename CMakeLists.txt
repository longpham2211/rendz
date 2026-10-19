
# SMALLLAB28 SOFTWARE

cmake_minimum_required(VERSION 3.10)
project(rendz)

SET(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(app 
    src/main-rendz/rendz-main.cpp
    src/pdf/pdf-document.cpp
    src/pdf/pdf-xref.cpp
    src/pdf/pdf-page.cpp
    src/pdf/pdf-stream.cpp
    src/rendz/context.cpp
    third-party/miniz/miniz.c
)

target_include_directories(
    app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(
    app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:app>/data
)
