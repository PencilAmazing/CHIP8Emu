project (CHIP8Emu)

cmake_minimum_required(VERSION 3.1)


set(raylib_INSTALL_DIR "D:/dev/raylib-install")
set(raylib_LIBRARY "${raylib_INSTALL_DIR}/lib/raylib.lib")
set(raylib_INCLUDE_DIR "${raylib_INSTALL_DIR}/include")

set(raylib_DIR "${raylib_INSTALL_DIR}/lib/cmake/raylib")
find_package(raylib 4.2 REQUIRED)

add_executable(CHIP8Emu "src/chip8.c" "src/chip8.h" "src/host_io.h" "src/opcode.h" "src/opcode.c" "src/main.c")
set_property(TARGET CHIP8Emu PROPERTY C_STANDARD 99)

if(MSVC)
  target_compile_options(CHIP8Emu PUBLIC "/ZI")
  target_link_options(CHIP8Emu PUBLIC "/INCREMENTAL")
endif()

set_property(TARGET CHIP8Emu PROPERTY DEBUG_POSTFIX _d)
target_link_libraries(CHIP8Emu raylib)

# Copy ROMs to binary dir
configure_file("init.ch8" "." COPYONLY)
configure_file("test_opcode.ch8" "." COPYONLY)
configure_file("c8_test.ch8" "." COPYONLY)
configure_file("chip8-test-suite.ch8" "." COPYONLY)
configure_file("trip8.ch8" "." COPYONLY)
configure_file("maze.ch8" "." COPYONLY)
configure_file("${raylib_INSTALL_DIR}/bin/raylib.dll" "." COPYONLY)
