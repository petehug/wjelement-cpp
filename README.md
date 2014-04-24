Wjelement-cpp
=============

WJElement-cpp - Lightweight C++ wrapper for WJElement with JSON Schema support

WJElement++ provides a simple, lightweight wrapper around the excellent WJElement
library. The wrapper supports JSON Schema draft-04.

While WJElement is fast and memory-efficiency, this C++ wrapper makes it
far more convenient to use and and the code is easier to understand and 
manage.

However, the wrapper is only a by-product of the real reason why this
library sprung to life. I was after a JSON Schema library for C or C++
that would allow me to deal with things such as binding validators
with elements, identifying elements as schemas, parsing the meta schema
and building schema and document stores. At the time of forking WJElement, 
its built in support for schema only supported validation and was based on 
JSON Schema draft-03.

To use this library, you will need to build and link with the wjelement++
branch of wjelement:

 git clone -b wjelement++ https://github.com/petehug/wjelement.git
 
Further reading:
- Example: https://github.com/petehug/wjelement-cpp/wiki
- Documentation: http://www.d3.org.nz/wjelement-cpp/

Notes: 

- currently I'm building this under windows using VC9 but will add support for Linux shortly
- the library currently uses boost regex but is expected to alos use boost threads in the future
- you find a test program in the test folder and the VC9 project file under windows
  this test program will run all draft4 compliance tests but it does require these additional
	libraries: libcurl, libssh2 and zlib

Peter Hug <pete@kapiti.co.nz>  - author of Wjelement-cpp

Special thanks goes to Micah N Gorrell for his excellent wjelement library!
