/**
\mainpage wjelement-cpp documentation

WJElement-cpp provides a simple, lightweight wrapper around the excellent WJElement library. The wrapper supports JSON Schema draft-04.

While WJElement is fast and memory-efficient, this C++ wrapper simply adds a level of indirection which makes wjelement far more convenient to use and the code easier to understand and manage with virtually no impact on performance.

However, the wrapper is only a by-product of the real reason why this library sprung to life. I was after a JSON Schema library for C or C++ that would allow me to deal with things such as binding validators with elements, identifying elements as schemas, parsing the meta schema and building schema and document stores. At the time of forking WJElement, its built in support for schema only supported validation and was based on JSON Schema draft-03.

To use this library, you will need to build and link with the wjelement++ branch of wjelement:

git clone -b wjelement++ https://github.com/petehug/wjelement.git

\notes
- Build support is minimal. You'll find VC9 project files and very basic unix Makefiles.
- The library currently uses boost regex but is expected to also use boost threads in the future.
- You'll find a test program in the test folder and the VC9 project file under windows, This test program will run all draft4 compliance tests.

Peter Hug peter.hug@warehouseoptimization.com - author of Wjelement-cpp

Special thanks goes to Micah N Gorrell for his excellent wjelement library!
*/
#ifndef _wjelementcpp_h_
#define _wjelementcpp_h_

//============================= I M P O R T A N T =======================================
/*
	wjelement++ is built on top of the C library WJElement. At the time of this writing,
	wjelement did not distinguish between number and integer JSON types, but this is vital
	for proper JSON Schema Draft 04 implementation support.
	For this reason I created the new WJElement branch wjelement++ and added support for
	integer JSON types. WJElement-cpp requires that you build and link with this branch
	of WJElement and that when building WJElement the precompiler directive
	WJE_DISTINGUISH_INTEGER_TYPE is defined.

	You can get my branch of the wjelement library from here:

		https://github.com/petehug/wjelement

	The included makfiles ensure the library is built correctly for use with wjelement++.
*/
#define WJE_DISTINGUISH_INTEGER_TYPE

/* Link statically with WJElement */
#define COMPILE_AS_STATIC

//=======================================================================================

