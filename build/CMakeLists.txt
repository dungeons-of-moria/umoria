cmake_minimum_required(VERSION 3.6)
project(moria)

# Version number
set (moria_VERSION_MAJOR 5)
set (moria_VERSION_MINOR 7)
set (moria_VERSION_PATCH 0)

# configure a header file to pass some of the CMake settings to the source code
# configure_file("${PROJECT_BINARY_DIR}/src/constants.h")

# add the binary tree to the search path for include files
# so that we will find header files
include_directories("${PROJECT_BINARY_DIR}")

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu11")

set(SOURCE_FILES
    src/config.h
    src/constant.h
    src/types.h
    src/externs.h
    src/main.c
    src/misc1.c
    src/misc2.c
    src/misc3.c
    src/misc4.c
    src/store1.c
    src/files.c
    src/io.c
    src/create.c
    src/desc.c
    src/generate.c
    src/sets.c
    src/dungeon.c
    src/creature.c
    src/death.c
    src/eat.c
    src/help.c
    src/magic.c
    src/potions.c
    src/prayer.c
    src/save.c
    src/staffs.c
    src/wands.c
    src/scrolls.c
    src/spells.c
    src/wizard.c
    src/store2.c
    src/moria1.c
    src/moria2.c
    src/moria3.c
    src/moria4.c
    src/monsters.c
    src/treasure.c
    src/variable.c
    src/rnd.c
    src/recall.c
    src/unix.c
    src/player.c
    src/tables.c
)
add_executable(moria ${SOURCE_FILES})

# Link Curses
target_link_libraries(moria ${CURSES_LIBRARIES})


# add the install targets
install (TARGETS moria DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/src/constants.h" DESTINATION include)

# build a CPack driven installer package
# include (InstallRequiredSystemLibraries)
# set (CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/LICENSE")
# set (CPACK_PACKAGE_VERSION_MAJOR "${moria_VERSION_MAJOR}")
# set (CPACK_PACKAGE_VERSION_MINOR "${moria_VERSION_MINOR}")
# set (CPACK_PACKAGE_VERSION_MINOR "${moria_VERSION_PATCH}")
# include (CPack)
