cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

# Do not allow in-source builds
if (${CMAKE_SOURCE_DIR} STREQUAL "${PROJECT_SOURCE_DIR}/src")
    message(FATAL_ERROR "CMake generation is not allowed within the source directory!")
endif ()

project(umoria
    LANGUAGES CXX
)

#
# Set a default build type
#
set(default_build_type "Release")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
else ()
    message(STATUS "Build type set to '${CMAKE_BUILD_TYPE}'")
endif ()

# Compiler settings
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

#
# Core set of warnings
#
set(cxx_warnings "-Wall")
set(cxx_warnings "${cxx_warnings} -Wextra")
set(cxx_warnings "${cxx_warnings} -Wpedantic")
set(cxx_warnings "${cxx_warnings} -Wshadow")
set(cxx_warnings "${cxx_warnings} -Werror")
set(cxx_warnings "${cxx_warnings} -pedantic-errors")
set(cxx_warnings "${cxx_warnings} -Weffc++ ")

# Additional warnings
set(cxx_warnings "${cxx_warnings} -Wcast-align")
set(cxx_warnings "${cxx_warnings} -Wdisabled-optimization")
set(cxx_warnings "${cxx_warnings} -Wfloat-equal")
set(cxx_warnings "${cxx_warnings} -Winline")
set(cxx_warnings "${cxx_warnings} -Winvalid-pch")
set(cxx_warnings "${cxx_warnings} -Wmissing-format-attribute")
set(cxx_warnings "${cxx_warnings} -Wmissing-include-dirs")
set(cxx_warnings "${cxx_warnings} -Wpacked")
set(cxx_warnings "${cxx_warnings} -Wredundant-decls")
set(cxx_warnings "${cxx_warnings} -Wswitch-default")
set(cxx_warnings "${cxx_warnings} -Wswitch-enum")
set(cxx_warnings "${cxx_warnings} -Wunreachable-code")
set(cxx_warnings "${cxx_warnings} -Wwrite-strings")

# Some very strict warnings, that will be nice to use, but require some hefty refactoring
# set(cxx_warnings "${cxx_warnings} -Wcast-qual")
# set(cxx_warnings "${cxx_warnings} -Wconversion")
# set(cxx_warnings "${cxx_warnings} -Wformat=2")
# set(cxx_warnings "${cxx_warnings} -Wpadded")
# set(cxx_warnings "${cxx_warnings} -Wstrict-overflow")
# set(cxx_warnings "${cxx_warnings} -fno-strict-aliasing")

# Temporary support for GCC 8.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(cxx_warnings "${cxx_warnings} -Wno-format-overflow")
endif()

# Ignore string truncation warnings for using snprintf
set(cxx_warnings "${cxx_warnings} -Wno-format-truncation")

#
# Set the flags and warnings for the debug/release builds
#
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0 ${cxx_warnings}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 ${cxx_warnings}")


#
# Source files and directories
#
set(source_dir src)
set(
        source_files
        ${source_dir}/character.h
        ${source_dir}/config.h
        ${source_dir}/curses.h
        ${source_dir}/dice.h
        ${source_dir}/dungeon.h
        ${source_dir}/dungeon_tile.h
        ${source_dir}/game.h
        ${source_dir}/headers.h
        ${source_dir}/helpers.h
        ${source_dir}/identification.h
        ${source_dir}/inventory.h
        ${source_dir}/mage_spells.h
        ${source_dir}/monster.h
        ${source_dir}/player.h
        ${source_dir}/recall.h
        ${source_dir}/rng.h
        ${source_dir}/scores.h
        ${source_dir}/scrolls.h
        ${source_dir}/spells.h
        ${source_dir}/staves.h
        ${source_dir}/store.h
        ${source_dir}/treasure.h
        ${source_dir}/types.h
        ${source_dir}/ui.h
        ${source_dir}/version.h
        ${source_dir}/wizard.h
        ${source_dir}/config.cpp
        ${source_dir}/helpers.cpp
        ${source_dir}/rng.cpp
        ${source_dir}/main.cpp
        ${source_dir}/data_creatures.cpp
        ${source_dir}/data_player.cpp
        ${source_dir}/data_recall.cpp
        ${source_dir}/data_store_owners.cpp
        ${source_dir}/data_stores.cpp
        ${source_dir}/data_tables.cpp
        ${source_dir}/data_treasure.cpp
        ${source_dir}/character.cpp
        ${source_dir}/dice.cpp
        ${source_dir}/dungeon.cpp
        ${source_dir}/dungeon_generate.cpp
        ${source_dir}/dungeon_los.cpp
        ${source_dir}/game.cpp
        ${source_dir}/game_death.cpp
        ${source_dir}/game_files.cpp
        ${source_dir}/game_objects.cpp
        ${source_dir}/game_run.cpp
        ${source_dir}/game_save.cpp
        ${source_dir}/identification.cpp
        ${source_dir}/inventory.cpp
        ${source_dir}/mage_spells.cpp
        ${source_dir}/monster.cpp
        ${source_dir}/monster_manager.cpp
        ${source_dir}/player.cpp
        ${source_dir}/player_bash.cpp
        ${source_dir}/player_eat.cpp
        ${source_dir}/player_magic.cpp
        ${source_dir}/player_move.cpp
        ${source_dir}/player_pray.cpp
        ${source_dir}/player_quaff.cpp
        ${source_dir}/player_run.cpp
        ${source_dir}/player_stats.cpp
        ${source_dir}/player_throw.cpp
        ${source_dir}/player_traps.cpp
        ${source_dir}/player_tunnel.cpp
        ${source_dir}/recall.cpp
        ${source_dir}/scores.cpp
        ${source_dir}/scrolls.cpp
        ${source_dir}/spells.cpp
        ${source_dir}/staves.cpp
        ${source_dir}/store.cpp
        ${source_dir}/store_inventory.cpp
        ${source_dir}/treasure.cpp
        ${source_dir}/ui.cpp
        ${source_dir}/ui_inventory.cpp
        ${source_dir}/ui_io.cpp
        ${source_dir}/wizard.cpp
)


