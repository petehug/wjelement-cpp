project(wjelement-test)
cmake_minimum_required(VERSION 2.6)

if(CMAKE_BOOST_INCLUDE)
    include_directories(${CMAKE_BOOST_INCLUDE})
else(CMAKE_BOOST_INCLUDE)
    message(FATAL_ERROR,"You have not  defined CMAKE_BOOST_INCLUDE")
endif(CMAKE_BOOST_INCLUDE)

if(CMAKE_BOOST_LIB)
    link_directories(${CMAKE_BOOST_LIB})
else(CMAKE_BOOST_LIB)
    message(FATAL_ERROR,"You have not  defined CMAKE_BOOST_LIB")
endif(CMAKE_BOOST_LIB)

if(CMAKE_WJELEMENT_INCLUDE)
    include_directories(${CMAKE_WJELEMENT_INCLUDE})
else(CMAKE_WJELEMENT_INCLUDE)
    message(FATAL_ERROR,"You have not  defined CMAKE_WJELEMENT_INCLUDE")
endif(CMAKE_WJELEMENT_INCLUDE)

if(CMAKE_WJELEMENT_LIB)
    link_directories(${CMAKE_WJELEMENT_LIB})
else(CMAKE_WJELEMENT_LIB)
    message(FATAL_ERROR,"You have not  defined CMAKE_WJELEMENT_LIB")
endif(CMAKE_WJELEMENT_LIB)

add_executable(wjelement-test wjelement-test.cpp Exception.cpp)
target_link_libraries(wjelement-test
wjelement++
)
