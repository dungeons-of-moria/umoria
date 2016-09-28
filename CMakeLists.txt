cmake_minimum_required(VERSION 3.6)
project(moria)

# Version number configuration
set (moria_VERSION_MAJOR 5)
set (moria_VERSION_MINOR 5)
set (moria_VERSION_PATCH 2)

# configure a header file to pass some of the CMake settings to the source code
# configure_file("${PROJECT_BINARY_DIR}/constants.h")

# add the binary tree to the search path for include files
# so that we will find constants.h
include_directories("${PROJECT_BINARY_DIR}")

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")

set(SOURCE_FILES
    src/config.h
    src/constant.h
    src/create.c
    src/creature.c
    src/death.c
    src/desc.c
    src/dungeon.c
    src/eat.c
    src/externs.h
    src/files.c
    src/generate.c
    src/help.c
    src/io.c
    src/magic.c
    src/main.c
    src/makefile
    src/misc1.c
    src/misc2.c
    src/misc3.c
    src/misc4.c
    src/monsters.c
    src/moria1.c
    src/moria2.c
    src/moria3.c
    src/moria4.c
    src/player.c
    src/potions.c
    src/prayer.c
    src/recall.c
    src/rnd.c
    src/save.c
    src/scrolls.c
    src/sets.c
    src/signals.c
    src/spells.c
    src/staffs.c
    src/store1.c
    src/store2.c
    src/tables.c
    src/treasure.c
    src/types.h
    src/unix.c
    src/variable.c
    src/wands.c
    src/wizard.c
)

add_executable(moria ${SOURCE_FILES})

# Link Curses
target_link_libraries(moria ${CURSES_LIBRARIES})
