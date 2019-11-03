## Process this file with cmake
#==============================================================================
#  NeXus - Neutron & X-ray Common Data Format
#
#  CMakeLists for building the NeXus file validation library
#  and command line validator nxvalidate
#
#  Copyright (C) 2015 Mark Koennecke
#
#  This library is free software; you can redistribute it and/or modify it under
#  the terms of the GNU Lesser General Public License as published by the Free
#  Software Foundation; either version 2 of the License, or (at your option) any
#  later version.
#
#  This library is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
#  FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
#  details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this library; if not, write to the Free Software Foundation, Inc.,
#  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#  For further information, see <http://www.nexusformat.org>
#
#
#==============================================================================
cmake_minimum_required(VERSION 2.8.9)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

#The name of our project
project (cnxvalidate)
enable_testing()

message(STATUS ${PROJECT_SOURCE_DIR})
#set the module path
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_include")
message(STATUS ${CMAKE_MODULE_PATH})

#------------------------------------------------------------------------------
# set the default installation prefix
#------------------------------------------------------------------------------
if(WIN32)
    #don't know yet what to put here
else()
    if(NOT CMAKE_INSTALL_PREFIX)
        set(CMAKE_INSTALL_PREFIX "/usr/local")
    endif()
    #load module for GNU/Linux default installation directories
    include(GNUInstallDirs)
endif()

#-----------------------------------------------------------------------------
# define package version
#-----------------------------------------------------------------------------
set (cnxvalidate_VERSION_MAJOR 0)
set (cnxvalidate_VERSION_MINOR 5)
set (NXVERSINFO '1:0:0')

#-----------------------------------------------------------------------------
# include some general purpose modules
#-----------------------------------------------------------------------------
#include(Utilities)
#include(CompilerChecks)

include(FindPkgConfig)

#----------------------------------------------------------------
# Dependencies
#----------------------------------------------------------------
find_package(HDF5 REQUIRED)
find_package(LibXml2)

# This is an ugly hack to work around the fact that the default
# cmake HDF5 library search thing does not set this variable though
# it says it does. This is actually a known and fixed bug in cmake
# and the need  for this hack will go away when cmake has been
# updated to a 2016 version

set(HDF5_HL_LIBRARIES "/home/yaya/lib/libhdf5_hl.so")
#set(HDF5_HL_LIBRARIES "/usr/pkg/lib/libhdf5_hl.dylib")
#set(HDF5_HL_LIBRARIES "/afs/psi.ch/project/sinq/sl6/lib/libhdf5_hl.so")

#--------------------------------------------------------------
# Our own stuff, really....
#-------------------------------------------------------------
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)
include_directories(include)

link_directories(${HDF5_LIBRARY_DIRS} )

add_executable (nxvalidate nxvmain.c)



target_link_libraries(nxvalidate NXV_Static_Library
  ${HDF5_LIBRARIES} ${HDF5_HL_LIBRARIES} ${LIBXML2_LIBRARIES} )

install (TARGETS nxvalidate
         DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
#install (FILES nxvalidate.1
#         DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 COMPONENT Documentation)
