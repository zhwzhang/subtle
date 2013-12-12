cmake_minimum_required (VERSION 2.6)
project (libsubtle)
set(libsubtle_VERSION_MAJOR 1)
set(libsubtle_VERSION_MINOR 0)
#set(CMAKE_BUILD_TYPE release)
#set(CMAKE_BUILD_TYPE Debug)
list(APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
#set(MAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang++)

# add modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/lib/cmake")

#include coverage
INCLUDE(CodeCoverage)

set(BUILD_SHARED_LIBS TRUE)

# configure a header file to pass some of the CMake settings
# to the source code
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/)
configure_file (
  "${PROJECT_SOURCE_DIR}/libsubtle_config.h.in"
  "${PROJECT_BINARY_DIR}/libsubtle_config.h"
  )

include_directories("${PROJECT_BINARY_DIR}")

################################
# GTest and GMock
################################
enable_testing()
set(GMOCK_ROOT "lib/gmock-1.6.0/")
set(GTEST_ROOT "${GMOCK_ROOT}/gtest/")
find_package(GMock REQUIRED)

################################
# Tests
################################
# Add test file
file(GLOB TestFiles **/*_test.cc)

set(libsubtleSources src/subtle.cc src/hash.h src/rpc_impl.cc
    src/subfile.cc)

file(GLOB TagSources **/*cc **/*h)

set(SourceFiles ${libsubtleSources})

SETUP_TARGET_FOR_COVERAGE(cov runTests doc/coverage)
add_executable(runTests ${SourceFiles} ${TestFiles})
# Link test executable against gtest & gtest_main
add_test(runTests build/runTests)

#set doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                 ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ${DOXYGEN_EXECUTABLE}
                    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating API documentation..." VERBATIM
  )
endif(DOXYGEN_FOUND)

# add outputs
add_library(subtle src/subtle.cc ${SourceFiles})

# link libs
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})

target_link_libraries(subtle dl curl zip
  xmlrpc++ xmlrpc_client++ xmlrpc_util xmlrpc)

target_link_libraries(runTests pthread dl curl ssl
  xmlrpc++ xmlrpc_client++ xmlrpc_util xmlrpc
  ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES})

# example
add_executable (example src/example.cc src/subtle.cc ${SourceFiles})
target_link_libraries(example dl curl zip
  xmlrpc++ xmlrpc_client++ xmlrpc_util xmlrpc)

# ctags exuberant
add_custom_command (TARGET subtle POST_BUILD COMMAND
  ctags --sort=foldcase --c++-kinds=+p --fields=+iaS --extra=+q -f tags
  ${TagSources}
  COMMENT "Creating tags" VERBATIM)

# packaging
INSTALL(TARGETS subtle
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
INSTALL(FILES src/subtle.h DESTINATION include)
set(CPACK_OUTPUT_FILE_PREFIX ${PROJECT_SOURCE_DIR}/build)
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION_MAJOR ${libsubtle_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${libsubtle_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Goran Petrovic")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6), libgcc1 (>= 1:3.4.2-12), curl (>= 7.0), libxmlrpc-c++4 (>= 1.16), libzip2 (>= 0.10)")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Library for accessing OpenSubtitles.org's XMLRPC API to search, upload and download subtitles.")
INCLUDE(CPack)
