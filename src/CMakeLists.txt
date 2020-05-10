cmake_minimum_required(VERSION 3.15)
project(ece141_sp20_assignment2_zhiqiang_sun_hongxiang_jiang)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(ece141_sp20_assignment2_zhiqiang_sun_hongxiang_jiang
        AppProcessor.cpp
        AppProcessor.hpp
        CommandProcessor.cpp
        CommandProcessor.hpp
        DBProcessor.cpp
        DBProcessor.hpp
        Errors.hpp
        FolderReader.hpp
        FolderReader.cpp
        Helpers.hpp
        keywords.hpp
        main.cpp
        Makefile
        README.md
        Statement.cpp
        Statement.hpp
        Storage.cpp
        Storage.hpp
        students.json
        Tokenizer.cpp
        Tokenizer.hpp
        View.hpp FolderReader.cpp)