#
# Set up the install paths and files
#
set(build_dir "umoria")
set(data_dir "${build_dir}/data")


set(EXECUTABLE_OUTPUT_PATH ${build_dir})

# Core game data files
set(
        data_files
        data/help.txt
        data/help_wizard.txt
        data/rl_help.txt
        data/rl_help_wizard.txt
        data/welcome.txt
        data/death_tomb.txt
        data/death_royal.txt
)
file(COPY ${data_files} DESTINATION "${data_dir}")

# Various support files (readme, etc.)
set(
        support_files
        data/scores.dat
        AUTHORS
        LICENSE
)
file(COPY ${support_files} DESTINATION "${build_dir}")


#
# Extract the Umoria version number from version.h
#
file(STRINGS "${source_dir}/version.h" VERSION_HEADER)

string(REGEX MATCH "CURRENT_VERSION_MAJOR = ([0-9]+);" ${VERSION_HEADER})
set(umoria_version_major ${CMAKE_MATCH_1})

string(REGEX MATCH "CURRENT_VERSION_MINOR = ([0-9]+);" ${VERSION_HEADER})
set(umoria_version_minor ${CMAKE_MATCH_1})

string(REGEX MATCH "CURRENT_VERSION_PATCH = ([0-9]+);" ${VERSION_HEADER})
set(umoria_version_patch ${CMAKE_MATCH_1})

set(umoria_version "${umoria_version_major}.${umoria_version_minor}.${umoria_version_patch}")


#
# Update the data files with the current version number and date
#

# Fetch release date from CHANGELOG if it's there :-)
file(READ "CHANGELOG.md" changelog)
string(REGEX MATCH "## ${umoria_version} \\(([0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9])\\)" results ${changelog})
list(LENGTH results match_count)
if (match_count EQUAL 0)
    string(TIMESTAMP current_date "%Y-%m-%d" UTC)
else ()
    set(current_date ${CMAKE_MATCH_1})
endif ()

set(
        data_files_to_update
        data/splash.txt
        data/versions.txt
)
foreach (data_file ${data_files_to_update})
    configure_file(${data_file}.in ${build_dir}/${data_file})
endforeach ()


# All of the game resource files
set(resources ${data_files} ${support_files})

# Also add resources to the target so they are visible in the IDE
add_executable(umoria ${source_files} ${resources})


#
# Get around the fact that Visual Studio doesn't have ssize_t
# defined but uses SSIZE_T instead.
#
if(MSVC)
    add_definitions(-Dssize_t=SSIZE_T)
endif(MSVC)

# This is horrible, but needed bacause `find_package()` doesn't use the
# include/lib inside the /mingw32 or /mingw64 directories, and with
# `ncurses-devel` installed, it won't compile.
if ((MSYS OR MINGW) AND "$ENV{MINGW}" STREQUAL "")
    message(FATAL_ERROR "You must set the MINGW environment variable ('mingw64' or 'mingw32'). Example: MINGW=mingw64 cmake .")
    message(FATAL_ERROR "This will be the directory used for locating the ncurses library files.")
elseif ((MSYS OR MINGW) AND NOT "$ENV{MINGW}" STREQUAL "")
    message(STATUS "NOTE: Configuring build for Windows release...")

    # Make the ncurses library static
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -static -lpthread")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static -lpthread")

    set(CURSES_INCLUDE_DIR "/$ENV{MINGW}/include/")
    set(CURSES_LIBRARIES "/$ENV{MINGW}/lib/libncurses.a")
else ()
    message(STATUS "NOTE: Configuring build for macOS/Linux release...")
    set(CURSES_NEED_NCURSES TRUE)
    find_package(Curses REQUIRED)
endif ()

include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(umoria ${CURSES_LIBRARIES})
