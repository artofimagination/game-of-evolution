cmake_minimum_required(VERSION 3.16.3)
project(GameOfEvolution)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# Find qml and qt packages. 
find_package(Qt5 COMPONENTS Qml Quick 3DQuick Widgets 3DQuickExtras REQUIRED)

# Main source for the library 
set(MAIN_SOURCES 
    ${PROJECT_SOURCE_DIR}/AlgorithmHelpers.cpp
    ${PROJECT_SOURCE_DIR}/AlgorithmHelpers.h
    ${PROJECT_SOURCE_DIR}/App.cpp
    ${PROJECT_SOURCE_DIR}/App.h
    ${PROJECT_SOURCE_DIR}/Backend.cpp
    ${PROJECT_SOURCE_DIR}/Backend.h
    ${PROJECT_SOURCE_DIR}/BasicTypes.cpp
    ${PROJECT_SOURCE_DIR}/BasicTypes.h
    ${PROJECT_SOURCE_DIR}/GenerationGenerator.cpp
    ${PROJECT_SOURCE_DIR}/GenerationGenerator.h
    ${PROJECT_SOURCE_DIR}/Genome.cpp
    ${PROJECT_SOURCE_DIR}/Genome.h
    ${PROJECT_SOURCE_DIR}/Grid.cpp
    ${PROJECT_SOURCE_DIR}/Grid.h
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/main.qrc
    ${PROJECT_SOURCE_DIR}/Parameters.cpp
    ${PROJECT_SOURCE_DIR}/Parameters.h
    ${PROJECT_SOURCE_DIR}/Peep.cpp
    ${PROJECT_SOURCE_DIR}/Peep.h
    ${PROJECT_SOURCE_DIR}/PeepsPool.cpp
    ${PROJECT_SOURCE_DIR}/PeepsPool.h
    ${PROJECT_SOURCE_DIR}/PheromoneSignals.cpp
    ${PROJECT_SOURCE_DIR}/PheromoneSignals.h
    ${PROJECT_SOURCE_DIR}/QMLInterface.cpp
    ${PROJECT_SOURCE_DIR}/QMLInterface.h
    ${PROJECT_SOURCE_DIR}/Random.cpp
    ${PROJECT_SOURCE_DIR}/Random.h
    ${PROJECT_SOURCE_DIR}/SensorsActions.cpp
    ${PROJECT_SOURCE_DIR}/SensorsActions.h
)

# Set QT libraries
set(QT_LIBRARIES
        Qt5::Core
        Qt5::Widgets
        Qt5::Qml
        Qt5::Network # Need to include QtQuick depends on it, causing missing shared library issue during deployment
        Qt5::Quick)

set(LIBRARIES ${LIBRARIES} ${QT_LIBRARIES})

add_executable(GameOfEvolution ${MAIN_SOURCES})

install(TARGETS GameOfEvolution DESTINATION .)

# Linking libraries.
target_link_libraries(GameOfEvolution LINK_PUBLIC ${LIBRARIES})

