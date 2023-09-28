cmake_minimum_required(VERSION 3.7)

if(WIN32 AND DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

if (CMAKE_VERSION VERSION_LESS "3.8")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
else()
  set(CMAKE_CXX_STANDARD 14)
endif()


if (CMAKE_VERSION VERSION_LESS "3.9")
  project(Filter_gaussian
    VERSION 1.0.0
    LANGUAGES CXX
  )
else()
  project(Filter_gaussian
      VERSION 1.0.0
      LANGUAGES CXX
      DESCRIPTION "Filter_gaussian"
  )
endif()

################################################################################
# Libraries
################################################################################
################################################################################
# - QT
################################################################################
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Core Concurrent REQUIRED)

################################################################################
# - Boost
################################################################################
#find_package(Boost COMPONENTS system REQUIRED)

################################################################################
# Options
################################################################################
# option(WIDGETS_BUILD_TESTS "Build test tools" ON)

# ################################################################################
# # Test tools
# ################################################################################
# if(WIDGETS_BUILD_TESTS)
#   add_subdirectory(tests)
# endif()

# # ################################################################################
# # # Resources
# # ################################################################################
# set(RESOURCES_DIR resources)
# file(GLOB files ${RESOURCES_DIR}/*.*)

# foreach(item IN LISTS files)
#   get_filename_component(dest_filename ${item} NAME)

#   add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#                      COMMAND ${CMAKE_COMMAND} -E copy ${item}
#                              $<TARGET_FILE_DIR:${PROJECT_NAME}>/${dest_filename}
#   )
# endforeach()

################################################################################
# Sources
################################################################################
set (DIR_INCLUDE ${CMAKE_SOURCE_DIR}/include)
set (DIR_SRC ${CMAKE_SOURCE_DIR}/src)

file(GLOB_RECURSE include_h ${DIR_INCLUDE}/*.h)
file(GLOB_RECURSE src_h ${DIR_SRC}/*.h)
file(GLOB_RECURSE src_cpp ${DIR_SRC}/*.cpp)

################################################################################
# Executable
################################################################################
add_executable(${PROJECT_NAME} ${include_h} ${src_h} ${src_cpp})

target_include_directories(${PROJECT_NAME} PRIVATE
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${DIR_INCLUDE}
  # ${Boost_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} 
  PRIVATE
    Qt5::Widgets
    Qt5::Core
    Qt5::Concurrent
    # Qt5::SerialPort
    # ${Boost_LIBRARIES}
    # IDS::BXML
    # IDS::DisplayCtrl
    # IDS::Joystick
    # IDS::Widgets
)

# target_link_directories(${PROJECT_NAME}
#   PRIVATE
#       ${Boost_LIBRARY_DIRS}
# )

################################################################################
# Compilation flags
################################################################################
include (${CMAKE_SOURCE_DIR}/cmake/compilation_flags.cmake)

################################################################################
# Resources
################################################################################
# set(RESOURCES_DIR resources)
# file(GLOB files ${RESOURCES_DIR}/*.*)

# foreach(item IN LISTS files)
#   get_filename_component(dest_filename ${item} NAME)

#   add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#                      COMMAND ${CMAKE_COMMAND} -E copy ${item}
#                              $<TARGET_FILE_DIR:${PROJECT_NAME}>/${dest_filename}
#   )
# endforeach()