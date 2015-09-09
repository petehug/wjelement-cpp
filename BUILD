Windows:
========
Please use the VC 9.0 vcproj files in .\windows and .\test\windows.


Unix:
========
This library is built on top of the C library WJElement. At the time of this writing, wjelement did not distinguish 
between number and integer JSON types. For this reason I created the new WJElement branch wjelement++ and added support 
for integer JSON types. WJElement-cpp requires that you build and link with this branch of WJElement and that when 
building WJElement the precompiler directive WJE_DISTINGUISH_INTEGER_TYPE is defined.

WJElement (wjelement++ branch is available from here: https://github.com/petehug/wjelement/tree/wjelement++)

Building WJElement-cpp requires c-make. On the command prompt, provide the following command from the root directory 
of wjelement-cpp and replace the "..." with the appropriate path:

Runing cmake
-------------
$cmake -DCMAKE_WJELEMENT_INCLUDE:FILEPATH="..."
       -DCMAKE_WJELEMENT_LIB:FILEPATH="..."
       -DCMAKE_BOOST_INCLUDE:FILEPATH="..."
       -DCMAKE_BOOST_LIB:FILEPATH="..."
       -DCMAKE_INSTALL_PREFIX:FILEPATH="..."
       CMAKE_VERBOSE_MAKEFILE=1

Notes:

CMAKE_WJELEMENT_INCLUDE: Must point to the path with the include files from the wjelement++ branch of WJElement
CMAKE_WJELEMENT_LIB: Must point to the path contaning the library files from the wjelement++ branch of WJElement
CMAKE_BOOST_INCLUDE: Must point to the path contaning the include files from the boost::regex library
CMAKE_BOOST_LIB: Must point to the path contaning the library files from the boost::regex library
CMAKE_INSTALL_PREFIX: (optional, if missing "/usr/local" is assumed) Specifies the output path. Header files will 
end up in CMAKE_INSTALL_PREFIX/include and generated library files in CMAKE_INSTALL_PREFIX/lib.

You can also specify the -i switch to cmake. This will prompt you interactively to either accept or override default
values.

Build/Install
-------------
$make clean
$make all
$make install

Test
-------------
To check if all is good, run the JSON Schema draft-04 tests:

$cd test
$./wjelement-test

=======
$cmake 
-DCMAKE_WJELEMENT_INCLUDE:FILEPATH="[path to wjelement header files]" 
-DCMAKE_WJELEMENT_LIB:FILEPATH="[path to wjelement library files]" 
-DCMAKE_BOOST_INCLUDE:FILEPATH="[path to boost header files]" 
-DCMAKE_BOOST_LIB:FILEPATH="[path to boost library files]"
-DCMAKE_INSTALL_PREFIX:FILEPATH="[pathe where wjelement-cpp will be installed]"
-DCMAKE_VERBOSE_MAKEFILE=1

$make clean

$make all

$make install

$cd test

$cd unix/release

$./wjelement-test

If any one of these cache variables (CMAKE_BOOST_INCLUDE,CMAKE_BOOST_LIB,CMAKE_WJELEMENT_INCLUDE,CMAKE_WJELEMENT_LIB)
are not defined, it will report an error for the same.

Build rules in CMakeLists.txt file will also check for header files for wjelement in the path CMAKE_WJELEMENT_INCLUDE
and check for header files for boost in the path CMAKE_BOOST_INCLUDE. If its not found in the given path, it will print
error message for the same.

Build rules in CMakeLists.txt file will also check for  wjelement library in the path CMAKE_WJELEMENT_LIB and also
check for boost library in the path CMAKE_BOOST_LIB. If its not found in the given path,it will print error message
for the same.

CMAKE_INSTALL_PREFIX:

It refers to the path where header and library files related to wjelement-cpp will be installed.If its not mentioned
on the command prompt,it will use either the old path mentioned in the previous build or uses default path (e.g. 
/usr/local). CMAKE_VERBOSE_MAKEFILE:User can set cache variable CMAKE_VERBOSE_MAKEFILE=1 to enable verbosity and
all the commands will be echoed to the console during the make.By Default,CMAKE_VERBOSE_MAKEFILE is disable/off.

=================================
Brief explanation about cmake -i:
=================================
User can also have an option to generate Makefile by using command cmake -i followed by zero or more cache variables
(as mentioned above) on the command prompt. The user is prompted to answer questions about the project configuration.
The answers are used to set cmake cache values into cmake cache variables. Based on answer provided, build will be 
configured accordingly. This command will use default cache values for cmake cache variables and prompt the user
whether he/she want to use default cache value for respective cache variable or user want to over-ride this value
by providing new cache value for respective cache variable. This will happen for each cache variable. One of the 
cache variable CMAKE_INSTALL_PREFIX will point to default cache value /usr/local.User will be prompted to use this
default value or provide a new cache value.
