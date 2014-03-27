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

using namespace ::std;	


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

	typedef vector<string>										StringVect;
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
		string				m_mode;
		string				m_host;
		string				m_path;
		string				m_query;
		string				m_jsonPointer;
		StringVect		m_vectPath;
		StringVect		m_vectJsonPointer;

		static string	decodeJsonPointer(const string& strPtr);
		static string	encodeJsonPointer(const string& strPtr);

	public:
		URI(const string& strURI)
			: m_pBaseURI(NULL)
		{
			setURI(strURI);
		}

		URI(URIPtr pBaseURI = NULL, const string& strURI = "")
			: m_pBaseURI(pBaseURI)
		{
			setURI(strURI);
		}

		void					setURI(const string& strURI);

		string				getURI();
		string				getBaseURI();	//!< full URI but without JsonPointers
		string				getFullURI();

		string&				getMode();
		string&				getHost();
		string				getPath();
		string&				getQuery();
		string				getJsonPointer();

		StringVect		getPathVect();
		StringVect		getJsonPointerVect();

		bool					hasJsonPointer()		{ return !m_vectJsonPointer.empty(); }

		// returns true if mode, host, path, query amd pointer all are empty
		bool					empty();
	};






	/*******************************************************************************************************
		JSON Node class
	*******************************************************************************************************/

	//! Lightweight WJElement wrapper
	class Node
	{
		friend class	Node;
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

				Schema(CachePtr cache, URIPtr pURI) : _cache(cache), _mapInlined(NULL), _isRef(false), _uri(pURI) {};
				Schema(CachePtr cache, const string & strURI) : _cache(cache), _mapInlined(NULL), _isRef(false), _uri(strURI) {};
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
			void										regInlineSchema(Node& node, const string& strJsnPtr);
			Node										getInlineSchema(const string& strJsnPtr);

			// simple queries
			bool										hasSchemaInfo()								{ return _schema() != NULL; }
			bool										hasValidators()								{ return validators() != NULL; }

			// add validators
			void										addValidatorSchema(Node& node);
			void										removeValidatorSchema(Node& node);

			void										initSchema(CachePtr pCache, URIPtr pURI);
			void										initSchema(CachePtr pCache, const string& strURI);

															operator bool()								{ return _e && _e->usr;	}
			SchemaInfo&							operator=(SchemaInfo& rhs)		{ _e = rhs._e; return *this;	}
			SchemaInfo&							operator=(Node& rhs)					{ _e = *rhs; return *this;	}
			SchemaInfo&							operator=(WJElement e)				{ _e = e;	return *this;	}

			// discards all schema info from the element
			void										discard();

			// discards only validators
			void										discardValidators();
		};


		string									_asJsonPointer(string& str);

		void										_discardValidators();

		ostream &								_dump(ostream & os, int indent);

		static Node							_loadMetaSchema(CachePtr pCache);
		void										_validateMetaSchemaMember(CachePtr pCache);

		static Node							_createEmptySchema(CachePtr pCache);

		bool										_validateType(Node& node, ostream& err, bool log = true);
		bool										_validateSingleType(Node& node, const string& type);
		bool										_validateInstance(Node& node, ostream& err, bool log = true);

		bool										_validateNumber(Node& node, ostream& err, bool log = true);
		bool										_validateString(Node& node, ostream& err, bool log = true);
		bool										_validateObject(Node& node, ostream& err, bool log = true);
		bool										_validateArray(Node& node, ostream& err, bool log = true);

		Node										_resolveRef(NodeVect& refs = NodeVect());

		void										_registerInlineSchema();
		Node										_getInlinedSchema(URI& uri);

		Node										_getRootSchema();

		bool										_markRootAsSchema(CachePtr pCache, const string& strURI = "");
		bool										_markAsSchema();

		WJElement								_beforeAdd(const string& name);

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
			friend class Node;

		protected:
			WJElement _e;
			iterator(WJElement e = NULL) : _e(e) {};

		public:
			Node				operator*()							{ return Node(_e); }
			iterator&		operator++(int)					{ if (_e) _e = _e->next; return *this; }
			bool				operator==(iterator & r){ return _e == r._e; }
			bool				operator!=(iterator & r){ return _e != r._e; }
			iterator&		operator=(iterator& rhs){ _e = rhs._e; return *this; }
		};

		bool										empty()												{ return !_e || !_e->child; }
		unsigned int						size()												{ return _e ? _e->count : 0; }
		iterator								begin()												{ return (_e && _e->child) ? iterator(_e->child) : iterator(NULL); }
		iterator								end()													{ return iterator(); }

		//! Access a child (if isContainer() == false or the element can't be found, the returned elements operator!() will return true) 
		/*! str is a selector.
				Please refer to:
					https://github.com/netmail-open/wjelement/wiki/WJElement-Selectors
				for a detailed description of selectors
		*/
		Node										operator[](const string& str);
		Node										operator[](const char* str)		{ return operator[](string(str)); }
		//! Access a child by index (if isContainer() == false or the element can't be found, the returned elements operator!() will return true) 
		/*! \note this works for both arrays and objects, but is not particularly meaningful for objects
		    \note it is recommended you use the iterator for iterating instead of this method
		*/
		Node										operator[](unsigned int i);
		Node										operator[](int i)							{ return operator[]((unsigned int) i); }
		//@}

		static Node							parseJson(const string & json);

		//! The Type of Node
		WJRType									getType() const								{ return _e ? _e->type : WJR_TYPE_UNKNOWN; }
		string									getTypeAsString();
		string									getName();

		// returns a string vector containing all the names of members this has
		StringVect							getMembers();
		// returns true if this is a container that has an immediate child with the specified name
		bool										isMember(const string& name);

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

		bool										getBool();
		int											getInt();
		double									getNum();
		string									getString();

		unsigned int						getDepth();
		Node										getRoot();

		// removes this from it's parent so that it becomes a root
		void										detach();
		// attaches the child to this (this must be an array or object and neither this nor child must null)
		void										attach(Node& child);

		string									asJsonPointer()								{ return _asJsonPointer(string()); }

		bool										operator==(Node& rhs);
		bool										operator!=(Node& rhs)					{ return !operator==(rhs); } 
		bool										operator!()										{ return _e == NULL; }
		WJElement								operator*() const							{ return _e; }

														operator WJElement()					{ return _e; }
