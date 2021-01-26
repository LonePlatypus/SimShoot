cmake_minimum_required(VERSION 3.0)
project(SimShoot)

find_package(OpenCV REQUIRED PATHS "C:/Users/Sigma/Documents/opencv450/install")

set(srcDir ../TargetShooter)
set(srcs 
${srcDir}/Hit.cpp
${srcDir}/TargetShooter.cpp
${srcDir}/settings.cpp
${srcDir}/Display.cpp
${srcDir}/game1.cpp
${srcDir}/Camera.cpp
)
set(headDir ../TargetShooter)
set(headers
${headDir}/Hit.h
${headDir}/cvui.h
${headDir}/settings.h
${headDir}/Display.h
${headDir}/game1.h
${headDir}/Camera.h
${headDir}/aruco_nano.h
)

add_definitions(${OpenCV_DEFINITIONS})
include_directories(${srcDir})
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(SimShoot ${srcs} ${headers})
target_link_libraries(SimShoot 
  ${OpenCV_LIBRARIES}
  )

file(COPY ${srcDir}/img 
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
