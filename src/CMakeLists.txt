cmake_minimum_required(VERSION 2.6)

add_library(wjelement++ SHARED
	wjelement++.cpp
)
target_link_libraries(wjelement++
	wjelement
	boost_regex
)
SET_TARGET_PROPERTIES(
	wjelement++
	PROPERTIES
	VERSION 1.0.0
	SOVERSION 1
	INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib 
)
install(TARGETS wjelement++ LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
add_library(wjelement++_sll STATIC
        wjelement++.cpp
)
target_link_libraries(wjelement++_sll
	wjelement_sll
        boost_regex
)
SET_TARGET_PROPERTIES(
        wjelement++_sll
        PROPERTIES
        VERSION 1.0.0
        SOVERSION 1
        INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib 
)
install(TARGETS wjelement++_sll ARCHIVE DESTINATION
${CMAKE_INSTALL_PREFIX}/lib)
