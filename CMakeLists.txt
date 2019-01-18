# Copyright (C) 2019  Diogo Justen. All rights reserved.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required (VERSION 3.0)
project (check-defects)
add_compile_options (-std=c++14 -Wall -Werror)

#### aravis
FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(ARAVIS aravis-0.6)
include_directories (${ARAVIS_INCLUDE_DIRS})

#### build
set (SOURCE_FILES
     src/main.cpp
     src/camera.cpp
    )

add_executable (check-defects ${SOURCE_FILES})
target_link_libraries (check-defects
    ${ARAVIS_LIBRARIES}
    )

#### build learning file
set (LEARNING_SOURCE_FILES
     src/main_c.cpp
    )

add_executable (check-defects-learning ${LEARNING_SOURCE_FILES})
target_link_libraries (check-defects-learning
    ${ARAVIS_LIBRARIES}
    )
