project(wjelement-cpp)
cmake_minimum_required(VERSION 2.6)
include(CheckIncludeFiles)
include(CheckSymbolExists)

if(NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX /usr/local)
endif(NOT CMAKE_INSTALL_PREFIX)
set(INC_DEST_DIR ${CMAKE_INSTALL_PREFIX}/include)
if(NOT LIB_DEST_DIR)
  set(LIB_DEST_DIR ${CMAKE_INSTALL_PREFIX}/lib)
endif(NOT LIB_DEST_DIR)
set(BIN_DEST_DIR ${CMAKE_INSTALL_PREFIX}/bin)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DDEBUG)
  option(SHOWNOTES "Show preprocessor notes" OFF)

  if(CMAKE_COMPILER_IS_GNUCC)
	# GCC specific debug options
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g3 -ggdb3 -gdwarf-2")
	set(AVOID_VERSION -avoid-version)
  endif(CMAKE_COMPILER_IS_GNUCC)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

add_definitions(-DHAVE_CONFIG_H)


check_include_files(string.h HAVE_STRING_H)
check_symbol_exists(strcasecmp "strings.h" HAVE_STRCASECMP)

check_include_files(regex.h HAVE_REGEX_H)
if(HAVE_REGEX_H)
	add_definitions(-DHAVE_REGEX_H)
else(HAVE_REGEX_H)
	message("***  WARNING: GNU C regex library not found.")
	message("     WJESchemaValidate() will not support patternProperties")
endif(HAVE_REGEX_H)

#set(OPENSSL_LIBS ssl crypto)
#set(PTHREAD_LIBS pthread)

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
# the RPATH to be used when installing
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
ENABLE_TESTING(1)

include_directories(include)

if(CMAKE_BOOST_INCLUDE)
    include_directories(${CMAKE_BOOST_INCLUDE})
else(CMAKE_BOOST_INCLUDE)
    message(FATAL_ERROR,"You have not  defined CMAKE_BOOST_INCLUDE")
endif(CMAKE_BOOST_INCLUDE)

if(CMAKE_BOOST_LIB)
    link_directories(${CMAKE_BOOST_LIB})
else(CMAKE_BOOST_LIB)
    message(FATAL_ERROR,"You have not  defined CMAKE_BOOST_LIB")
endif(CMAKE_BOOST_LIB)

if(CMAKE_WJELEMENT_INCLUDE)
    include_directories(${CMAKE_WJELEMENT_INCLUDE})
else(CMAKE_WJELEMENT_INCLUDE)
    message(FATAL_ERROR,"You have not  defined CMAKE_WJELEMENT_INCLUDE")
endif(CMAKE_WJELEMENT_INCLUDE)

if(CMAKE_WJELEMENT_LIB)
    link_directories(${CMAKE_WJELEMENT_LIB})
else(CMAKE_WJELEMENT_LIB)
    message(FATAL_ERROR,"You have not  defined CMAKE_WJELEMENT_LIB")
endif(CMAKE_WJELEMENT_LIB)

FIND_FILE(BOOST_HEADER "boost/regex.hpp" ${CMAKE_BOOST_INCLUDE})
if(BOOST_HEADER MATCHES BOOST_HEADER-NOTFOUND)
  message(SEND_ERROR "Can't find boost/regex.hpp in ${CMAKE_BOOST_INCLUDE}")
endif(BOOST_HEADER MATCHES BOOST_HEADER-NOTFOUND)

find_library(BOOST_LIBRARY
    NAMES boost_regex
    HINTS "${CMAKE_BOOST_LIB}"
)
if(BOOST_LIBRARY MATCHES BOOST_LIBRARY-NOTFOUND)
  message(SEND_ERROR "Can't find libboost_regex in ${CMAKE_BOOST_LIB}")
endif(BOOST_LIBRARY MATCHES BOOST_LIBRARY-NOTFOUND)

FIND_FILE(WJELEMENT_HEADER "wjelement.h" ${CMAKE_WJELEMENT_INCLUDE})
if(WJELEMENT_HEADER MATCHES WJELEMENT_HEADER-NOTFOUND)
  message(SEND_ERROR "Can't find wjelement.h in ${CMAKE_WJELEMENT_INCLUDE}")
endif(WJELEMENT_HEADER MATCHES WJELEMENT_HEADER-NOTFOUND)

find_library(WJELEMENT_LIBRARY
    NAMES wjelement
    HINTS "${CMAKE_WJELEMENT_LIB}"
)

if(WJELEMENT_LIBRARY MATCHES WJELEMENT_LIBRARY-NOTFOUND)
  message(SEND_ERROR "Can't find library wjelement in ${CMAKE_WJELEMENT_LIB}")
endif(WJELEMENT_LIBRARY MATCHES WJELEMENT_LIBRARY-NOTFOUND)

if (UNIX)
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test/unix/release)
endif ()

if (WIN32)
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test/windows/release)
endif ()

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)
