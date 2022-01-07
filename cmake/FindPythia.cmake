# - Locate pythia library
# Defines:
#
#  Pythia_FOUND
#  PYTHIA_VERSION
#  PYTHIA_INCLUDE_DIR
#  PYTHIA_XMLDOC_DIR
#  PYTHIA_INCLUDE_DIRS (not cached)
#  PYTHIA_LIBRARY
#  PYTHIA_hepmcinterface_LIBRARY
#  PYTHIA_lhapdfdummy_LIBRARY
#  PYTHIA_LIBRARIES (not cached) : includes 3 libraries above; not to be used if lhapdf is used
set(TEST_PYTHIA_ROOT_DIR  "" ${PYTHIA_ROOT_DIR})
IF(TEST_PYTHIA_ROOT_DIR STREQUAL "")
  IF(DEFINED ENV{PYTHIA_ROOT_DIR})
    set(PYTHIA_ROOT_DIR  $ENV{PYTHIA_ROOT_DIR})
  else()
    set(PYTHIA_ROOT_DIR  "/usr")
  endif()
endif()

find_path(PYTHIA_INCLUDE_DIR Pythia.h Pythia8/Pythia.h HINTS  ${PYTHIA_ROOT_DIR}/include)

find_path(PYTHIA_XMLDOC_DIR Version.xml HINTS  ${PYTHIA_ROOT_DIR}/xmldoc  ${PYTHIA_ROOT_DIR}/share/Pythia8/xmldoc ${PYTHIA_ROOT_DIR}/share/pythia8-data/xmldoc  ${PYTHIA_ROOT_DIR}/share/doc/packages/pythia/xmldoc )

if(PYTHIA_INCLUDE_DIR AND PYTHIA_XMLDOC_DIR)
  file(READ ${PYTHIA_XMLDOC_DIR}/Version.xml versionstr)
  string(REGEX REPLACE ".*Pythia:versionNumber.*default.*[0-9][.]([0-9]+).*" "\\1" PYTHIA_VERSION "${versionstr}")
  set(PYTHIA_VERSION "8.${PYTHIA_VERSION}")
  find_library(PYTHIA_LIBRARY NAMES pythia8 Pythia8 HINTS ${PYTHIA_ROOT_DIR}/lib ${PYTHIA_ROOT_DIR}/lib64)
  find_library(PYTHIA_lhapdfdummy_LIBRARY NAMES lhapdfdummy  HINTS ${PYTHIA_ROOT_DIR}/lib ${PYTHIA_ROOT_DIR}/lib64)
  set(PYTHIA_INCLUDE_DIRS ${PYTHIA_INCLUDE_DIR} ${PYTHIA_INCLUDE_DIR}/Pythia ${PYTHIA_INCLUDE_DIR}/PythiaPlugins )
  set(PYTHIA_LIBRARIES ${PYTHIA_LIBRARY})
  if(PYTHIA_VERSION VERSION_LESS 8.200)
    #Is this library needed?
    set(PYTHIA_LIBRARIES ${PYTHIA_LIBRARY} ${PYTHIA_lhapdfdummy_LIBRARY})
  endif()
  find_file(resHEPMC3 HepMC3.h PATHS  ${PYTHIA_INCLUDE_DIRS} NO_DEFAULT_PATH)
  if (resHEPMC3)
    set(Pythia_HEPMC3_FOUND TRUE)
  endif()
endif()

# handle the QUIETLY and REQUIRED arguments and set Pythia_FOUND to TRUE if
# all listed variables are TRUE

SET(Pythia_INCLUDE_DIRS  ${PYTHIA_INCLUDE_DIR})
SET(Pythia_LIBDIR       ${PYTHIA_INCLUDE_DIR}/../lib64)
SET(Pythia_LIBRARIES   ${PYTHIA_LIBRARIES} )
SET(Pythia_xmldoc_PATH  ${PYTHIA_XMLDOC_DIR})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Pythia REQUIRED_VARS PYTHIA_INCLUDE_DIR PYTHIA_LIBRARIES PYTHIA_XMLDOC_DIR VERSION_VAR PYTHIA_VERSION HANDLE_COMPONENTS)

mark_as_advanced(Pythia_FOUND PYTHIA_INCLUDE_DIR PYTHIA_LIBRARY PYTHIA_LIBRARIES PYTHIA_XMLDOC_DIR)

