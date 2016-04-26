cmake_minimum_required(VERSION 2.8.9)

set(PROJECTNAME force-cop-estimator)
project(${PROJECTNAME})

set(CMAKE_INSTALL_PREFIX $ENV{ICUBcontrib_DIR})
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
add_definitions(${YARP_DEFINES}) # add yarp definitions - this contains also -D_REENTRANT
include_directories(${YARP_INCLUDE_DIRS})
include(YarpInstallationHelpers)
include(YarpIDL)


find_package(Gurls REQUIRED)
include_directories(${Gurls_INCLUDE_DIRS})

find_package(ICUBcontrib)
list(APPEND CMAKE_MODULE_PATH ${ICUBCONTRIB_MODULE_PATH})
include(ICUBcontribHelpers)
include(ICUBcontribOptions)
icubcontrib_set_default_prefix()
icubcontrib_add_uninstall_target()



subdirs(app)
subdirs(src)














	
