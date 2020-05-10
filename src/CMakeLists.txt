cmake_minimum_required(VERSION 3.15)
project(ece141_sp20_assignment4_zhiqiang_sun_hongxiang_jiang)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(ece141_sp20_assignment4_zhiqiang_sun_hongxiang_jiang
        AppProcessor.cpp
        AppProcessor.hpp
        Attribute.cpp
        Attribute.hpp
        CommandProcessor.cpp
        CommandProcessor.hpp
        Database.cpp
        Database.hpp
        DBProcessor.cpp
        DBProcessor.hpp
        Errors.hpp
        FolderReader.cpp
        FolderReader.hpp
        Helpers.hpp
        keywords.hpp
        main.cpp
        Schema.cpp
        Schema.hpp
        SQLProcessor.cpp
        SQLProcessor.hpp
        Statement.cpp
        Statement.hpp
        Storage.cpp
        Storage.hpp
        StorageBlock.cpp
        StorageBlock.hpp
        Tokenizer.cpp
        Tokenizer.hpp
        View.cpp
        View.hpp)
