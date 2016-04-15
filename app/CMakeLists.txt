# Search for application files
file(GLOB APP_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/applications/*.xml)
# Search for ini files
file(GLOB APP_CONF
                ${CMAKE_CURRENT_SOURCE_DIR}/contexts/*.ini
                ${CMAKE_CURRENT_SOURCE_DIR}/contexts/*.xml)


yarp_install(FILES ${APP_SCRIPTS} DESTINATION ${ICUBCONTRIB_APPLICATIONS_INSTALL_DIR})
yarp_install(FILES ${APP_CONF} DESTINATION ${ICUBCONTRIB_CONTEXTS_INSTALL_DIR}/${PROJECTNAME})