#include <wjelement.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <list>
#include <boost/regex.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace WJPP
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                                    //
	//																				JSON Schema Stuff																						//
	//                                                                                                    //
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	// some simple forward decls
	class URI;
	typedef URI*			URIPtr;

	// some simple forward decls
	class Node;
	class Cache;
	typedef Cache*														CachePtr;

	struct Node_cmp
	{
		bool operator() (const Node& lhs, const Node& rhs) const;
	};

	typedef std::list<WJElement>							WJEList;
	typedef WJEList::iterator									WJEListItr;

	typedef std::set<Node, Node_cmp>					NodeSet;
	typedef NodeSet::iterator									NodeSetItr;

	typedef std::list<Node>										NodeList;
	typedef NodeList::iterator								NodeListItr;

	typedef std::vector<Node>									NodeVect;
	typedef NodeVect::iterator								NodeVectItr;
	typedef NodeVect::size_type								NodeVectST;

	typedef std::map<std::string, Node>				NodeMap;
	typedef NodeMap::iterator									NodeMapItr;
	typedef NodeMap*													NodeMapPtr;

	typedef std::vector<std::string>					StringVect;
	typedef StringVect::iterator							StringVectItr;
	typedef StringVect::size_type							StringVectST;


	/*******************************************************************************************************
		URI Class
	*******************************************************************************************************/

	//! URI class can be used to store full or paretially defined URI's based on other URI's (protocol, host, path, query-string(s) and JsonPointer(s))
	/*! The URI class works just like page internal URL's behave in a web browser.
	
			The page itself has a URL which is considered the base URL. Other URL's can be fully qualified (i.e. not based on the pages URL)
			or relative to the pages URL:

			Page URL: http://xyz.com/here/is/my/page1

			embedded URL: page2                              ==> http://xyz.com/here/is/my/page2
			embedded URL: ../your/page1                      ==> http://xyz.com/here/is/your/page1
			embedded URL: /your/page1                        ==> http://xyz.com/your/page1
			embedded URL: ../..//your/./page1                ==> http://xyz.com/here/your/page1
			embedded URL: xyz.com/blah                       ==> http://xyz.com/blah

			etc.

			Usage:
			======
			- pass the base URI and the URL to the constructor (base URI can be NULL)
			- if you need to, you can change the URI with a call to setURI
			- call getURI to get the relative URI
			- call getFullURI to get the fully qualified URI
	*/
	class URI
	{
		friend class Node;

	protected:
		URIPtr				m_pBaseURI;					//!< The base URI - will be used to resolve full URI if this is a partial URI
		std::string		m_mode;							//!< The mode (aka "scheme") (e.g. http, ftp, ssh, https, etc.)
		std::string		m_host;							//!< The host (aka "authority") details (e.g. mysite.com, mysite.com:8080, user:pwd@mysite.com:8080, etc.)
		std::string		m_path;							//!< The path (e.g. foo, /foo, ../bar, foo/bar, etc). If the leading slash is omitted, this is a relative path and the full path borrows from the m_pBaseURI member.
		std::string		m_query;						//!< The query componet (e.g. ?id=1234&page=5&rowspage=28)
		std::string		m_jsonPointer;			//!< The fragment which is expected to hold a json  pointer (e.g. #, #/foo, #/foo/bar, etc.)
		StringVect		m_vectPath;					//!< The path segments in order
		StringVect		m_vectJsonPointer;	//!< The json pointer segments in order

		//! The json pointer decoder (see http://tools.ietf.org/search/rfc6901)
		static std::string	decodeJsonPointer(const std::string& strPtr);
		//! The json pointer encoder (see http://tools.ietf.org/search/rfc6901)
		static std::string	encodeJsonPointer(const std::string& strPtr);

	public:
		//! Basic constructor, just pass a URI as a string
		URI(const std::string& strURI)
			: m_pBaseURI(NULL)
		{
			setURI(strURI);
		}

		//! Use this if you have a hierarchy of URI's
		URI(URIPtr pBaseURI = NULL, const std::string& strURI = "")
			: m_pBaseURI(pBaseURI)
		{
			setURI(strURI);
		}

		//! Assign another URI
		void							setURI(const std::string& strURI);

		//! Get the URI (same as what was passed into constructor or setURI() method
		std::string				getURI();
		//! full URI but without JsonPointers (if this is a relative URI, it will borrow the missing details from member m_pBaseURI)
		std::string				getBaseURI();
		//! full URI (if this is a relative URI, it will borrow the missing details from member m_pBaseURI)
		std::string				getFullURI();

		//! Get the mode (aka "scheme") (e.g. http, ftp, ssh, https, etc.)
		std::string&			getMode();
		//! Get the host (aka "authority") details (e.g. mysite.com, mysite.com:8080, user:pwd@mysite.com:8080, etc.)
		std::string&			getHost();
		//! Get the path (e.g. foo, /foo, ../bar, foo/bar, etc). If the leading slash is omitted, this is a relative path and the full path borrows from the m_pBaseURI member.
		std::string				getPath();
		//! Get the query componet (e.g. ?id=1234&page=5&rowspage=28)
		std::string&			getQuery();
		//! Get the fragment which is expected to hold a json  pointer (e.g. #, #/foo, #/foo/bar, etc.)
		std::string				getJsonPointer();

		//! Get the path segments in order
		StringVect				getPathVect();
		//! Get the json pointer segments in order
		StringVect				getJsonPointerVect();

		//! Returns true if this has a JSON pointer and false otherwise
		bool							hasJsonPointer()		{ return !m_vectJsonPointer.empty(); }

		//! returns true if mode, host, path, query amd pointer all are empty
		bool							empty();
	};






	/*******************************************************************************************************
		JSON Node class
	*******************************************************************************************************/

	//! Lightweight WJElement wrapper
	class Node
	{
		friend class	Cache;
		friend class	ManagedNode;
		friend struct Node_cmp;
		friend std::ostream & operator<<(std::ostream & strmOut, Node & node);

	protected:
		//! The ONLY data member is a WJElement (which is a pointer)
		WJElement								_e;

		/*******************************************************************************************************
			JSON SchemaInfo Class
		*******************************************************************************************************/

		//! The SchemaInfo is another super light WJElement wrapper class that provides access to schema details of a WJElement
		/*! The JSON Schema framework stores detailes in WJElement's void* client members. There are two
				different types of info:

				- if the WJElement is a schema, some details about the schema
				- if the WJElement has been validated by one or more schemas, it will hold a list of all these schemas

		*/
		class SchemaInfo
		{
		protected:
			//! If the node is a schema, this structure will hold its details
			struct Schema
			{
				//! The Cache this belongs to
				CachePtr								_cache;
				//! true if this has a $ref member
				bool										_isRef;
				//! inlined meta schema id's (only if this is a root schema)
				NodeMapPtr							_mapInlined;
				//! The URI (if this is a sub-schema, the URI has as base URI the URI of the parent schema)
				URI											_uri;

				Schema(CachePtr cache, URIPtr pURI) : _cache(cache), _isRef(false), _mapInlined(NULL), _uri(pURI) {};
				Schema(CachePtr cache, const std::string & strURI) : _cache(cache), _isRef(false), _mapInlined(NULL), _uri(strURI) {};
				~Schema()
				{
					delete _mapInlined;
				}
			};

			//! If a WJElement has data in void* client, it will be a pointer to such an Info structure
			struct Info
			{
				//! Schema details
				Schema*									_s;
				//! List of validators that apply to the WJElement
				WJEList*								_v;

				//! Default ctor
				Info() : _s(NULL), _v(NULL) {}
				//! dtor deletes members
				~Info()
				{
					delete _s;
					delete _v;
				}
			};

			//! The WJElement this wrappes
			WJElement	_e;

			//! Returns the info structure (or NULL if the WJElement has none)
			Info*										_info()												{ return _e && _e->client ? (Info*) _e->client : NULL; }
			//! creates a new Info structure
			Info*										_createInfo();
			//! creates a new list to store validators
			WJEList*								_createValidators();

			//! Returns the schema details (or NULL if the WJElement has none)
			Schema*									_schema()											{ Info* i = _info(); return i ? i->_s : NULL; }

		public:
			//! Creates a SchemaInfo object and attaches it to the WJElement passed in
			SchemaInfo(WJElement	wje = NULL) : _e(wje) {}

			//! Returns a pointer to a list of validators which apply to the WJElement (or NULL if this has none)
			WJEList*								validators()									{ Info* i = _info(); return i ? i->_v : NULL; }

			//! Returns the Cache this belongs to (or NULL if the WJElement isn't a schema)
			CachePtr								cache()												{ Schema* s = _schema(); return s ? s->_cache : NULL; }
			//! Returns true if the WJElement is a schema and has a member "$ref"
			bool										isRef()												{ Schema* s = _schema(); return s ? s->_isRef : false; }
			//! If the WJElement is a root schema and it contains inlined schema, these will be accessible by URI from this store (returns NULL if the WJElement isn't a root schema or hasn't got inlined schemas)
			NodeMapPtr							mapInlined()									{ Schema* s = _schema(); return s ? s->_mapInlined : NULL; }
			//! Returns the URI (or NULL if the WJElement isn't a schema)
			URI*										uri()													{ Schema* s = _schema(); return s ? &(s->_uri) : NULL; }

			//! Returns true if the WJElement is a schema and has a $ref member
			void										isRef(bool b)									{ Schema* s = _schema(); if (s) s->_isRef=b; }

			//! Removes the node from the map of inlined schemas (only applies if WJElement is a root schema)
			void										unregInlineSchema(Node& node);
			//! Adds the node to the map of inlined schemas (only applies if WJElement is a root schema)
			void										regInlineSchema(Node& node, const std::string& strJsnPtr);
			//! Given a URI, looks up the subschema with the URI specified.
			Node										getInlineSchema(const std::string& strJsnPtr);

			//! Returns true if the WJElement belongs to a schema
			bool										hasSchemaInfo()								{ return _schema() != NULL; }
			//! Returns true if the WJElement has validators
			bool										hasValidators()								{ return validators() != NULL; }

			//! Adds a schema to the WJElements list of validators (typically happens after a validator successfully validated a WJElement)
			void										addValidatorSchema(Node& node);
			//! Removes a schema from the WJElements list of validators
			void										removeValidatorSchema(Node& node);

			//! Creates schema details for the WJElelement
			void										initSchema(CachePtr pCache, URIPtr pURI);
			//! Creates schema details for the WJElelement
			void										initSchema(CachePtr pCache, const std::string& strURI);

			//! returns false if this has no WJElement or the WJElement has no client data
															operator bool()								{ return _e && _e->client;	}
			//! allows you to assign one SchemaInfo object to another (does not copy anything, it just means the lhs object will now look at the same WJElement as the rhs object)
			SchemaInfo&							operator=(SchemaInfo& rhs)		{ _e = rhs._e; return *this;	}
			//! allows you to assign a Node to a SchemaInfo (this simply means the SchemaInfo will now apply to the Node's WJElement)
			SchemaInfo&							operator=(Node& rhs)					{ _e = *rhs; return *this;	}
			//! allows you to assign a WJElement to a SchemaInfo (this simply means the SchemaInfo will now apply to the passed in WJElement)
			SchemaInfo&							operator=(WJElement e)				{ _e = e;	return *this;	}

			//! This method is typically called by Node::discard so refer to this method for details
			void										discard();

			//! a helper to discard
			void										discardValidators();
		};


		//! The address of the node in the form of a JSON Pointer
		std::string							_getName(bool zeroIndex = false);

		//! The address of the node in the form of a JSON Pointer
		std::string							_asJsonPointer(std::string& str, bool zeroIndex = false);

		//! Deletes the list of validators this may hold, but does not delete tha validators themselves
		void										_discardValidators();

		//! Helper for dump() (if indent is negative, no indentation
		std::ostream &					_dump(std::ostream & os, int indent = 1);

		//! Loads the meta schema (the meta schema draft04 is currently embedded in the code)
		static Node							_loadMetaSchema(CachePtr pCache);
		//! Recursive meta schema loader helper
		void										_validateMetaSchemaMember(CachePtr pCache);

		//! Creates the specia EMPTY schema which can be used to validate anything
		static Node							_createEmptySchema(CachePtr pCache);

		//! Adds an error object to the errors array
		void										_validationError(Node& schema, Node& node, const std::string& strMessage);

		//! Validation helper that assumes this is a schema and checks if node is of the appropriate data type
		bool										_validateType(Node& node, Node& errors, bool log = true);
		//! _validateType helper
		bool										_validateSingleType(Node& node, const std::string& type);

		//! _validateInstance assumes this is a schema and validates node
		bool										_validateInstance(Node& node, Node& errors, bool log = true);
		//! helper to _validateInstance that checks if a numeric node matches the set criteria
		bool										_validateNumber(Node& node, Node& errors, bool log = true);
		//! helper to _validateInstance that checks if a string node matches the set criteria
		bool										_validateString(Node& node, Node& errors, bool log = true);
		//! helper to _validateInstance that checks if an object node matches the set criteria
		bool										_validateObject(Node& node, Node& errors, bool log = true);
		//! helper to _validateInstance that checks if an array node matches the set criteria
		bool										_validateArray(Node& node, Node& errors, bool log = true);

		//! method checks if this has a $ref member and if so loads the referenced schema anbd passes the message on to it, otherwise returns this
		Node										_resolveRef(NodeVect& refs, Node& errors);
		//! Main interface to _resolveRef(NodeVect& refs)
		Node										_resolveRef(Node& errors);

		//! Sent if this is an inlined schema so that it can register itself with the root schemas map fo inlined schemas
		void										_registerInlineSchema();

		//! This method searches the directory of inlined schema root nodes maintain and returns the inlined schema if it exists
		/*! \note This method also accepts a json pointer URI (e.g. #/properties/person/properties/address, #/definitions/address, etc.) and
				if no entry exists in the dictionary with that name, the method will travers the path to reach the schema element requested.
		*/
		Node										_getInlinedSchema(URI& uri);

		//! Send this message to any node in a schema to get the root node of the schema
		Node										_getRootSchema();

		//! This helper adds the extra elements to the underlying WJElement to store root schema node info
		bool										_markRootAsSchema(CachePtr pCache, const std::string& strURI = "");
		//! This helper adds the extra elements to the underlying WJElement to store schema node info
		bool										_markAsSchema();

		//! This helper adds a new child element to this' WJElement
		/*! This method only works for nodes where isObject() or isArray() returns true.
				If this isObject()==true, there must be no child with the specified name already.
				If this isArray()==true, the name can be an empty string.
		*/
		WJElement								_addChild(const std::string& name);

	public:
    //! Create a node from a WJElement
    Node() : _e(NULL) {}
		//! Create a node from a WJElement
		Node(WJElement e) : _e(e) {}
		//! copy c'tor
		Node(const Node& j) : _e(j._e) {}

		//@{ support to access children of container Nodes

		//! use the iterator class to traverse children
		/*! \note while using operator[](unsigned int) works with Node where isContainer() == true, it is a sequential lookup and not cheap
		*/
		class iterator
		{
			friend class Node;

		protected:
			//! iterator delegates work through _e->next ==> _e->next ==> _e->next ...
			WJElement _e;

		public:
			//! Default ctor is at end
			iterator(WJElement e = NULL) : _e(e) {};

			//! Node accessor
			Node				operator*()							{ return Node(_e); }
			//! Postfix increment operator
			iterator		operator++(int)					{ if (_e) _e = _e->next; return *this; }
			//! Equal-to comparison operator
			bool				operator==(iterator r)	{ return _e == r._e; }
			//! Not equal-to comparison operator
			bool				operator!=(iterator r)	{ return _e != r._e; }
			//! Assignment operator
			iterator		operator=(iterator rhs)	{ _e = rhs._e; return *this; }
		};

		//! Returns true if this node has no children
		bool										empty()												{ return !_e || !_e->child; }
		//! Returns the number of children this has
		unsigned int						size()												{ return _e ? _e->count : 0; }
		//! returns an iterator that points to the Nodes first child (if there are no children begin()==end() is true)
		iterator								begin()												{ return iterator(_e ? _e->child : NULL); }
		//! returns an iterator that points passed the last element
		iterator								end()													{ return iterator(); }

		//! Access a child (if isContainer() == false or the element can't be found, the returned elements operator!() will return true)
		/*! Please beware that this methond will not work like WJElements selectors. Instead, it will only go one level deep at the time.

				So while in WJElement you might do:
					WJElement three = WJGet(current, "one.two.three", last);

				With nodes you'd do:
					Node three = current["one"]["two"]["three"];
					if (three)
					{
						// work with three
					}

					Here, if current["one"] didn't exists, you could still do current["one"]["two"] though this would also return a NULL Node.

				\note Passing in a string with only numeric characters can be used (as a not so eficient way) to retieve array elements.
				This makes it easy to navigate the JSON structure using JSON pointers.
		*/
		Node										operator[](const char* name);
		//! Access a child of an array (if isContainer() == false or the element can't be found, the returned elements operator!() will return true)
		Node										operator[](const std::string& name) { return operator[](name.c_str()); }

		//! Access a child by index (preconditions for success: isContainer()==true && i < size())
		/*! \note this works for both arrays and objects, but is not particularly meaningful for objects as it simply gives you the i'th property
		    \note it is recommended you use the iterator for iterating over childern instead of this method
		*/
		Node										operator[](unsigned int i);
		//! Access a child by index (preconditions for success: isContainer()==true && i < size())
		Node										operator[](int i)							{ return operator[]((unsigned int) i); }
		//@}

		//! Expects json to be a string containing JSON and returns a Node pointing to the root
		/*! When you're finished working with the parsed data, you should send the returned Node a discard() message to free the resources it uses.

				\note At the time of this writing, support for handling syntactic errors in JSON streams is really non-existent in wjelement. This
				means that you will almost always receive a valid Node even if you send crap data. Once error handling works in WJElement, this method
				is expected to throw a runtime_error
		*/
		static Node							parseJson(const std::string & json);

		//! return the nodes type (NULL nodes return WJR_TYPE_UNKNOWN)
		WJRType									getType() const								{ return _e ? _e->type : WJR_TYPE_UNKNOWN; }
		//! returns the type as a string as would be used in a JSON schema
		std::string							getTypeAsString();
		//! returns the nodes name
		std::string							getName()											{ return _getName(); }
		//! set the nodes name
		void										setName(const std::string& n);

		//! returns a std::string vector containing all the names of members this has
		StringVect							getMembers();
		//! returns true if this is a container that has an immediate child with the specified name
		bool										isMember(const std::string& name);

		//! returns true if this Node belongs to the one and only metaSchema
		bool										isMetaSchema();
		//! returns true if this Node belongs to a JSON Schema
		bool										isSchema()										{ return SchemaInfo(_e).hasSchemaInfo(); }
		//! returns true if this has no parent node
		bool										isRoot()											{ return _e && !_e->parent; }
		//! returns true if this Node belongs to a JSON Schema and has no parent
		bool										isRootSchema()								{ return isRoot() && isSchema(); }

		//! returns true if this is NULL
		bool										isNull()											{ return !_e || _e->type == WJR_TYPE_NULL; }
		//! returns true if this is an array or object
		bool										isContainer()									{ return _e && (_e->type == WJR_TYPE_OBJECT || _e->type == WJR_TYPE_ARRAY); }
		//! returns true if this is an object
		bool										isObject()										{ return _e && _e->type == WJR_TYPE_OBJECT; }
		//! returns true if this is an array
		bool										isArray()											{ return _e && _e->type == WJR_TYPE_ARRAY; }
		//! returns true if this is a bool
		bool										isBoolean()										{ return _e && (_e->type == WJR_TYPE_BOOL || _e->type == WJR_TYPE_TRUE || _e->type == WJR_TYPE_FALSE); }
		//! returns true if this is a number
		bool										isNumber()										{ return _e && _e->type == WJR_TYPE_NUMBER; }
		//! returns true if this is an integer
		bool										isInteger()										{ return _e && _e->type == WJR_TYPE_INTEGER; }
		//! returns true if this is an string
		bool										isString()										{ return _e && _e->type == WJR_TYPE_STRING; }

		//! returns the length of the ancestor chain, the root returns 0
		unsigned int						getDepth();
		//! given any node, get the top most ancestor
		Node										getRoot();
		//! returns the parent
		Node										getParent()										{ return _e && _e->parent ? Node(_e->parent) : Node(NULL); }


		//! removes this from it's parent so that it becomes a root
		void										detach();
		//! attaches the child to this (this must be an array or object and neither this nor child must null)
		void										attach(Node& child);
		//! attaches the child to this (this must be an array or object and neither this nor child must null)
		void										attach(Node& child, const std::string& name)	{ child.setName(name); attach(child); }

		//! returns a JSON pointer to this (if zeroIndex is true, every index will be expressed as * rather than the true index)
		std::string							asJsonPointer(bool zeroIndex = false)		{ std::string str; return _asJsonPointer(str, zeroIndex); }

		//! Compares this with rhs
		/*!
				returns -1 if this is smaller than rhs, 0 if this and rhs are equal and 1 if this is less than rhs.

				A node without WJElement is less than a node with a WJElement.

				If the nodes are of different type, the method always returns -1.
		*/
		int											compare(Node& rhs);

		/** @name Comparison operators
		*/
		//@{
		bool										operator==(Node& rhs)		{ return compare(rhs) == 0; }
		bool										operator!=(Node& rhs)		{ return compare(rhs) != 0; }
		bool										operator> (Node& rhs)		{ return compare(rhs) == 1; }
		bool										operator< (Node& rhs)		{ return compare(rhs) == -1; }
		bool										operator>=(Node& rhs)		{ return compare(rhs) != -1; }
		bool										operator<=(Node& rhs)		{ return compare(rhs) != 1; }
		//@}

		//! If this operator returns true, you're dealing with a NULL node
		/*! This operator is extremely useful to check success of method which return a node but could fail:

				Node node = Node::parseJson("[fail,1,2,3]");

				if (!node)
				{
					// parsing failed
				}

				if (node)
				{
					// parsing succeeded
				}
		*/
		bool										operator!()										{ return _e == NULL; }
		//! Allow a Node obejct to be used wherever a WJElement is expected
		WJElement								operator*() const							{ return _e; }
		//! Allow a Node obejct to be used wherever a WJElement is expected
														operator WJElement()					{ return _e; }
/*
		// unfortunately, these create conflicts with other operators

														operator bool()								{ return getBool(); }
														operator double()							{ return getNum(); }
														operator int()								{ return getInt(); }
														operator std::string()				{ return getString(); }
*/

		//! pretty print this to the stream
		/*! @param os The output stream where this should be written to
				@param indent The output will be indented by indent * 2 spaces

				The message is recursively passed to children each time incrementing indent by 1
		*/
		std::ostream &					dump(std::ostream & os, int indent = 0);

		//! to_stream outputs the value of this to the stream
		/*! @param os The output stream where this should be written to

				\note This method writes just values, no labels or JSON punctuation.
				If this is an array or an object, it will write [array] or [object].
		*/
		std::ostream &					to_stream(std::ostream & os);

		//! deletes this' and all its depended WJElements (operator!() will return true after this call;
		/*! \note This method deletes all memory resources used by this and and all its dependents
				The method should be called on root Nodes (it automatically discards all direct and indirect
				offspring)
				If you do not call this method, your code will leak memory.
		*/
		void										discard();

		//! This must be a schema (isSchema()==true), it validates node against itself
		/*! If you send this message to the metaSchema (see also Cache::getMetaSchema()), node is expected to be
				a schema otherwise it is expected to be an instance of the schema.
				The validator writes validation errors to error. If error is a NULL node, it will be created as an
				array and each error found will be written to the error array as an error object containing a schema
				property (json pointer into the schema), instance property (json pointer into the instance) and
				an error message.
				When validating schemas, providing the correct URI will guarantee proper $ref resolution.
				\note The callee is responsible to send the error parameter a discard message.
		*/
		bool										validateInstance(Node& node, Node& error, const std::string& strURI = "");

		//! create a root object
		/*! \note At the end of the returned nodes lifecycle, send a discard() message to free resources
		*/
		static Node							createObject();
		//! create a root array
		/*! \note At the end of the returned nodes lifecycle, send a discard() message to free resources
		*/
		static Node							createArray();

		//@{ add JSON value variants
		/*! @param name The name of the new node (specify an empty string for array elements)
				@param value The value to assign

				This message must only be sent to a Node where isContainer()==true.
				Pass an empty string for name if the container is an array.
				If this is an array, the new node will be appended to the end of the collection.
				If this is an object an a member with the name specified already exists, the method will
				throw a runtime_error.
		*/
		Node										addObject(const std::string& name);
		Node										addArray(const std::string& name);
		Node										addNull(const std::string& name);
		Node										addString(const std::string& name, const std::string& value);
		Node										addBool(const std::string& name, bool value);
		Node										addInt(const std::string& name, int value)						{ return addInt32(name, (int32) value); }
		Node										addInt32(const std::string& name, int32 value);					
		Node										addUInt32(const std::string& name, uint32 value);
		Node										addInt64(const std::string& name, int64 value);
		Node										addUInt64(const std::string& name, uint64 value);
		Node										addDouble(const std::string& name, double value);
		//@}

		//@{ get this' value
		/*! This methods will raise a runtime_error if the Node's type doesn't match the
				requested type. So this code will raise an exception:

				\code
				if (!node.isString())
					node.getString();
				\endcode
		*/
		std::string							getJSONEncodedString();
		std::string							getString();
		char *									getChar();
		bool										getBool();
		int											getInt()																				{ return (int) getInt32(); }
		int32										getInt32();
		uint32									getUInt32();
		int64										getInt64();
		uint64									getUInt64();
		double									getNum();
		//@}

		//@{ set this' value
		/*!
				You can use these methods to change the type of a node. Lets assume node.isString() returns true:

				\code
				cout << (node.isString() ? "a string" : "not a string") << endl;
				node.setBool(false);
				cout << (node.isString() ? "a string" : "not a string") << endl;
				cout << (node.isBool() ? "a bool" : "not a bool") << endl;
				\endcode

				This should output:

				\code
				a string
				not a string
				a bool
				\endcode

				These methods will raise a runtime_error if the Node is NULL.
		*/
		void										setNull();
		void										setString(const std::string& value);
		void										setBool(bool value);
		void										setInt(int value)																{ return setInt32((int32) value); }
		void										setInt32(int32 value);
		void										setUInt32(uint32 value);
		void										setInt64(int64 value);
		void										setUInt64(uint64 value);
		void										setDouble(double value);
		//@}

	};


	//! Stream a Node to an std::ostream
	inline std::ostream & operator<<(std::ostream & strmOut, Node & node)
	{
		return node._dump(strmOut, -1);
	}





	/*******************************************************************************************************
		JSON Schema Cache Class
	*******************************************************************************************************/

	//! See Cache::loadSchema for an explanation/requirement of function of this type
	typedef std::string (*SchemaLoaderFunc)(const std::string& strURI);

	//! The schema cache singleton class stores schema info globally
	class Cache
	{
	protected:
		//! This sigleton class is used to generate unique schema id's when they are missing
		/*!
				This class basically builds ID's like 'http://localhost/schema/xyz'

				where xyz is an ever increasing identifier going from starting from aaaa to zzzz.
		*/
		class IDGenerator
		{
		protected:
			std::string			m_strID;
			IDGenerator() : m_strID("http://localhost/schema/aaaa") {};

		public:
			static std::string		getID();

		};

		friend class Node;
		friend class Cache::IDGenerator;

		static boost::recursive_mutex	M_mtx;

		NodeMap									m_mapSchema;
		Node										m_metaSchema;
		Node										m_emptySchema;
		NodeSet									m_set;

		void							_onNodeCreated(Node& schema)		{ m_set.insert(schema); }
		void							_onNodeDeleted(Node& schema)		{ m_set.erase(m_set.find(schema)); }

		void							_onSchemaCreated(Node& schema, URIPtr pURI);
		void							_onSchemaDeleted(Node& schema, URIPtr pURI);

		Cache()																						{ _initialize(); }
		~Cache();

		void							_initialize();

	public:

		//! Singleton accessor
		/*! 
				\note It is HIGHLY recommended that the first time you call this method you provide a fnLoader.
				If you call loadSchema(strURI) and the schema doesn't exist, the Cache will delegate the loading
				of the resource to fnLoader. If no loader is specified, only schemas already cached can be
				accessed.
				Beware that fnLoader values provided in subsequent calls to GetCache are ignored.
		*/
		static Cache&			GetCache();

		//! Returns the Node that specifies the schema requested in the URI
		/*! @param strURI		The URI to the schema resource.

				@param errors		The node that, if errors are encountered during parsing of the schema, 
												will be populated with errors.

				@param fnLoader A function that resolves the URI. It simply takes a string as input
												parameter and returns an std::string that contains the body of the 
												resource. The method can either throw an std::runtime_error exception
												with details of an error or an empty string (which will be treated as
												"resource not found" error). If no loader function is supplied and
												the resource is not already in the cache, the method call will result
												in a "resource not found" error.

				The method first looks in its own cache and returns the schema if it already exists. If
				the schema is not found, the method calls fnLoader (if provided).
		*/
		Node							loadSchema(const std::string& strURI, Node& errors, SchemaLoaderFunc fnLoader = NULL);

		//! Validate a Draft-04 JSON schema and binds its nodes to the Draft-04 meta schema definition
		/*!	The method returns true if the schema validates successfully. If it returns false, the errors
				parameter contains the details of the errors.

				Please note that since there is no URI, a temporary URI is generated so that the schema
				can be stored in the cache. You can access the URI if the call is successfull via 
				/code
					SchemaInfo(schema).uri();
				/endcode
		*/
		bool							loadSchema(Node& schema, Node& errors);

		Node							getMetaSchema()							{ return m_metaSchema; }
		Node							getEmptySchema()						{ return m_emptySchema; }

		std::string				getUniqueID()								{ return IDGenerator::getID(); }

	};


	//! compare one node with another
	inline bool Node_cmp::operator()(const Node& lhs, const Node& rhs) const
	{
		return *lhs < *rhs;
	}


	//! Managed nodes are discarded when the object is distroyed
	/*! This class behaves just like a Node. The only difference is that when the object is deleted,
			the underlying element is also deleted.

			Consider this code:

			\code
			Node myNode;
			{
				Node node = Node::parseJson("{\"value\":1}");
				myNode = node;
			}
			myNode.dump(cout); // no problem
			\endcode
			The above code works fine because when node gets destroyed on block exit, the WJElement attached
			to it is not discarded. To discard it you'd have to call discard() explicitly.

			\code
			Node myNode;
			{
				ManagedNode node = Node::parseJson("{\"value\":1}");
				myNode = node;
			}
			myNode.dump(cout); // Undefined behaviour
			\endcode
			The above codes behaviour is undefined because when node gets destroyed on block exit it deletes
			the attached WJElement.

		*/
	class ManagedNode : public Node
	{
		friend std::ostream & operator<<(std::ostream & strmOut, ManagedNode & node);

	public:
    //! Create a node from a WJElement
    ManagedNode() {}
		//! Create a node from a WJElement
		ManagedNode(WJElement e) : Node(e) {}
		//! copy c'tor
		ManagedNode(const Node& j) : Node(j) {}

		//! dtor
		~ManagedNode() { discard(); }

		//@{ valid assignemnts
		ManagedNode&	operator=(Node& n)				{ if (_e) discard(); _e = n._e; return *this; }
		ManagedNode&	operator=(WJElement e)		{ if (_e) discard(); _e = e; return *this; }
		//@} assignemnt

		//! Illegal assignment - will throw runtime_error if used
		/*!
				If you need to assign a managedNode to another, you can use this technique:

				/code
				{
					ManagedNode a, b;

					a = Node::parseJson("{\"value\":1}");
					b = a.releaseNode();

					cout << (a ? "a is valid" : "a is a NULL node") << endl;		// prints "a is a NULL node"
					cout << (b ? "b is valid" : "b is a NULL node") << endl;		// prints "b is valid"
				}
				/endcode
		*/
		ManagedNode&	operator=(ManagedNode& n) { throw std::runtime_error("assignement of one ManagedNode to another is illegal"); }

		//! Releases the Node
		/*!
			If you decide you need to keep the underlying Node past the life cycle of the ManagedNode,
			send the releaseNode message and discard() the returned node manually.

			This ManagedNode will be a NULL node on return.
		*/
		Node					releaseNode()							{ Node node(_e); _e = NULL; return node; }
	};


	//! Stream a Node to an std::ostream
	inline std::ostream & operator<<(std::ostream & strmOut, WJPP::ManagedNode & node)
	{
		return node._dump(strmOut, -1);
	}

} /* namespace WJPP */



#endif /* _wjelementcpp_h_ */
