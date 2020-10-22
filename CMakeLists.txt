cmake_minimum_required(VERSION 3.13.0)
project(tg-poker VERSION 0.0.1 LANGUAGES CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
include(CheckCXXCompilerFlag)

macro(find_fetch package repo)
    find_package(${package} QUIET)
    if(NOT ${${package}_FOUND})
        message(STATUS "fetching ${package} from ${repo}")
        FetchContent_Declare(${package}
            GIT_REPOSITORY ${repo}
            GIT_SHALLOW true
        )
        FetchContent_MakeAvailable(${package})
        FetchContent_GetProperties(${package})
        if(NOT ${${package}_POPULATED})
          FetchContent_Populate(${package})
          add_subdirectory(${${package}_SOURCE_DIR} ${${package}_BINARY_DIR})
        endif(NOT ${${package}_POPULATED})
        list(APPEND CMAKE_PREFIX_PATH ${${package}_BINARY_DIR})
    endif(NOT ${${package}_FOUND})
endmacro()

find_fetch(TgBot https://github.com/reo7sp/tgbot-cpp)

add_executable(tg-poker main.cpp)
target_include_directories(tg-poker PRIVATE include)
target_link_libraries(tg-poker TgBot)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
