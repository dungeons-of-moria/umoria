cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

# Do not allow in-source builds
if (${CMAKE_SOURCE_DIR} STREQUAL "${PROJECT_SOURCE_DIR}/src")
    message(FATAL_ERROR "CMake generation is not allowed within the source directory!")
endif ()


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

# Compiler settings (this must come before calling project)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

project(umoria)

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

#
# Set the flags and warnings for the debug/release builds
#
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0 ${cxx_warnings}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 ${cxx_warnings}")


#
# Source files and directories
#
set(source_dir ${PROJECT_SOURCE_DIR}/src)
set(
        source_files
        ${source_dir}/config.h
        ${source_dir}/constant.h
        ${source_dir}/curses.h
        ${source_dir}/externs.h
        ${source_dir}/headers.h
        ${source_dir}/types.h
        ${source_dir}/version.h
        ${source_dir}/data_creatures.cpp
        ${source_dir}/data_player.cpp
        ${source_dir}/data_recall.cpp
        ${source_dir}/data_store_owners.cpp
        ${source_dir}/data_stores.cpp
        ${source_dir}/data_tables.cpp
        ${source_dir}/data_treasure.cpp
        ${source_dir}/main.cpp
        ${source_dir}/moria.cpp
        ${source_dir}/misc1.cpp
        ${source_dir}/misc2.cpp
        ${source_dir}/misc3.cpp
        ${source_dir}/misc4.cpp
        ${source_dir}/store1.cpp
        ${source_dir}/files.cpp
        ${source_dir}/io.cpp
        ${source_dir}/create.cpp
        ${source_dir}/desc.cpp
        ${source_dir}/generate.cpp
        ${source_dir}/sets.cpp
        ${source_dir}/dungeon.cpp
        ${source_dir}/creature.cpp
        ${source_dir}/death.cpp
        ${source_dir}/eat.cpp
        ${source_dir}/help.cpp
        ${source_dir}/magic.cpp
        ${source_dir}/potions.cpp
        ${source_dir}/prayer.cpp
        ${source_dir}/save.cpp
        ${source_dir}/staffs.cpp
        ${source_dir}/wands.cpp
        ${source_dir}/scrolls.cpp
        ${source_dir}/spells.cpp
        ${source_dir}/wizard.cpp
        ${source_dir}/store2.cpp
        ${source_dir}/moria1.cpp
        ${source_dir}/moria2.cpp
        ${source_dir}/moria3.cpp
        ${source_dir}/moria4.cpp
        ${source_dir}/monsters.cpp
        ${source_dir}/treasure.cpp
        ${source_dir}/globals.cpp
        ${source_dir}/rnd.cpp
        ${source_dir}/recall.cpp
        ${source_dir}/player.cpp
        ${source_dir}/helpers.cpp
)


#
# Set up the install paths and files
#
set(build_dir "umoria")
set(data_dir "${build_dir}/data")

file(MAKE_DIRECTORY ${build_dir})
file(MAKE_DIRECTORY ${data_dir})

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${build_dir})
set(RUNTIME_OUTPUT_DIRECTORY ${build_dir})

# Core game data files
set(
        data_files
        ${PROJECT_SOURCE_DIR}/data/help.txt
        ${PROJECT_SOURCE_DIR}/data/help_wizard.txt
        ${PROJECT_SOURCE_DIR}/data/rl_help.txt
        ${PROJECT_SOURCE_DIR}/data/rl_help_wizard.txt
        ${PROJECT_SOURCE_DIR}/data/splash.txt
        ${PROJECT_SOURCE_DIR}/data/versions.txt
        ${PROJECT_SOURCE_DIR}/data/welcome.txt
)
# Various support files (readme, etc.)
set(
        support_files
        ${PROJECT_SOURCE_DIR}/data/scores.dat
        ${PROJECT_SOURCE_DIR}/docs/manual.md
        ${PROJECT_SOURCE_DIR}/docs/faq.md
        ${PROJECT_SOURCE_DIR}/LICENSE
)

# All of the game resource files
set(resources ${data_files} ${support_files})

# Also add resources to the target so they are visible in the IDE
add_executable(umoria ${source_files} ${resources})


# This is horrible, but needed bacause `find_package()` doesn't use the
# include/lib inside the /mingw32 or /mingw64 directories, and with
# `ncurses-devel` installed, it won't compile.
if (_WIN32 AND "$ENV{MINGW}" STREQUAL "")
    message(FATAL_ERROR "You must set the MINGW environment variable ('mingw64' or 'mingw32'). Example: MINGW=mingw64 cmake .")
    message(FATAL_ERROR "This will be the directory used for locating the ncurses library files.")
elseif (_WIN32 AND NOT "$ENV{MINGW}" STREQUAL "")
    # Make the ncurses library static
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -static -lpthread")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static -lpthread")

    set(CURSES_INCLUDE_DIR "/$ENV{MINGW}/include/")
    set(CURSES_LIBRARIES "/$ENV{MINGW}/lib/libncurses.a")
else ()
    find_package(Curses REQUIRED)
endif ()

include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(umoria ${CURSES_LIBRARIES})

# Install (copy) the data files
install(FILES ${support_files} DESTINATION ${build_dir})
install(FILES ${data_files} DESTINATION ${data_dir})

# Build and install the umoria binary
install(TARGETS umoria DESTINATION ${build_dir})

#
# Extract the Umoria version number from version.h
#
file(STRINGS "${source_dir}/version.h" VERSION_HEADER)

string(REGEX MATCH "CURRENT_VERSION_MAJOR = ([0-9]+);" ${VERSION_HEADER})
set(UMORIA_VERSION_MAJOR ${CMAKE_MATCH_1})

string(REGEX MATCH "CURRENT_VERSION_MINOR = ([0-9]+);" ${VERSION_HEADER})
set(UMORIA_VERSION_MINOR ${CMAKE_MATCH_1})

string(REGEX MATCH "CURRENT_VERSION_PATCH = ([0-9]+);" ${VERSION_HEADER})
set(UMORIA_VERSION_PATCH ${CMAKE_MATCH_1})

set(UMORIA_VERSION "${UMORIA_VERSION_MAJOR}.${UMORIA_VERSION_MINOR}.${UMORIA_VERSION_PATCH}")


#
# CPack configuration
#
include(InstallRequiredSystemLibraries)

set(CPACK_MONOLITHIC_INSTALL TRUE)

set(CPACK_PACKAGE_NAME "Umoria")
set(CPACK_PACKAGE_VENDOR "umoria.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Umoria: The Dungeons of Moria - A Classic Roguelike Game")
set(CPACK_PACKAGE_VERSION ${UMORIA_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${UMORIA_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${UMORIA_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${UMORIA_VERSION_PATCH})

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

if (_WIN32 AND NOT UNIX)
    message(FATAL_ERROR "It is not currently possible to build a .zip archive for Windows!")
endif ()

if (APPLE)
    set(CPACK_SYSTEM_NAME "macOS")
endif ()

set(CPACK_GENERATOR "ZIP")

include(CPack)
