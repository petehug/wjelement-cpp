#ifndef _wjelementcpp_h_
#define _wjelementcpp_h_

//#include "OutboundHttpRequest.h"
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
		URIPtr				m_pBaseURI;
		std::string		m_mode;
		std::string		m_host;
		std::string		m_path;
		std::string		m_query;
		std::string		m_jsonPointer;
		StringVect		m_vectPath;
		StringVect		m_vectJsonPointer;

		static std::string	decodeJsonPointer(const std::string& strPtr);
		static std::string	encodeJsonPointer(const std::string& strPtr);

	public:
		URI(const std::string& strURI)
			: m_pBaseURI(NULL)
		{
			setURI(strURI);
		}

		URI(URIPtr pBaseURI = NULL, const std::string& strURI = "")
			: m_pBaseURI(pBaseURI)
		{
			setURI(strURI);
		}

		void							setURI(const std::string& strURI);

		std::string				getURI();
		std::string				getBaseURI();	//!< full URI but without JsonPointers
		std::string				getFullURI();

		std::string&			getMode();
		std::string&			getHost();
		std::string				getPath();
		std::string&			getQuery();
		std::string				getJsonPointer();

		StringVect				getPathVect();
		StringVect				getJsonPointerVect();

		bool							hasJsonPointer()		{ return !m_vectJsonPointer.empty(); }

		// returns true if mode, host, path, query amd pointer all are empty
		bool							empty();
	};






	/*******************************************************************************************************
		JSON Node class
	*******************************************************************************************************/

	//! Lightweight WJElement wrapper
	class Node
	{
		friend class	Cache;
		friend struct Node_cmp;

	protected:
		//! The ONLY data member is a WJElement (which is a pointer)
		WJElement								_e;

		/*******************************************************************************************************
			JSON SchemaInfo Class
		*******************************************************************************************************/

		//! The SchemaInfo class describes the data (if any) a Node stores in a WJElements userdtls_ member
		class SchemaInfo
		{
		protected:
			struct Schema
			{
				//! The Cache this belongs to
				CachePtr								_cache;
				//! true if this has a $ref member
				bool										_isRef;
				//! inlined meta schema id's
				NodeMapPtr							_mapInlined;
				// URI
				URI											_uri;

				Schema(CachePtr cache, URIPtr pURI) : _cache(cache), _isRef(false), _mapInlined(NULL), _uri(pURI) {};
				Schema(CachePtr cache, const std::string & strURI) : _cache(cache), _isRef(false), _mapInlined(NULL), _uri(strURI) {};
				~Schema()
				{
					delete _mapInlined;
				}
			};

			struct Info
			{
				//! schema details
				Schema*									_s;
				//! list of validators
				WJEList*								_v;

				Info() : _s(NULL), _v(NULL) {}
				~Info()
				{
					delete _s;
					delete _v;
				}
			};

			WJElement	_e;

			Info*										_info()												{ return _e && _e->usr ? (Info*) _e->usr : NULL; }
			Info*										_createInfo();
			WJEList*								_createValidators();

			Schema*									_schema()											{ Info* i = _info(); return i ? i->_s : NULL; }

		public:
			SchemaInfo(WJElement	wje = NULL) : _e(wje) {}

			// access info details
			WJEList*								validators()									{ Info* i = _info(); return i ? i->_v : NULL; }

			// access schema details
			CachePtr								cache()												{ Schema* s = _schema(); return s ? s->_cache : NULL; }
			bool										isRef()												{ Schema* s = _schema(); return s ? s->_isRef : false; }
			NodeMapPtr							mapInlined()									{ Schema* s = _schema(); return s ? s->_mapInlined : NULL; }
			URI*										uri()													{ Schema* s = _schema(); return s ? &(s->_uri) : NULL; }

			// setter
			void										isRef(bool b)									{ Schema* s = _schema(); if (s) s->_isRef=b; }

			// remove and add a schema to the validators
			void										unregInlineSchema(Node& node);
			void										regInlineSchema(Node& node, const std::string& strJsnPtr);
			Node										getInlineSchema(const std::string& strJsnPtr);

			// simple queries
			bool										hasSchemaInfo()								{ return _schema() != NULL; }
			bool										hasValidators()								{ return validators() != NULL; }

			// add validators
			void										addValidatorSchema(Node& node);
			void										removeValidatorSchema(Node& node);

			void										initSchema(CachePtr pCache, URIPtr pURI);
			void										initSchema(CachePtr pCache, const std::string& strURI);

															operator bool()								{ return _e && _e->usr;	}
			SchemaInfo&							operator=(SchemaInfo& rhs)		{ _e = rhs._e; return *this;	}
			SchemaInfo&							operator=(Node& rhs)					{ _e = *rhs; return *this;	}
			SchemaInfo&							operator=(WJElement e)				{ _e = e;	return *this;	}

			// discards all schema info from the element
			void										discard();

			// discards only validators
			void										discardValidators();
		};


		std::string							_asJsonPointer(std::string& str);

		void										_discardValidators();

		std::ostream &					_dump(std::ostream & os, int indent);

		static Node							_loadMetaSchema(CachePtr pCache);
		void										_validateMetaSchemaMember(CachePtr pCache);

		static Node							_createEmptySchema(CachePtr pCache);

		bool										_validateType(Node& node, std::ostream& err, bool log = true);
		bool										_validateSingleType(Node& node, const std::string& type);
		bool										_validateInstance(Node& node, std::ostream& err, bool log = true);

		bool										_validateNumber(Node& node, std::ostream& err, bool log = true);
		bool										_validateString(Node& node, std::ostream& err, bool log = true);
		bool										_validateObject(Node& node, std::ostream& err, bool log = true);
		bool										_validateArray(Node& node, std::ostream& err, bool log = true);

		Node										_resolveRef(NodeVect& refs);
		Node										_resolveRef()													{ NodeVect refs; return _resolveRef(refs); }

		void										_registerInlineSchema();
		Node										_getInlinedSchema(URI& uri);

		Node										_getRootSchema();

		bool										_markRootAsSchema(CachePtr pCache, const std::string& strURI = "");
		bool										_markAsSchema();

		WJElement								_beforeAdd(const std::string& name);

	public:
		Node(WJElement e = NULL) : _e(e) {}
		Node(const Node& j) : _e(j._e) {}

		//@{ support to access children of container Nodes

		//! use the iterator class to traverse children
		/*! \note while using operator[](unsigned int) works with Node where isContainer() == true, it is a sequential lookup and not cheap
		*/
		class iterator
		{
			friend class Node;

		protected:
			WJElement _e;
			iterator(WJElement e = NULL) : _e(e) {};

		public:
			Node				operator*()							{ return Node(_e); }
			iterator		operator++(int)					{ if (_e) _e = _e->next; return *this; }
			bool				operator==(iterator r)	{ return _e == r._e; }
			bool				operator!=(iterator r)	{ return _e != r._e; }
			iterator		operator=(iterator rhs)	{ _e = rhs._e; return *this; }
		};

		bool										empty()												{ return !_e || !_e->child; }
		unsigned int						size()												{ return _e ? _e->count : 0; }
		iterator								begin()												{ return iterator(_e ? _e->child : NULL); }
		iterator								end()													{ return iterator(); }

		//! Access a child (if isContainer() == false or the element can't be found, the returned elements operator!() will return true) 
		/*! str is a selector.
				Please refer to:
					https://github.com/netmail-open/wjelement/wiki/WJElement-Selectors
				for a detailed description of selectors
		*/
		Node										operator[](const std::string& str);
		Node										operator[](const char* str)		{ return operator[](std::string(str)); }
		//! Access a child by index (if isContainer() == false or the element can't be found, the returned elements operator!() will return true) 
		/*! \note this works for both arrays and objects, but is not particularly meaningful for objects
		    \note it is recommended you use the iterator for iterating instead of this method
		*/
		Node										operator[](unsigned int i);
		Node										operator[](int i)							{ return operator[]((unsigned int) i); }
		//@}

		static Node							parseJson(const std::string & json);

		//! The Type of Node
		WJRType									getType() const								{ return _e ? _e->type : WJR_TYPE_UNKNOWN; }
		std::string							getTypeAsString();
		std::string							getName();

		// returns a std::string vector containing all the names of members this has
		StringVect							getMembers();
		// returns true if this is a container that has an immediate child with the specified name
		bool										isMember(const std::string& name);

		bool										isMetaSchema();
		bool										isSchema()										{ return SchemaInfo(_e).hasSchemaInfo(); }
		bool										isRoot()											{ return _e && !_e->parent; }
		bool										isRootSchema()								{ return isRoot() && isSchema(); }

		bool										isNull()											{ return !_e || _e->type == WJR_TYPE_NULL; }
		bool										isContainer()									{ return _e && (_e->type == WJR_TYPE_OBJECT || _e->type == WJR_TYPE_ARRAY); }
		bool										isObject()										{ return _e && _e->type == WJR_TYPE_OBJECT; }
		bool										isArray()											{ return _e && _e->type == WJR_TYPE_ARRAY; }
		bool										isBoolean()										{ return _e && (_e->type == WJR_TYPE_BOOL || _e->type == WJR_TYPE_TRUE || _e->type == WJR_TYPE_FALSE); }
		bool										isNumber()										{ return _e && _e->type == WJR_TYPE_NUMBER; }
		bool										isInteger()										{ return _e && _e->type == WJR_TYPE_INTEGER; }
		bool										isString()										{ return _e && _e->type == WJR_TYPE_STRING; }

		unsigned int						getDepth();
		Node										getRoot();
		Node										getParent()										{ return _e && _e->parent ? Node(_e->parent) : Node(NULL); }


		// removes this from it's parent so that it becomes a root
		void										detach();
		// attaches the child to this (this must be an array or object and neither this nor child must null)
		void										attach(Node& child);

		std::string							asJsonPointer()								{ std::string str; return _asJsonPointer(str); }

		bool										operator==(Node& rhs);
		bool										operator!=(Node& rhs)					{ return !operator==(rhs); } 
		bool										operator!()										{ return _e == NULL; }
		WJElement								operator*() const							{ return _e; }

														operator WJElement()					{ return _e; }
/*
														operator bool()								{ return getBool(); }
														operator double()							{ return getNum(); }
														operator int()								{ return getInt(); }
														operator std::string()				{ return getString(); }
*/

		std::ostream &					dump(std::ostream & os, int indent = 0);

		//! deletes this' and all its depended WJElements (operator!() will return true after this call;
		void										discard();

		bool										validateInstance(Node& node, const std::string& strURI = "");

		// create roots
		static Node							createObject();
		static Node							createArray();

		// add nodes to this (this must be a valid container)
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

		// get values
		std::string							getString();
		bool										getBool();
		int											getInt()																				{ return (int) getInt32(); }
		int32										getInt32();
		uint32									getUInt32();
		int64										getInt64();
		uint64									getUInt64();
		double									getNum();

		// set values
		void										setNull();
		void										setString(const std::string& value);
		void										setBool(bool value);
		void										setInt(int value)																{ return setInt32((int32) value); }
		void										setInt32(int32 value);
		void										setUInt32(uint32 value);
		void										setInt64(int64 value);
		void										setUInt64(uint64 value);
		void										setDouble(double value);

	};





	/*******************************************************************************************************
		JSON Schema Cache Class
	*******************************************************************************************************/

	//! See Cache::loadSchema for an explanation/requirement of function of this type
	typedef std::string (*SchemaLoaderFunc)(const std::string& strURI);

	// ensures cache gets deleted
	class Cache
	{
		friend class Node;

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

		NodeMap						m_mapSchema;
		Node							m_metaSchema;
		Node							m_emptySchema;
		NodeSet						m_set;
		SchemaLoaderFunc	m_fnLoader;

		void							_onNodeCreated(Node& schema)		{ m_set.insert(schema); }
		void							_onNodeDeleted(Node& schema)		{ m_set.erase(m_set.find(schema)); }

		void							_onSchemaCreated(Node& schema, URIPtr pURI);
		void							_onSchemaDeleted(Node& schema, URIPtr pURI);

	public:
		Cache(SchemaLoaderFunc fnLoader = NULL) : m_fnLoader(fnLoader) { initialise(); }
		~Cache();

		void							initialise();

		//! Returns the Node that specifies the schema requested in the URI
		/*! The method first looks in its own cache and returns what the schema when it finds it

				If the schema is not found, the mothod calls m_fnLoader if it is defined. The method
				is expected to retrieve the resource at the URI specified and return its raw form to this.
				Return an empty std::string to indicate the resource can't be found.
		*/
		Node							loadSchema(const std::string& strURI);
		bool							loadSchema(Node& schema);

		Node							getMetaSchema()							{ return m_metaSchema; }
		Node							getEmptySchema()						{ return m_emptySchema; }

		std::string				getUniqueID()								{ return IDGenerator::getID(); }

	};


	inline bool Node_cmp::operator()(const Node& lhs, const Node& rhs) const
	{
		return *lhs < *rhs;
	}

} /* namespace WJPP */

#endif /* _wjelementcpp_h_ */
