cmake_minimum_required(VERSION 3.13.0)
project(tg-poker VERSION 0.0.3 LANGUAGES CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

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
target_compile_options(tg-poker PRIVATE -Wall -Wextra)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
