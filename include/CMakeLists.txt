FILE(GLOB INCLUDES_HEADERS *.h )
LIST(REMOVE_ITEM INCLUDES_HEADERS ${PROJECT_SOURCE_DIR}/include/config.h)
INSTALL(FILES ${INCLUDES_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
