cmake_minimum_required(VERSION 3.28)

set(USING_PYTHON ON)

project(Sinfourmis)

set(sources
	src/map/map.cpp
	src/map/node.cpp
	src/map/edge.cpp
	src/game/game.cpp
	src/game/ant.cpp
	src/game/queen.cpp
	src/utils/groups.cpp
	src/animation/animation.cpp
	src/interfaces/dummy.cpp
	src/interfaces/shared.cpp
	src/debugger/debugger.cpp
	src/main.cpp
)

if (USING_PYTHON)
	list(APPEND sources src/interfaces/python.cpp)
endif()

add_executable(Sinfourmis ${sources})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

string(APPEND CMAKE_CXX_FLAGS "-std=c++20 -Wall -Wextra -Werror -pedantic -Wpointer-arith -Wchar-subscripts -Wcomment -Wformat -Wwrite-strings -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function")

include_directories(include)

if (USING_PYTHON)
	add_compile_definitions(USING_PYTHON)
	find_package(Python COMPONENTS Interpreter Development)
	include_directories(${Python_INCLUDE_DIRS})

	execute_process(
	COMMAND sh -c "python3-config --cflags | tr -d '\n'"
	OUTPUT_VARIABLE PY_CFLAGS
	)
	execute_process(
	COMMAND sh -c "python3-config --ldflags --embed | tr -d '\n'"
	OUTPUT_VARIABLE PYTHON_LDFLAGS
	)
	execute_process(
	COMMAND sh -c "python3 --version | cut -d' ' -f2 | cut -d'.' -f1,2"
	OUTPUT_VARIABLE PYTHON_VERSION
	)

	string(APPEND CMAKE_CXX_FLAGS ${PY_CFLAGS})
	string(APPEND CMAKE_EXE_LINKER_FLAGS ${PYTHON_LDFLAGS})
endif()

# json library
include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(Sinfourmis PRIVATE nlohmann_json::nlohmann_json)

# argparser library
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

target_link_libraries(Sinfourmis PRIVATE argparse)

# clang-format
find_program(CLANG_FORMAT_EXE NAMES "clang-format" DOC "Path to clang-format executable")
if(CLANG_FORMAT_EXE)
	add_custom_target(
		format
		COMMAND ${CLANG_FORMAT_EXE} -i -style=file 
			${PROJECT_SOURCE_DIR}/src/*.cpp 
			${PROJECT_SOURCE_DIR}/src/*/*.cpp 
			${PROJECT_SOURCE_DIR}/include/*.h
			${PROJECT_SOURCE_DIR}/include/*/*.hpp
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	)
else()
	message(STATUS "clang-format not found.")
endif()
