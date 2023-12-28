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
#
# Locates and prepares LodePNG for usage with CMAKE.
#
# This module creates the following defines:
#
#   LodePNG_FOUND
#   LODEPNG_INCLUDE_DIRS
#
####################################################################################################

find_path(LODEPNG_INCLUDE_DIRS
    NAMES
        "lodepng.h"
    PATHS
        "ext/lodepng/"
    )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LodePNG
    DEFAULT_MSG
        LODEPNG_INCLUDE_DIRS
    )

mark_as_advanced(LODEPNG_INCLUDE_DIRS)
