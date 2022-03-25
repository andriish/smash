# This cmake code to integrate Pythia contains contributions by 
# K. Gallmeister, Goethe University, March 2015
# <gallmei@th.physik.uni-frankfurt.de>
# and by
# A. Verbytskyi, Max-Planck Institute für Physics, January 2022
# <andrii.verbytskyi@mpp.mpg.de>


# - Locate pythia library
#
# Exploits the environment variables:
#  PYTHIA_ROOT_DIR or PYTHIA8
# or the options
#  -DPythia_CONFIG_EXECUTABLE or -DPYTHIA_ROOT_DIR
#
# Tries to find the version defined in the variable:
#
#  Pythia_VERSION
#
# Defines:
#
#  Pythia_FOUND
#  Pythia_VERSION
#  Pythia_INCLUDE_DIR
#  PYTHIA_XMLDOC_DIR
#  Pythia_xmldoc_PATH
#  Pythia_INCLUDE_DIRS
#  Pythia_LIBRARY
#  Pythia_LIBDIR
#  Pythia_LHAPDFDummy_LIBRARY
#  Pythia_LIBRARIES : includes 3 libraries above; not to be used if lhapdf is used


if(NOT("${Pythia_CONFIG_EXECUTABLE}" STREQUAL ""))
    MESSAGE(STATUS "Trying to locate Pythia using variable Pythia_CONFIG_EXECUTABLE = ${Pythia_CONFIG_EXECUTABLE}")
    find_program(Pythia_CONFIG_EXECUTABLE NAMES pythia8-config)
    if(${Pythia_CONFIG_EXECUTABLE} MATCHES "Pythia_CONFIG_EXECUTABLE-NOTFOUND")
        MESSAGE(FATAL_ERROR "pythia8-config executable not found, please check \"-DPythia_CONFIG_EXECUTABLE\" or use \"-DPYTHIA_ROOT_DIR\"")
    else()
        EXEC_PROGRAM(${Pythia_CONFIG_EXECUTABLE} ARGS "--prefix" OUTPUT_VARIABLE PYTHIA_ROOT_DIR)
    endif()
endif()  

if("${PYTHIA_ROOT_DIR}" STREQUAL "")
    if(DEFINED ENV{PYTHIA8})
        MESSAGE(STATUS "Trying to locate Pythia using the environment variable PYTHIA8 = $ENV{PYTHIA8}")
        set(PYTHIA_ROOT_DIR $ENV{PYTHIA8})
    elseif(DEFINED ENV{PYTHIA_ROOT_DIR})
        MESSAGE(STATUS "Trying to locate Pythia using the environment variable PYTHIA_ROOT_DIR = $ENV{PYTHIA_ROOT_DIR}")
        set(PYTHIA_ROOT_DIR  $ENV{PYTHIA_ROOT_DIR})
    else()
        MESSAGE(STATUS "The installation directory of Pythia can be specified by setting the environment variables PYTHIA_ROOT_DIR or PYTHIA8\n"
                       "   or either with \"-DPythia_CONFIG_EXECUTABLE\" or \"-DPYTHIA_ROOT_DIR\", but none of these options was used.\n"
                       "   Trying to proceed assuming that Pythia is installed under /usr.")
        set(PYTHIA_ROOT_DIR  "/usr")
    endif()
endif()

find_path(Pythia_INCLUDE_DIR Pythia.h Pythia8/Pythia.h HINTS  ${PYTHIA_ROOT_DIR}/include)

find_path(Pythia_XMLDOC_DIR Version.xml HINTS  ${PYTHIA_ROOT_DIR}/xmldoc  ${PYTHIA_ROOT_DIR}/share/Pythia8/xmldoc ${PYTHIA_ROOT_DIR}/share/pythia8-data/xmldoc  ${PYTHIA_ROOT_DIR}/share/doc/packages/pythia/xmldoc) 

if(Pythia_INCLUDE_DIR AND Pythia_XMLDOC_DIR)
    find_library(Pythia_LIBRARY NAMES pythia8 Pythia8 HINTS ${PYTHIA_ROOT_DIR}/lib ${PYTHIA_ROOT_DIR}/lib64)
    get_filename_component(Pythia_LIBDIR ${Pythia_LIBRARY} DIRECTORY)
    find_library(Pythia_LHAPDFDummy_LIBRARY NAMES lhapdfdummy  HINTS ${PYTHIA_ROOT_DIR}/lib ${PYTHIA_ROOT_DIR}/lib64)
    set(Pythia_INCLUDE_DIRS ${Pythia_INCLUDE_DIR} ${Pythia_INCLUDE_DIR}/Pythia ${Pythia_INCLUDE_DIR}/PythiaPlugins)
    set(Pythia_LIBRARIES ${Pythia_LIBRARY})
    file(READ ${Pythia_INCLUDE_DIR}/Pythia8/Pythia.h Pythia_header)
    string(REGEX MATCH "#define PYTHIA_VERSION_INTEGER ([0-9])([0-9][0-9][0-9])" _ ${Pythia_header})
    set(Pythia_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(Pythia_VERSION_MINOR ${CMAKE_MATCH_2})
    set(Pythia_VERSION ${Pythia_VERSION_MAJOR}.${Pythia_VERSION_MINOR})
    if(NOT(${Pythia_VERSION} VERSION_EQUAL ${Pythia_FIND_VERSION}))
         MESSAGE(STATUS "** WRONG Pythia version: ${Pythia_VERSION},"
                        " required ${Pythia_FIND_VERSION}." )
         set(Pythia_VERSION_OK FALSE)
         set(Pythia_FOUND FALSE)
    else()
         MESSAGE(STATUS "** Pythia version ok: ${Pythia_VERSION},"
                        " required ${Pythia_FIND_VERSION}.")
         set(Pythia_VERSION_OK TRUE)
         set(Pythia_FOUND TRUE)
    endif()
else()
    set(Pythia_FOUND FALSE)
endif()

SET(Pythia_INCLUDE_DIRS  ${Pythia_INCLUDE_DIR})
SET(Pythia_xmldoc_PATH   ${Pythia_XMLDOC_DIR})

# handle the QUIETLY and REQUIRED arguments
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS( Pythia DEFAULT_MSG Pythia_LIBRARY Pythia_INCLUDE_DIR Pythia_xmldoc_PATH)

# display some status information
IF(Pythia_FOUND)
    MESSAGE(STATUS "** Pythia 8 library: ${Pythia_LIBRARIES}")
    MESSAGE(STATUS "** Pythia 8 include: ${Pythia_INCLUDE_DIRS}")
    MESSAGE(STATUS "** Pythia 8 xmldoc:  ${Pythia_xmldoc_PATH}")
ENDIF()

# the variables listed here will only show up in the GUI (ccmake) in the "advanced" view
mark_as_advanced(Pythia_FOUND Pythia_INCLUDE_DIR Pythia_LIBRARY Pythia_LIBRARIES Pythia_LHAPDFDummy_LIBRARY Pythia_XMLDOC_DIR Pythia_LIBDIR Pythia_CONFIG_EXECUTABLE)
