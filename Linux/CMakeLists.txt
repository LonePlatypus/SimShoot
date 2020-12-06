cmake_minimum_required(VERSION 3.0)
project(SimShoot)

find_package(OpenCV REQUIRED)
find_package(X11 REQUIRED)

if(NOT UNIX)
  message(fatal_error "UNIX env required")
endif()

set(srcDir ../TargetShooter)
set(srcs 
${srcDir}/Hit.cpp
${srcDir}/TargetShooter.cpp
${srcDir}/settings.cpp
${srcDir}/Display.cpp
${srcDir}/game1.cpp
${srcDir}/Linux/screen.cpp
)

add_definitions(${OpenCV_DEFINITIONS})
include_directories(${srcDir})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${X11_INCLUDE_DIRS})

add_executable(SimShoot ${srcs})
target_link_libraries(SimShoot 
  ${OpenCV_LIBRARIES}
  ${X11_LIBRARIES}
  )

file(COPY ${srcDir}/img 
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
