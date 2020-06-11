cmake_minimum_required(VERSION 3.15)
project(src)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(src
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
        Filters.cpp
        Filters.hpp
        FolderReader.cpp
        FolderReader.hpp
        Helpers.hpp
        Index.cpp
        Index.hpp
        IndexProcessor.cpp
        IndexProcessor.hpp
        keywords.hpp
        main.cpp
        RecordProcessor.cpp
        RecordProcessor.hpp
        Row.cpp
        Row.hpp
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
        Timer.hpp
        Tokenizer.cpp
        Tokenizer.hpp
        Value.cpp
        Value.hpp
        View.cpp
        View.hpp)
