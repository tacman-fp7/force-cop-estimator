cmake_minimum_required(VERSION 2.8.9)

set(PROJECTNAME forceCoPEstimation)
project(${PROJECTNAME})

find_package(YARP REQUIRED)

include_directories(${YARP_INCLUDE_DIRS})


set(HEADERS src/forceCoPEstimation.h src/forceCoPEstimation_ANN.h src/neuralNet.h)

set(SOURCES  src/main.cpp src/forceCoPEstimation.cpp src/forceCoPEstimation_ANN.cpp src/neuralNet.cpp ${HEADERS})

set(CONTEXT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/context" CACHE PATH "Path to the context files")
set( CMAKE_BUILD_TYPE Debug CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo
MinSizeRel." FORCE )




add_executable(${PROJECTNAME} ${SOURCES})

target_link_libraries(${PROJECTNAME} ${YARP_LIBRARIES})

install(TARGETS ${PROJECTNAME} DESTINATION ./bin)


install(DIRECTORY ./contexts/
  DESTINATION ./contexts/${PROJECTNAME}/)










	
