
find_package(Gurls REQUIRED)
include_directories(
${Gurls_INCLUDE_DIRS}
./include/)

set(HEADERS
include/forceCoPController.h
include/contactConditionEstimator.h
include/forceCoPEstimation.h
include/forceCoPEstimation_ANN.h
include/neuralNet.h
include/gp-force-cop-estimator.h)

set(SOURCES
src/main.cpp
src/forceCoPController.cpp
src/contactConditionEstimator.cpp
src/forceCoPEstimation.cpp
src/forceCoPEstimation_ANN.cpp
src/neuralNet.cpp
src/gp-force-cop-estimator.cpp
${HEADERS})

#set(CONTEXT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/context" CACHE PATH "Path to the context files")
set(CMAKE_BUILD_TYPE Debug CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE )



###################################################################
# Generate source from Apache Thrift Interface Definition Language
###################################################################
set(generated_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}")
yarp_idl_to_dir(force-cop-estimator-control.thrift ${generated_libs_dir} ${SOURCES} ${HEADERS} ${INCLUDE_DIRS})

include(force_cop_estimator_control_thrift.cmake)




add_executable(${PROJECTNAME} ${SOURCES})

target_link_libraries(${PROJECTNAME} ${YARP_LIBRARIES} ${Gurls_LIBRARIES})


if(WIN32)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin/${CMAKE_BUILD_TYPE})
else(WIN32)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
endif(WIN32)




