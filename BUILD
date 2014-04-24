#BUILD INSTRUCTIONS FOR WJelement-cpp

Windows:
========
Please use the VC 9.0 vcproj files in .\windows and .\test\windows.


Unix:
========
This library is built on top of the C library WJElement. At the time of this writing, wjelement did not distinguish between number and integer JSON types. For this reason I created the new WJElement branch wjelement++ and added support for integer JSON types. WJElement-cpp requires that you build and link with this branch of WJElement and that when building WJElement the precompiler directive WJE_DISTINGUISH_INTEGER_TYPE is defined.

WJElement (wjelement++ branch is available from here: https://github.com/petehug/wjelement/tree/wjelement++)

Building WJElement-cpp requires c-make. On the command prompt, provide the following command from the root directory of wjelement-cpp and replace the "..." with the appropriate path:

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
CMAKE_INSTALL_PREFIX: (optional, if missing "/usr/local" is assumed) Specifies the output path. Header files will end up in CMAKE_INSTALL_PREFIX/include and generated library files in CMAKE_INSTALL_PREFIX/lib.

You can also specify the -i switch to cmake. This will prompt you interactively to either accept or override default values.

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