/*
														operator bool()								{ return getBool(); }
														operator double()							{ return getNum(); }
														operator int()								{ return getInt(); }
														operator string()							{ return getString(); }
*/
		Node										getParent()										{ return _e && _e->parent ? Node(_e->parent) : Node(NULL); }
		Node										getSchema();

		ostream &								dump(ostream & os, int indent = 0);

		//! deletes this' and all its depended WJElements (operator!() will return true after this call;
		void										discard();

		bool										validateInstance(Node& node, const string& strURI = "");

		// add nodes to this (this must be a valid container)
		Node										addObject(const string& name);
		Node										addArray(const string& name);
		Node										addNull(const string& name);
		Node										addString(const string& name, const string& value);
		Node										addBool(const string& name, bool value);
		Node										addInt(const string& name, int value);
//		Node										addInt32(const string& name, int32 value);
//		Node										addUInt32(const string& name, uint32 value);
//		Node										addInt64(const string& name, int64 value);
//		Node										addUInt64(const string& name, uint64 value);
		Node										addDouble(const string& name, double value);

		// set values
		Node										setNull();
		void										setString(const string& value);
		void										setBool(bool value);
		void										setInt(int value);
//		Node										setInt32(int32 value);
//		Node										setUInt32(uint32 value);
//		Node										setInt64(int64 value);
//		Node										setUInt64(uint64 value);
		void										setDouble(double value);

	};





	/*******************************************************************************************************
		JSON Schema Cache Class
	*******************************************************************************************************/

	//! See Cache::loadSchema for an explanation/requirement of function of this type
	typedef string (*SchemaLoaderFunc)(const string& strURI);

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
			string			m_strID;
			IDGenerator() : m_strID("http://localhost/schema/aaaa") {};

		public:
			static string		getID();

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
				Return an empty string to indicate the resource can't be found.
		*/
		Node							loadSchema(const string& strURI);
		bool							loadSchema(Node& schema);

		Node							getMetaSchema()							{ return m_metaSchema; }
		Node							getEmptySchema()						{ return m_emptySchema; }

		string						getUniqueID()								{ return IDGenerator::getID(); }

	};


	inline bool Node_cmp::operator()(const Node& lhs, const Node& rhs) const
	{
		return *lhs < *rhs;
	}

} /* namespace WJPP */

#endif /* _wjelementcpp_h_ */
