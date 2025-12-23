cmake_minimum_required(VERSION 3.16)

project(PaintApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(PaintApp WIN32
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp

    src/canvas/Canvas.h
    src/canvas/Canvas.cpp
)



target_link_libraries(PaintApp Qt6::Widgets)
