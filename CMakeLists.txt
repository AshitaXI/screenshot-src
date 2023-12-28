####################################################################################################
# Ashita - Copyright (c) 2023 Ashita Development Team
# Contact: https://www.ashitaxi.com/
# Contact: https://discord.gg/Ashita
#
# This file is part of Ashita.
#
# Ashita is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Ashita is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Ashita.  If not, see <https://www.gnu.org/licenses/>.
####################################################################################################

cmake_minimum_required(VERSION 3.22)

# Set the project..
set(ASHITA_PROJECT_NAME "screenshot")
project(${ASHITA_PROJECT_NAME} CXX)

# Set the CMAKE module path..
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(LodePNG REQUIRED)

# Enable parallel building..
if (NOT CMAKE_BUILD_PARALLEL_LEVEL)
    include(ProcessorCount)
    ProcessorCount(N)

    if (NOT N EQUAL 0)
        set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
    endif()
endif()

# Require the Ashita SDK..
find_package(AshitaSDK REQUIRED)
ashita_sdk_initialize()

# Set the project sources..
set(${ASHITA_PROJECT_NAME}_res
    )

set(${ASHITA_PROJECT_NAME}_src
    "src/defines.hpp"
    "src/exports.def"
    "src/screenshot.cpp"
    "src/screenshot.hpp"

    "ext/lodepng/lodepng.cpp"
    )

# Define the library..
add_library(${ASHITA_PROJECT_NAME} MODULE ${${ASHITA_PROJECT_NAME}_res} ${${ASHITA_PROJECT_NAME}_src})
set_target_properties(${ASHITA_PROJECT_NAME} PROPERTIES
    CXX_EXTENSIONS ON           # Enable CXX extensions..
    CXX_STANDARD_REQUIRED ON    # Mark the CXX standard option as a requirement..
    LINKER_LANGUAGE CXX         # Sets the linker language to C++..
    )

if (WIN32)
    set_target_properties(${ASHITA_PROJECT_NAME} PROPERTIES
        OUTPUT_NAME ${ASHITA_PROJECT_NAME}
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
        )
endif()

# Attach and configure the Ashita SDK..
ashita_sdk_attach(${ASHITA_PROJECT_NAME})
ashita_sdk_set_compiler_defaults(${ASHITA_PROJECT_NAME})
ashita_sdk_set_compiler_warn_level(${ASHITA_PROJECT_NAME} 4)
ashita_sdk_set_compiler_warnings_as_errors(${ASHITA_PROJECT_NAME})
ashita_sdk_set_linker_defaults(${ASHITA_PROJECT_NAME})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    ashita_sdk_set_linker_enable_debug_linking(${ASHITA_PROJECT_NAME})
else()
    ashita_sdk_set_linker_disable_debug_linking(${ASHITA_PROJECT_NAME})
endif()

# Note: This must be disabled if compiling with Direct3D calls!
#ashita_sdk_set_linker_enable_safe_seh(${ASHITA_PROJECT_NAME})

# Include LodePNG..
target_include_directories(${ASHITA_PROJECT_NAME}
    PRIVATE
        ${LODEPNG_INCLUDE_DIRS}
    )
