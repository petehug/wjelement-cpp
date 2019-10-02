WJElement-cpp
=============

WJElement-cpp - Lightweight C++ wrapper for WJElement with JSON Schema support

WJElement-cpp is based on top of the C library WJElement. The library is fast and efficient, but the C interface is neither easy to read nor to write. WJElement-cpp changes all that by implementing a WJPP::Node class which simply wraps a WJElement (a pointer to a WJElementPublic struct). You can pass a WJPP::Node object whereever a WJElement is expected and vs. They are interchangable.

WJElement-cpp also supports JSON Schema draft-04. The library maintains a schema cache which, on first use, loads the draft-04 meta schema. You can subsequently use the meta schema to load, validate and cache draft-04 schema. You can then use these schema to validate JSON documents.

One of the unique features of this library is that it ties schematic components to instance data. This means that a node will have access to the schema components that are used for its validation. 

Further reading:
- Example: https://github.com/petehug/wjelement-cpp/wiki
- Documentation: http://www.d3.org.nz/wjelement-cpp/

Dependencies: 
- WJElement (wjelement++ branch). This library requires that you use the wjelement++ branch of WJElement. You can get it from here: https://github.com/petehug/wjelement/tree/wjelement++. The library must be built by specifying the WJE_DISTINGUISH_INTEGER_TYPE compiler directive.
- boost.regex and boost.thread (I've been using 1.42.0). You can get the library from here: http://www.boost.org.

Notes:
- The original WJElement library does no strict JSON parsing. Instead of rejecting JSON that isn't valid, it simply omits incorrect "portions". 
  I changed this to simply return and NULL node but no message as to where the problem is :(. Hoping the WJElement group will fix this.
- The schema cache is now thread safe but other parts are not
- JSON Schema issues: 
-- I feel there are major issues in JSON Schema draft-04 spec WRT the schema's "id" propperty. It is likely that you face some issues with this library if you rely on "id".
-- The library does not honor "format", but this item is on the road-map
-- The library does not implement JSON Hyper-Schema

Peter Hug

Special thanks goes to Micah N Gorrell for his excellent wjelement library!
