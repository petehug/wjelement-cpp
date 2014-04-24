#BUILD INSTRUCTIONS FOR WJelement-cpp
$cmake -DCMAKE_WJELEMENT_INCLUDE:FILEPATH="<Mention wjelement include path,its the path where wjelement header files are installed>" -DCMAKE_WJELEMENT_LIB:FILEPATH="<Mention wjelement library path,its the path where wjelement library is installed>" -DCMAKE_BOOST_INCLUDE:FILEPATH="<Mention boost include path,its the path where boost header files are installed>" -DCMAKE_BOOST_LIB:FILEPATH="<Mention boost library path,its the path where boost library is installed>" -DCMAKE_INSTALL_PREFIX:FILEPATH="<Mention thepath where wjelement-cpp will be installed>" -DCMAKE_VERBOSE_MAKEFILE=1

$make clean

$make all

$make install

$cd test

$cd unix/release

$./wjelement-test

If any one of these cache variables (CMAKE_BOOST_INCLUDE,CMAKE_BOOST_LIB,CMAKE_WJELEMENT_INCLUDE,CMAKE_WJELEMENT_LIB) are not defined,it will report an
error for the same.

Build rules in CMakeLists.txt file will also check for header files for wjelement in the path CMAKE_WJELEMENT_INCLUDE and check for header files for boost in the path
CMAKE_BOOST_INCLUDE
If its not found in the given path,it will print error message for the same.

Build rules in CMakeLists.txt file will also check for  wjelement library in the path CMAKE_WJELEMENT_LIB and also check for boost library in the path
CMAKE_BOOST_LIB
If its not found in the given path,it will print error message for the same.

CMAKE_INSTALL_PREFIX:

It refers to the path where header and library files related to wjelement-cpp will be installed.If its not mentioned on the command prompt,it will use either the old path mentioned in the previous build or uses default path(e.g. /usr/local)
CMAKE_VERBOSE_MAKEFILE:User can set cache variable CMAKE_VERBOSE_MAKEFILE=1 to enable verbosity and all the commands will be echoed to the console during the make.By Default,CMAKE_VERBOSE_MAKEFILE is disable/off.

Note:"The pre-compiler directive WJE_DISTINGUISH_INTEGER_TYPE shall be mandatory defined for building wjelement++ library and test application. Irrespective whether user define pre-compiler directive WJE_DISTINGUISH_INTEGER_TYPE on the command line along with cmake or not ,wjelement++ build rules will ensure WJE_DISTINGUISH_INTEGER_TYPE is always define."

=================================
Brief explanation about cmake -i:
=================================
User can also have an option to generate Makefile by using command cmake -i followed by zero or more cache variables(as mentioned above) on the command prompt.The user is prompted to answer questions about the project configuration.The answers are used to set cmake cache values into cmake cache variables.Based on answer provided,build will be configured accordingly. This command will use default cache values for cmake cache variables and prompt the user whether he/she want to use default cache value for respective cache variable or user want to over-ride this value by providing new cache value for respective cache variable.This will happen for each cache variable. One of the cache variable CMAKE_INSTALL_PREFIX will point to default cache value /usr/local.User will be prompted to use this default value or provide a new cache value.
