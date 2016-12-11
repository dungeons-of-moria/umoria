cmake_minimum_required(VERSION 3.6)

project(umoria)

# Current version number
set(CURRENT_VERSION_MAJOR 5)
set(CURRENT_VERSION_MINOR 7)
set(CURRENT_VERSION_PATCH 0)

set(SOURCE_FILES_DIR ${PROJECT_SOURCE_DIR}/src)

# NOTE: currently disabled until Windows/MinGW support is added.
# configure a header file to pass some of the CMake settings to the source code
#configure_file(
#    "${SOURCE_FILES_DIR}/version.h.in"
#    "${PROJECT_BINARY_DIR}/version.h"
#)
## include directory for generated version.h
#include_directories("${PROJECT_BINARY_DIR}")

# add the binary tree to the search path for include files so that we will find header files
include_directories(${SOURCE_FILES_DIR})

set(SOURCE_FILES
    ${SOURCE_FILES_DIR}/main.c
    ${SOURCE_FILES_DIR}/misc1.c
    ${SOURCE_FILES_DIR}/misc2.c
    ${SOURCE_FILES_DIR}/misc3.c
    ${SOURCE_FILES_DIR}/misc4.c
    ${SOURCE_FILES_DIR}/store1.c
    ${SOURCE_FILES_DIR}/files.c
    ${SOURCE_FILES_DIR}/io.c
    ${SOURCE_FILES_DIR}/create.c
    ${SOURCE_FILES_DIR}/desc.c
    ${SOURCE_FILES_DIR}/generate.c
    ${SOURCE_FILES_DIR}/sets.c
    ${SOURCE_FILES_DIR}/dungeon.c
    ${SOURCE_FILES_DIR}/creature.c
    ${SOURCE_FILES_DIR}/death.c
    ${SOURCE_FILES_DIR}/eat.c
    ${SOURCE_FILES_DIR}/help.c
    ${SOURCE_FILES_DIR}/magic.c
    ${SOURCE_FILES_DIR}/potions.c
    ${SOURCE_FILES_DIR}/prayer.c
    ${SOURCE_FILES_DIR}/save.c
    ${SOURCE_FILES_DIR}/staffs.c
    ${SOURCE_FILES_DIR}/wands.c
    ${SOURCE_FILES_DIR}/scrolls.c
    ${SOURCE_FILES_DIR}/spells.c
    ${SOURCE_FILES_DIR}/wizard.c
    ${SOURCE_FILES_DIR}/store2.c
    ${SOURCE_FILES_DIR}/moria1.c
    ${SOURCE_FILES_DIR}/moria2.c
    ${SOURCE_FILES_DIR}/moria3.c
    ${SOURCE_FILES_DIR}/moria4.c
    ${SOURCE_FILES_DIR}/monsters.c
    ${SOURCE_FILES_DIR}/treasure.c
    ${SOURCE_FILES_DIR}/variable.c
    ${SOURCE_FILES_DIR}/rnd.c
    ${SOURCE_FILES_DIR}/recall.c
    ${SOURCE_FILES_DIR}/player.c
    ${SOURCE_FILES_DIR}/tables.c
)
add_executable(umoria ${SOURCE_FILES})

set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -g -Wall -Werror -Wshadow -std=gnu11")

# Link Curses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(umoria ${CURSES_LIBRARIES})

# add the package install targets
install(TARGETS umoria
    RUNTIME
    DESTINATION "./"
    COMPONENT applications
)

set(DATA_FILES
    ${PROJECT_SOURCE_DIR}/data/origcmds.hlp
    ${PROJECT_SOURCE_DIR}/data/owizcmds.hlp
    ${PROJECT_SOURCE_DIR}/data/roglcmds.hlp
    ${PROJECT_SOURCE_DIR}/data/rwizcmds.hlp
    ${PROJECT_SOURCE_DIR}/data/splash.hlp
    ${PROJECT_SOURCE_DIR}/data/version.hlp
    ${PROJECT_SOURCE_DIR}/data/welcome.hlp
)
install(FILES ${DATA_FILES}
    DESTINATION "data"
    COMPONENT gameData
)

set(SUPPORT_FILES
    ${PROJECT_SOURCE_DIR}/data/scores.dat
    ${PROJECT_SOURCE_DIR}/docs/manual.md
    ${PROJECT_SOURCE_DIR}/docs/faq.md
    ${PROJECT_SOURCE_DIR}/LICENSE
)
install(FILES ${SUPPORT_FILES}
    DESTINATION "./"
    COMPONENT supportData
)

# CPack configuration

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_VENDOR "umoria.org")

set(CPACK_PACKAGE_DESCRIPTION "Umoria")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Umoria: The Dungeons of Moria - A Classic Roguelike")
set(CPACK_PACKAGE_VERSION_MAJOR ${CURRENT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CURRENT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CURRENT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

set(CPACK_COMPONENTS_ALL application gameData supportData)

if(WIN32 AND NOT UNIX)
    # Windows MinGW not currently support, please use makefile.win.
else(WIN32 AND NOT UNIX)
    set(CPACK_GENERATOR "ZIP")

    if(APPLE)
        set(CPACK_SYSTEM_NAME "macos")
    endif(APPLE)
endif(WIN32 AND NOT UNIX)

include(CPack)
