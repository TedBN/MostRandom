cmake_minimum_required(VERSION 3.24)
project(MostRandom)

set(CMAKE_CXX_STANDARD 20)

add_executable(MostRandom main.cpp
        ListFunctions.cpp
        ListFunctions.h
        EventTriggerFunctions.cpp
        EventTriggerFunctions.h
        GeneralFunctions.cpp
        GeneralFunctions.h
        RaceFunctions.cpp
        RaceFunctions.h
        PursuitFunctions.cpp
        PursuitFunctions.h
        UnusedFunctions.cpp
        FreeRoamFunctions.cpp
        FreeRoamFunctions.h
        BossRaceMarkerFunctions.cpp
        BossRaceMarkerFunctions.h
        NodeConversionFunctions.cpp
        NodeConversionFunctions.h
        XoshiroCpp.hpp)
