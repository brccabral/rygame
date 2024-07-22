cmake_minimum_required(VERSION 3.20)
project(raylib_utils VERSION 0.0.1)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)

find_package(raylib REQUIRED)
find_package(raylib-tmx REQUIRED)

set(PROJECT_CONFIG "${PROJECT_NAME}Config")

add_library(${PROJECT_NAME} STATIC raylib_utils.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Install rules
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_CONFIG}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)

# Export the library for use with find_package
export(EXPORT ${PROJECT_CONFIG}
        FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CONFIG}.cmake"
)
install(EXPORT ${PROJECT_CONFIG}
        FILE ${PROJECT_CONFIG}.cmake
        DESTINATION lib/cmake/${PROJECT_NAME}
)