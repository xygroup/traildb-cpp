project (traildb-cpp)

cmake_minimum_required (VERSION 2.8)

set(CMAKE_MACOSX_RPATH 1)

include(${WORKSPACE}/WORKSPACE.cmake)

include_directories("." "./src")

add_compile_options(-std=c++14)
add_compile_options(-DBAZEL=1)

SET( CPP_FILES
)
SET( H_FILES
${PROJECT_SOURCE_DIR}/include/ByteManipulations.h
${PROJECT_SOURCE_DIR}/include/TrailDBConstructor.h
${PROJECT_SOURCE_DIR}/include/TrailDBcpp.h
${PROJECT_SOURCE_DIR}/include/TrailDBException.h
)

SET( SOURCE_FILES ${CPP_FILES} ${H_FILES})

SET(BUILD_FILES
${PROJECT_SOURCE_DIR}/BUILD
${PROJECT_SOURCE_DIR}/PACKAGE.cmake
)

include (${WORKSPACE}/traildb-cpp/PACKAGE.cmake)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system)

add_executable(test ${PROJECT_SOURCE_DIR}/tests/test.cpp ${SOURCE_FILES} ${BUILD_FILES})
target_link_libraries(test ${TRAILDB_LIB})
target_link_libraries(test ${GTEST_LIB})

add_custom_target (FORMAT ALL
/usr/local/bin/clang-format -style=Google -i ${SOURCE_FILES} ${PROJECT_SOURCE_DIR}/tests/test.cpp
)
