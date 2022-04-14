cmake_minimum_required(VERSION 3.14)

project(DataSender LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network Concurrent REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network Concurrent REQUIRED)

add_executable(DataSender
  main.cpp
        DataSender.cpp DataSender.h DataGenerator.cpp DataGenerator.h ConnectionHandler.cpp ConnectionHandler.h)
target_link_libraries(DataSender Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Concurrent)
