cmake_minimum_required(VERSION 3.20)
project(rygame VERSION 0.0.1)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(WITH_TMX "use TMX features" OFF)
option(SHOW_FPS "show FPS on top left of screen" OFF)

find_package(raylib REQUIRED)

set(PROJECT_CONFIG "${PROJECT_NAME}Config")
set(PROJECT_TARGETS "${PROJECT_NAME}Targets")

add_library(${PROJECT_NAME} STATIC src/rygame.cpp
        src/rygame_ns_math.cpp
        src/rygame_cl_Rect.cpp
        src/rygame_cl_Line.cpp
        src/rygame_cl_Surface.cpp
        src/rygame_ns_image.cpp
        src/rygame_cl_Frames.cpp
        src/rygame_ns_draw.cpp
        src/rygame_ns_tmx.cpp
        src/rygame_cl_Group.cpp
        src/rygame_cl_Sprite.cpp
        src/rygame_ns_sprite.cpp
        src/rygame_cl_Timer.cpp
        src/rygame_cl_Sound.cpp
        src/rygame_ns_display.cpp
        src/rygame_cl_Clock.cpp
        src/rygame_cl_Mask.cpp
        src/rygame_ns_mask.cpp
        src/rygame_cl_Font.cpp
        src/rygame_ns_transform.cpp
        src/rygame_cl_Vector2.cpp)
target_link_libraries(${PROJECT_NAME} INTERFACE raylib)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE "/WX" "/NODEFAULTLIB:libcmt")
    target_link_libraries(${PROJECT_NAME} INTERFACE gdi32 winmm)
else ()
    #    set(CMAKE_CXX_COMPILER "clang++") # Sprite::Kill() has [[nodiscard]]
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Werror" "-Wpedantic" "-Wno-missing-braces" "-Wno-gnu-anonymous-struct" "-Wno-nested-anon-types")
    target_link_libraries(${PROJECT_NAME} INTERFACE m)
endif ()

message("WITH_TMX ${WITH_TMX}")
if (WITH_TMX)
    find_package(LibXml2 REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(tmx REQUIRED)
    find_package(raylib-tmx REQUIRED)
    add_compile_definitions("WITH_TMX")
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib-tmx tmx LibXml2::LibXml2 ZLIB::ZLIB)
endif ()

if (SHOW_FPS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "SHOW_FPS")
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Install rules
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_TARGETS}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)

# Export the library for use with find_package
install(EXPORT ${PROJECT_TARGETS}
        FILE ${PROJECT_TARGETS}.cmake
        DESTINATION lib/cmake/${PROJECT_NAME}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CONFIG}Version.cmake"
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_CONFIG}.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CONFIG}.cmake"
        INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CONFIG}.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CONFIG}Version.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME}
)
