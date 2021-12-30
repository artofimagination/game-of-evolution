cmake_minimum_required(VERSION 3.16.3)
project(GameOfEvolution)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# Find qml and qt packages. 
find_package(Qt5 COMPONENTS Charts Qml Quick 3DQuick Widgets 3DQuickExtras  REQUIRED)
find_package(OpenMP)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Main source for the library 
set(MAIN_SOURCES
    ${PROJECT_SOURCE_DIR}/AlgorithmHelpers.cpp
    ${PROJECT_SOURCE_DIR}/AlgorithmHelpers.h
    ${PROJECT_SOURCE_DIR}/Analytics.cpp
    ${PROJECT_SOURCE_DIR}/Analytics.h
    ${PROJECT_SOURCE_DIR}/App.cpp
    ${PROJECT_SOURCE_DIR}/App.h
    ${PROJECT_SOURCE_DIR}/Backend.cpp
    ${PROJECT_SOURCE_DIR}/Backend.h
    ${PROJECT_SOURCE_DIR}/BasicTypes.cpp
    ${PROJECT_SOURCE_DIR}/BasicTypes.h
    ${PROJECT_SOURCE_DIR}/Barriers/CircleBarrier.cpp
    ${PROJECT_SOURCE_DIR}/Barriers/CircleBarrier.h
    ${PROJECT_SOURCE_DIR}/Barriers/iBarriers.cpp
    ${PROJECT_SOURCE_DIR}/Barriers/iBarriers.h
    ${PROJECT_SOURCE_DIR}/Barriers/RectangleBarrier.cpp
    ${PROJECT_SOURCE_DIR}/Barriers/RectangleBarrier.h
    ${PROJECT_SOURCE_DIR}/Challenges/AgainstAnyWall.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/AgainstAnyWall.h
    ${PROJECT_SOURCE_DIR}/Challenges/Altruism.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/Altruism.h
    ${PROJECT_SOURCE_DIR}/Challenges/AltruismSacrifice.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/AltruismSacrifice.h
    ${PROJECT_SOURCE_DIR}/Challenges/CenterSparsed.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/CenterSparsed.h
    ${PROJECT_SOURCE_DIR}/Challenges/CenterUnweighted.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/CenterUnweighted.h
    ${PROJECT_SOURCE_DIR}/Challenges/CenterWeighted.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/CenterWeighted.h
    ${PROJECT_SOURCE_DIR}/Challenges/Circle.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/Circle.h
    ${PROJECT_SOURCE_DIR}/Challenges/CircularSequence.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/CircularSequence.h
    ${PROJECT_SOURCE_DIR}/Challenges/Corner.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/Corner.h
    ${PROJECT_SOURCE_DIR}/Challenges/CornerWeighted.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/CornerWeighted.h
    ${PROJECT_SOURCE_DIR}/Challenges/EastWestEighths.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/EastWestEighths.h
    ${PROJECT_SOURCE_DIR}/Challenges/iChallenges.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/iChallenges.h
    ${PROJECT_SOURCE_DIR}/Challenges/LeftEighth.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/LeftEighth.h
    ${PROJECT_SOURCE_DIR}/Challenges/LocationSequence.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/LocationSequence.h
    ${PROJECT_SOURCE_DIR}/Challenges/MigrateDistance.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/MigrateDistance.h
    ${PROJECT_SOURCE_DIR}/Challenges/NearBarrier.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/NearBarrier.h
    ${PROJECT_SOURCE_DIR}/Challenges/NeighborCount.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/NeighborCount.h
    ${PROJECT_SOURCE_DIR}/Challenges/Pairs.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/Pairs.h
    ${PROJECT_SOURCE_DIR}/Challenges/RadioactiveWalls.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/RadioactiveWalls.h
    ${PROJECT_SOURCE_DIR}/Challenges/RightHalf.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/RightHalf.h
    ${PROJECT_SOURCE_DIR}/Challenges/RightQuarter.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/RightQuarter.h
    ${PROJECT_SOURCE_DIR}/Challenges/TouchAnyWall.cpp
    ${PROJECT_SOURCE_DIR}/Challenges/TouchAnyWall.h
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
    ${PROJECT_SOURCE_DIR}/SysStateMachine.cpp
    ${PROJECT_SOURCE_DIR}/SysStateMachine.h
    ${PROJECT_SOURCE_DIR}/qml/ChartsConnector.cpp
    ${PROJECT_SOURCE_DIR}/qml/ChartsConnector.h
    ${PROJECT_SOURCE_DIR}/QMLChallengeItems.h
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
        Qt5::Quick
        Qt5::Charts
        OpenMP::OpenMP_CXX) # for critical section and parallel execution

set(LIBRARIES ${LIBRARIES} ${QT_LIBRARIES})

add_executable(GameOfEvolution ${MAIN_SOURCES})

install(TARGETS GameOfEvolution DESTINATION .)
target_compile_options(GameOfEvolution PRIVATE -Werror -Wall -Wextra -fopenmp -ftree-parallelize-loops=10)

# Linking libraries.
target_link_libraries(GameOfEvolution LINK_PUBLIC ${LIBRARIES})

