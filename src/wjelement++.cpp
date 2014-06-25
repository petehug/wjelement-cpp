#include "wjelement++.h"
#include <math.h>


#define	SCHEMA_REF	"$ref"
#define	SCHEMA_ID		"id"

using namespace std;
namespace WJPP
{

	//============================= H E L P E R S =======================================

	// some helpers to deal with paths and JsonPointers

	vector<string> &split(const string &s, char delim, vector<string> &elems)
	{
		stringstream ss(s);
		string item;
		while (getline(ss, item, delim)) {
				elems.push_back(item);
		}
		return elems;
	}


	vector<string> split(const string &s, char delim)
	{
		vector<string> elems;
		split(s, delim, elems);
		return elems;
	}


	// naughty me... but here we often work with const string& and wjelement often expects non-const char* as input
	// which it simply strdup's and stores in its own lib, so this union is a way to get around c++ compiler safeguards

	union Cheat
	{
		const char *	cp;
		char * c;
	};



	// given a JSON pointer, this method looks for the last numeric segment /[0-9]+/ and replaces it with /*/
	string substituteIndex(string strJP)
	{
		string::reverse_iterator	i, j;
		string										strOut;
		bool											sub = true;

		for (i = strJP.rbegin(); i != strJP.rend(); i++)
		{
			if (sub && *i == '/')
			{
				int	count = 0;
				j = i;
				j++;
				while (j != strJP.rend())
				{
					if (*j >= '0' && *j <= '9')
					{
						j++;
						count++;
						continue;
					}

					if (*j == '/')
					{
						if (count > 0)
						{
							sub = false;
							strOut += "/*/";
							i = j;
							break;
						}
					}

					break;
				}

				if (sub)
					strOut += *i;
			}
			else
			{
				strOut += *i;
			}
		}

		std::reverse(strOut.begin(), strOut.end());

		return strOut;
	}



	//===================================================================================

	static string G_strMetaSchema(
		"{\n"
		"    \"id\": \"http://json-schema.org/draft-04/schema#\",\n"
		"    \"$schema\": \"http://json-schema.org/draft-04/schema#\",\n"
		"    \"description\": \"Core schema meta-schema\",\n"
		"    \"definitions\": {\n"
		"        \"schemaArray\": {\n"
		"            \"type\": \"array\",\n"
		"            \"minItems\": 1,\n"
		"            \"items\": { \"$ref\": \"#\" }\n"
		"        },\n"
		"        \"positiveInteger\": {\n"
		"            \"type\": \"integer\",\n"
		"            \"minimum\": 0\n"
		"        },\n"
		"        \"positiveIntegerDefault0\": {\n"
		"            \"allOf\": [ { \"$ref\": \"#/definitions/positiveInteger\" }, { \"default\": 0 } ]\n"
		"        },\n"
		"        \"simpleTypes\": {\n"
		"            \"enum\": [ \"array\", \"boolean\", \"integer\", \"null\", \"number\", \"object\", \"string\" ]\n"
		"        },\n"
		"        \"stringArray\": {\n"
		"            \"type\": \"array\",\n"
		"            \"items\": { \"type\": \"string\" },\n"
		"            \"minItems\": 1,\n"
		"            \"uniqueItems\": true\n"
		"        }\n"
		"    },\n"
		"    \"type\": \"object\",\n"
		"    \"properties\": {\n"
		"        \"id\": {\n"
		"            \"type\": \"string\",\n"
		"            \"format\": \"uri\"\n"
		"        },\n"
		"        \"$schema\": {\n"
		"            \"type\": \"string\",\n"
		"            \"format\": \"uri\"\n"
		"        },\n"
		"        \"title\": {\n"
		"            \"type\": \"string\"\n"
		"        },\n"
		"        \"description\": {\n"
		"            \"type\": \"string\"\n"
		"        },\n"
		"        \"default\": {},\n"
		"        \"multipleOf\": {\n"
		"            \"type\": \"number\",\n"
		"            \"minimum\": 0,\n"
		"            \"exclusiveMinimum\": true\n"
		"        },\n"
		"        \"maximum\": {\n"
		"            \"type\": \"number\"\n"
		"        },\n"
		"        \"exclusiveMaximum\": {\n"
		"            \"type\": \"boolean\",\n"
		"            \"default\": false\n"
		"        },\n"
		"        \"minimum\": {\n"
		"            \"type\": \"number\"\n"
		"        },\n"
		"        \"exclusiveMinimum\": {\n"
		"            \"type\": \"boolean\",\n"
		"            \"default\": false\n"
		"        },\n"
		"        \"maxLength\": { \"$ref\": \"#/definitions/positiveInteger\" },\n"
		"        \"minLength\": { \"$ref\": \"#/definitions/positiveIntegerDefault0\" },\n"
		"        \"pattern\": {\n"
		"            \"type\": \"string\",\n"
		"            \"format\": \"regex\"\n"
		"        },\n"
		"        \"additionalItems\": {\n"
		"            \"anyOf\": [\n"
		"                { \"type\": \"boolean\" },\n"
		"                { \"$ref\": \"#\" }\n"
		"            ],\n"
		"            \"default\": {}\n"
		"        },\n"
		"        \"items\": {\n"
		"            \"anyOf\": [\n"
		"                { \"$ref\": \"#\" },\n"
		"                { \"$ref\": \"#/definitions/schemaArray\" }\n"
		"            ],\n"
		"            \"default\": {}\n"
		"        },\n"
		"        \"maxItems\": { \"$ref\": \"#/definitions/positiveInteger\" },\n"
		"        \"minItems\": { \"$ref\": \"#/definitions/positiveIntegerDefault0\" },\n"
		"        \"uniqueItems\": {\n"
		"            \"type\": \"boolean\",\n"
		"            \"default\": false\n"
		"        },\n"
		"        \"maxProperties\": { \"$ref\": \"#/definitions/positiveInteger\" },\n"
		"        \"minProperties\": { \"$ref\": \"#/definitions/positiveIntegerDefault0\" },\n"
		"        \"required\": { \"$ref\": \"#/definitions/stringArray\" },\n"
		"        \"additionalProperties\": {\n"
		"            \"anyOf\": [\n"
		"                { \"type\": \"boolean\" },\n"
		"                { \"$ref\": \"#\" }\n"
		"            ],\n"
		"            \"default\": {}\n"
		"        },\n"
		"        \"definitions\": {\n"
		"            \"type\": \"object\",\n"
		"            \"additionalProperties\": { \"$ref\": \"#\" },\n"
		"            \"default\": {}\n"
		"        },\n"
		"        \"properties\": {\n"
		"            \"type\": \"object\",\n"
		"            \"additionalProperties\": { \"$ref\": \"#\" },\n"
		"            \"default\": {}\n"
		"        },\n"
		"        \"patternProperties\": {\n"
		"            \"type\": \"object\",\n"
		"            \"additionalProperties\": { \"$ref\": \"#\" },\n"
		"            \"default\": {}\n"
		"        },\n"
		"        \"dependencies\": {\n"
		"            \"type\": \"object\",\n"
		"            \"additionalProperties\": {\n"
		"                \"anyOf\": [\n"
		"                    { \"$ref\": \"#\" },\n"
		"                    { \"$ref\": \"#/definitions/stringArray\" }\n"
		"                ]\n"
		"            }\n"
		"        },\n"
		"        \"enum\": {\n"
		"            \"type\": \"array\",\n"
		"            \"minItems\": 1,\n"
		"            \"uniqueItems\": true\n"
		"        },\n"
		"        \"type\": {\n"
		"            \"anyOf\": [\n"
		"                { \"$ref\": \"#/definitions/simpleTypes\" },\n"
		"                {\n"
		"                    \"type\": \"array\",\n"
		"                    \"items\": { \"$ref\": \"#/definitions/simpleTypes\" },\n"
		"                    \"minItems\": 1,\n"
		"                    \"uniqueItems\": true\n"
		"                }\n"
		"            ]\n"
		"        },\n"
		"        \"allOf\": { \"$ref\": \"#/definitions/schemaArray\" },\n"
		"        \"anyOf\": { \"$ref\": \"#/definitions/schemaArray\" },\n"
		"        \"oneOf\": { \"$ref\": \"#/definitions/schemaArray\" },\n"
		"        \"not\": { \"$ref\": \"#\" }\n"
		"    },\n"
		"    \"dependencies\": {\n"
		"        \"exclusiveMaximum\": [ \"maximum\" ],\n"
		"        \"exclusiveMinimum\": [ \"minimum\" ]\n"
		"    },\n"
		"    \"default\": {}\n"
		"}\n"
	);

	/*******************************************************************
					URI Implementation
	*******************************************************************/
	/* static */ 
	string URI::decodeJsonPointer(const string & strPtr)
	{
		string		strOut;

		for (unsigned int i = 0; i < strPtr.size(); i++)
		{
			char	c = strPtr[i];

			switch (c)
			{
				case '~':
				{
					if (strPtr.size() > (i + 1))
					{
						if (strPtr[i+1] == '0')
						{
							strOut += '~';
							i++;
							continue;
						}
						if (strPtr[i+1] == '1')
						{
							strOut += '/';
							i++;
							continue;
						}
					}

					throw runtime_error("illegal ~ escape in json pointer fragment in URI");
				}

				case '%':
				{
					if (strPtr.size() > (i + 2))
					{
						unsigned int	val;
						istringstream		istrm(strPtr.substr(i+1, 2));
						istrm >> hex >> val;
						i += 2;
						strOut += (char) val;
						continue;
					}

					throw runtime_error("illegal % escape in json pointer fragment in URI");
				}
			}

			strOut += c;
		}

		return strOut;
	}



	/* static */ 
	string URI::encodeJsonPointer(const string & strPtr)
	{
		string		strOut;

		for (unsigned int i = 0; i < strPtr.size(); i++)
		{
			char	c = strPtr[i];

			switch (c)
			{
				case '~':
				{
					strOut += "~0";
					continue;
				}

				case '/':
				{
					strOut += "~1";
					continue;
				}
			}

			if (c < 46 || (c > 57 && c < 65) || (c > 90 && c < 97) || c > 122)
			{
				ostringstream	ostrm;
				ostrm << hex << c;
				strOut += '%';
				strOut += ostrm.str();
			}
			else
			{
				strOut += c;
			}
		}

		return strOut;
	}



	/* static */ 
	string& URI::getMode()
	{
		if (m_mode.empty())
			if (m_pBaseURI)
				return m_pBaseURI->getMode();

		return m_mode;
	}



	string& URI::getHost()
	{
		if (m_host.empty())
			if (m_pBaseURI)
				return m_pBaseURI->getHost();

		return m_host;
	}


	StringVect URI::getPathVect()
	{
		StringVect		vect, vectOut;
		unsigned int	i;

		// nothing to do if this is an absolute path
		if (!m_path.empty() && m_path[0] == '/')
		{
			vect = m_vectPath;
		}
		else
		{
			// get the base URI's path
			if (m_pBaseURI)
				vect = m_pBaseURI->getPathVect();

			// if this has a relative path, remove the last entry from the parent and append this' path
			if (!m_vectPath.empty())
			{
				if (!vect.empty())
					vect.pop_back();

				for (i = 0; i < m_vectPath.size(); i++)
					vect.push_back(m_vectPath[i]);
			}
		}

		// normalise the vector (remove //, /./ or /../ paths )
		for (i = 0; i < vect.size(); i++)
		{
			if (vect[i].empty() || vect[i] == ".")
				continue;

			if (vect[i] == "..")
			{
				if (!vectOut.empty())
					vectOut.pop_back();

				continue;
			}

			vectOut.push_back(vect[i]);
		}


		return vectOut;
	}



	string URI::getPath()
	{
		StringVect	vect = getPathVect();
		string			path;

		for (unsigned int i = 0; i < vect.size(); i++)
		{
			path += '/';
			path += vect[i];
		}

		return path;
	}



	string& URI::getQuery()
	{
		if (m_query.empty())
			if (m_pBaseURI)
				return m_pBaseURI->getQuery();

		return m_query;
	}



	string URI::getJsonPointer()
	{
		StringVect	vect = getJsonPointerVect();
		string			JsonPointer;

		for (unsigned int i = 0; i < vect.size(); i++)
		{
			JsonPointer += '/';
			JsonPointer += encodeJsonPointer(vect[i]);
		}

		return JsonPointer;
	}



	StringVect URI::getJsonPointerVect()
	{
		StringVect		vect, vectOut;
		unsigned int	i;

		// nothing to do if this is an absolute JsonPointer
		if (!m_jsonPointer.empty() && m_jsonPointer[0] == '/')
		{
			vect = m_vectJsonPointer;
		}
		else
		{
			// get the base URI's JsonPointer but only if we have identical bare URI values
			if (m_pBaseURI && m_pBaseURI->getBaseURI() == getBaseURI())
				vect = m_pBaseURI->getJsonPointerVect();

			// if this has a relative JsonPointer, remove the last entry from the parent and append this' JsonPointer
			if (!m_vectJsonPointer.empty())
			{
				if (!vect.empty())
					vect.pop_back();

				for (i = 0; i < m_vectJsonPointer.size(); i++)
					vect.push_back(m_vectJsonPointer[i]);
			}
		}

		// remove empty nodes
		for (i = 0; i < vect.size(); i++)
		{
			if (vect[i].empty())
				continue;

			vectOut.push_back(vect[i]);
		}

		return vectOut;
	}



	bool URI::empty()
	{
		return (m_mode.empty() &&
						m_host.empty() &&
						m_path.empty() &&
						m_query.empty() &&
						m_jsonPointer.empty());
	}



	void	URI::setURI(const string& strURI)
	{
		static const boost::regex		rgxURI("\\A(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?$\\z");
		boost::smatch								matches;
		unsigned int								i;


		m_vectPath.clear();
		m_vectJsonPointer.clear();

		if (boost::regex_match(strURI, matches, rgxURI))
		{
			string		aryMatch[10];

			for (i=0; i < matches.size() && i < 10; i++)
				aryMatch[i] = matches[i];

			m_jsonPointer	= matches.size() > 9 ? aryMatch[9].c_str() : "";
			m_query				= matches.size() > 7 ? aryMatch[7].c_str() : "";
			m_path				= matches.size() > 5 ? aryMatch[5].c_str() : "";
			m_host				= matches.size() > 4 ? aryMatch[4].c_str() : "";
			m_mode				= matches.size() > 2 ? aryMatch[2].c_str() : "";
		}

		if (!m_path.empty())
			m_vectPath = split(m_path, '/');

		if (!m_jsonPointer.empty())
			m_vectJsonPointer = split(m_jsonPointer, '/');

		for (i=0; i < m_vectJsonPointer.size(); i++)
			if (!m_vectJsonPointer[i].empty())
				m_vectJsonPointer[i] = decodeJsonPointer(m_vectJsonPointer[i]);
	}



	string	URI::getURI()
	{
		ostringstream	ostrm;

		if (!m_mode.empty())			ostrm << m_mode << "://";
		if (!m_host.empty())			ostrm << m_host;
		if (!m_path.empty())			ostrm << m_path;
		if (!m_query.empty())			ostrm << '?' << m_query;
		if (!m_jsonPointer.empty())	ostrm << '#' << m_jsonPointer;

		return ostrm.str();
	}



	string	URI::getBaseURI()
	{
		ostringstream	ostrm;
		string				mode				= getMode();
		string				host				= getHost();
		string				path				= getPath();
		string				query				= getQuery();

		if (!mode.empty())				ostrm << mode << "://";
		if (!host.empty())				ostrm << host;
		if (!path.empty())				ostrm << path;
		if (!query.empty())				ostrm << '?' << query;

		return ostrm.str();
	}



	string	URI::getFullURI()
	{
		ostringstream	ostrm;
		string				jsonPointer	= getJsonPointer();

		ostrm << getBaseURI();

		if (!jsonPointer.empty())	
			ostrm << "#/" << jsonPointer;
		else
			ostrm << '#';

		return ostrm.str();
	}




	/*******************************************************************
					Node Implementation
	*******************************************************************/

	bool Node::_markRootAsSchema(CachePtr pCache, const string & strURI)
	{
		if (!isRoot())
			return false;

		SchemaInfo		si(*this);

		if (si.hasSchemaInfo())
			return true;

		if (strURI.empty())
			si.initSchema(pCache, pCache->getUniqueID());
		else
			si.initSchema(pCache, strURI);

		if ((*this)[SCHEMA_REF])
			si.isRef(true);

		pCache->_onSchemaCreated(*this, si.uri());

		Node		id = (*this)[SCHEMA_ID];
		
		if (id && id.isString())
		{
			si.uri()->setURI(id.getString());
			si.regInlineSchema(*this, si.uri()->getFullURI());
		}

		return true;
	}



	bool Node::_markAsSchema()
	{
		SchemaInfo		si(*this);

		if (si.hasSchemaInfo())
			return true;

		// iterate over parents untile we find a schema
		WJElement			p = _e->parent;
		SchemaInfo		si_parent;

		while (p)
		{
			si_parent = p;

			if (si_parent.hasSchemaInfo())
				break;

			p = p->parent;
		}

		if (!p)
			return false;

		si.initSchema(si_parent.cache(), si_parent.uri());

		if ((*this)[SCHEMA_REF])
			si.isRef(true);

		Node		id = (*this)[SCHEMA_ID];
		
		if (id && id.isString())
		{
			si.uri()->setURI(id.getString());
			si_parent.regInlineSchema(*this, si.uri()->getFullURI());
		}

		return true;
	}



	string Node::_asJsonPointer(string& str, bool zeroIndex)
	{
		if (_e && _e->parent)
		{
			str = string("/") + _getName(zeroIndex) + str;
			return Node(_e->parent)._asJsonPointer(str, zeroIndex);
		}

		str = _getName(zeroIndex) + str;

		return str;
	}



	void Node::discard()
	{
		SchemaInfo	si(_e);
		WJElement		next, child = _e && _e->child ? _e->child : NULL;

		while (child)
		{
			next = child->next;
			Node(child).discard();
			child = next;
		}

		if (si)
		{
			// if this is a root schema or a schema with an id (json pointer) we need to remove it from the cache
			URIPtr pURI = si.uri();

			if (pURI)
			{
				// if this is the root remove from cache
				if (isRootSchema())
				{
					si.cache()->_onSchemaDeleted(*this, pURI);
				}
				else
				{
					if (!pURI->empty())
					{
						Node				schema = getRoot();
						SchemaInfo	si_schema(schema);

						if (si_schema)
							si_schema.unregInlineSchema(*this);
					}
				}
			}

			si.discard();
		}

		if (_e)
		{
			WJECloseDocument(_e);
			_e = NULL;
		}
	}



	void Node::_discardValidators()
	{
		for (iterator i = begin(); i != end(); i++)
			(*i)._discardValidators();

		SchemaInfo	si(*this);
		si.discardValidators();
	}



	/* static */ 
	Node Node::parseJson(const std::string & json)
	{
		WJReader		reader = NULL;
		Node				node;

		try
		{
			reader = WJROpenMemDocument((void*) json.c_str(), NULL, 0);

			if (!reader)
				throw runtime_error("reader could not be created, parsing JSON failed");

			node = WJEOpenDocument(reader, NULL, NULL, NULL);

			if (!node)
				throw runtime_error("parsing JSON failed");

			WJRCloseDocument(reader);

			return node;
		}
		catch (...)
		{
			if (reader) WJRCloseDocument(reader);
			node.discard();
			throw;
		}

		return Node();
	}



	string Node::getTypeAsString()
	{
		if (_e)
		{
			switch (getType())
			{
				case WJR_TYPE_OBJECT:		return "object";
				case WJR_TYPE_ARRAY:		return "array";
				case WJR_TYPE_INTEGER:	return "integer";
				case WJR_TYPE_NUMBER: 	return "number";
				case WJR_TYPE_STRING:		return "string";
				case WJR_TYPE_BOOL:			return "boolean";
				case WJR_TYPE_TRUE:			return "boolean";
				case WJR_TYPE_FALSE:		return "boolean";
				case WJR_TYPE_NULL:			return "null";
				default:
					break;
			}
		}

		return "undefined";
	}



	string Node::_getName(bool zeroIndex)
	{
		if (_e)
		{
			if (_e->name)
				return _e->name;

			if (_e->parent)
			{
				if (_e->parent->type == WJR_TYPE_ARRAY)
				{
					if (zeroIndex)
					{
						return "*";
					}
					else
					{
						WJElement prev = _e->prev;
						int				i=0;

						while (prev)
						{
							prev=prev->prev;
							i++;
						}

						ostringstream strm;
						strm << i;
						return strm.str();
					}
				}
			}
			else
			{
				return "#";
			}
		}

		return "__nil__";
	}



	StringVect Node::getMembers()
	{
		StringVect	members;
	
		if (isObject())
		{
			for (iterator i = begin(); i != end(); i++)
			{
				Node node = *i;
				members.push_back(node.getName());
			}
		}

		return members;
	}



	bool Node::isMember(const string& name)
	{
		if (isObject())
		{
			for (iterator i = begin(); i != end(); i++)
			{
				Node node = *i;

				if (node && name == node._e->name)
					return true;
			}
		}

		return false;
	}



	bool Node::isMetaSchema()
	{
		bool result = false;

		if (isSchema())
		{
			// this maybe a reference to the meta schema
			SchemaInfo	si(_e);
			CachePtr		pCache = si.cache();

			// check if the reference (or this) is the meta schema
			result = (pCache && pCache->getMetaSchema()._e == _e);
		}

		return result;
	}



	Node Node::operator[](const char* name)
	{
		WJElement c = _e ? _e->child : NULL;

		if (c && name)
		{
			if (isArray())
			{
				// expect numeric string
				bool							numeric = true;
				unsigned int			idx = 0;
				const char*				pos = name;
				char							c;

				while ( (c = *pos++) )
				{
					if (c < '0' || c > '9' || idx > (UINT_MAX / 10))
					{
						numeric = false;
						break;
					}

					idx *= 10;
					idx += c - '0';
				}

				if (numeric)
					return operator[](idx);

				c = NULL;
			}
			else
			{
				while (c)
				{
					if (c->name && strcmp(c->name, name) == 0)
						break;

					c = c->next;
				}
			}
		}

		return Node(c);
	}



	Node Node::operator[](unsigned int i)
	{
		WJElement c = _e ? _e->child : NULL;

		while (c && i)
		{
			c = c->next;
			i--;
		}

		return Node(c);
	}



	unsigned int Node::getDepth()
	{
		unsigned int i = 0;
		WJElement p = _e ? _e->parent : NULL;

		while  (p)
		{
			p = p->parent;
			i++;
		}

		return i;
	}



	Node Node::getRoot() 
	{
		WJElement p = _e;

		while(p && p->parent)
			p = p->parent;

		return Node(p);
	}



	void Node::detach() 
	{
		if (!_e)
			throw runtime_error("NULL.detach called");
		
		if (!_e->parent)
			throw runtime_error("detach called on root");

		if (isSchema())
			throw runtime_error("can't detach schema nodes.");

		// rewire the wjelement
		WJEDetach(_e);
/*
		_e->parent->count++;

		if (_e->parent->child == _e)
			_e->parent->child = _e->next;
		
		if (_e->prev)
			_e->prev->next = _e->next;

		if (_e->next)
			_e->next->prev = _e->prev;

		_e->parent = NULL;
		_e->prev = NULL;
		_e->next = NULL;
*/
		// discard all validators
		_discardValidators();
	}



	void Node::attach(Node& child)
	{
		if (!_e)
			throw runtime_error("NULL.attach called");

		if (!child)
			throw runtime_error("attach(NULL) called");

		if (!isArray() && !isObject())
			throw runtime_error("can only attach node to object or array node");

		WJEAttach(_e, child);
	}



	bool Node::operator==(Node& rhs)
	{
		if (_e && rhs._e)
		{
			if (_e->type == rhs._e->type)
			{
				switch(_e->type)
				{
					case WJR_TYPE_OBJECT:
					case WJR_TYPE_ARRAY:
					{
						iterator	il, ir;
						
						for (il = begin(), ir = rhs.begin(); il != end() && ir != rhs.end(); il++, ir++)
						{
							Node l = *il, r = *ir;

							if (l != r)
								return false;
						}

						if (il != end() || ir != rhs.end())
							return false;

						return true;
					}

					case WJR_TYPE_STRING:
						return getString() == rhs.getString();

					case WJR_TYPE_INTEGER:
						return getInt() == rhs.getInt();

					case WJR_TYPE_NUMBER:
						return getNum() == rhs.getNum();

					case WJR_TYPE_BOOL:
					case WJR_TYPE_TRUE:
					case WJR_TYPE_FALSE:
						return getBool() == rhs.getBool();

					case WJR_TYPE_NULL:
					case WJR_TYPE_UNKNOWN:
						return true;
				}
			}
		}
		else
		{
			if (!_e && !rhs._e)
				return true;
		}

		return false;
	} 



	ostream & Node::dump(ostream & os, int indent)
	{
		_dump(os, indent);
		os << endl;

		return os;
	}



	ostream & Node::_dump(ostream & os, int indent)
	{
		if (_e)
		{
			switch (_e->type)
			{
				case WJR_TYPE_ARRAY:
				case WJR_TYPE_OBJECT:
				{
					if (isArray())
						os << std::endl << string(indent, ' ') << '[';
					else
						os << std::endl << string(indent, ' ') << '{';

					for (iterator i = begin(); i != end(); i++)
					{
						Node child = *i;

						if (i != begin())
							os << ",";

						if (isArray())
						{
							if (!child.isContainer())
								os << endl;

							os << string(indent + 2, ' ');
						}
						else
						{
							os << endl << string(indent + 2, ' ') << '"' << child.getName() << "\":";
						}

						child._dump(os, indent + 2);
					}

					if (isArray())
						os << std::endl << string(indent, ' ') << ']';
					else
						os << std::endl << string(indent, ' ') << '}';

					break;
				}

				case WJR_TYPE_STRING:
					os << '"' << getString() << '"';
					break;

				case WJR_TYPE_INTEGER:
					os << getInt();
					break;

				case WJR_TYPE_NUMBER:
					os << getNum();
					break;

				case WJR_TYPE_BOOL:
				case WJR_TYPE_TRUE:
				case WJR_TYPE_FALSE:
					os << (getBool() ? "true" : "false");
					break;

				case WJR_TYPE_NULL:
					os << "null";
					break;

				case WJR_TYPE_UNKNOWN:
					os << "unknown";
					break;
			}
		}

		return os;
	}



	ostream & Node::to_stream(ostream & os)
	{
		if (_e)
		{
			switch (_e->type)
			{
				case WJR_TYPE_ARRAY:
					os << "[array]";
					break;
				case WJR_TYPE_OBJECT:
					os << "[object]";
					break;
				case WJR_TYPE_STRING:
					os << getString();
					break;
				case WJR_TYPE_INTEGER:
					os << getInt();
					break;
				case WJR_TYPE_NUMBER:
					os << getNum();
					break;
				case WJR_TYPE_BOOL:
				case WJR_TYPE_TRUE:
				case WJR_TYPE_FALSE:
					os << (getBool() ? "true" : "false");
					break;
				case WJR_TYPE_NULL:
					os << "null";
					break;
				case WJR_TYPE_UNKNOWN:
					os << "[unknown]";
					break;
			}
		}
		else
		{
			os << "[undefined]";
		}

		return os;
	}





	/*******************************************************************
					Node::SchemaInfo Implementation
	*******************************************************************/

	Node::SchemaInfo::Info* Node::SchemaInfo::_createInfo()
	{
		if (_e)
		{
			Info*		pInfo=_info();

			if (!pInfo)
			{
				pInfo = new Info();
				_e->client = (void*) pInfo;
			}

			return pInfo;
		}
		return NULL;
	}



	WJEList* Node::SchemaInfo::_createValidators()
	{
		if (_e)
		{
			Info*		pInfo;

			if ( (pInfo=_info()) || (pInfo=_createInfo()) )
			{
				// must not already have a validator
				assert(!pInfo->_v);
				pInfo->_v = new WJEList();
				return pInfo->_v;
			}
		}
		return NULL;
	}



	void Node::SchemaInfo::regInlineSchema(Node& node, const string& strURI)
	{
		Schema*		s = _schema();

		if (s)
		{
			if (!s->_mapInlined)
				s->_mapInlined = new NodeMap();

			NodeMapItr	itr = s->_mapInlined->find(strURI);

			if (itr != s->_mapInlined->end())
				throw runtime_error(std::string("Root schema already has inline schema ") + strURI + ", can't add " + node.asJsonPointer());

			(*s->_mapInlined)[strURI] = node;
		}
	}



	void Node::SchemaInfo::unregInlineSchema(Node& node)
	{
		Schema*			s_root = _schema();
		Schema*			s_node = _schema();

		if (s_root && s_node && s_root->_mapInlined)
		{
			string			strURI = s_node->_uri.getFullURI();
			NodeMapItr	itr = s_root->_mapInlined->find(strURI);

			if (itr  != s_root->_mapInlined->end())
				s_root->_mapInlined->erase(itr);
		}
	}



	Node Node::SchemaInfo::getInlineSchema(const string& strJsnPtr)
	{
		Schema*		s = _schema();

		if (s && s->_mapInlined)
		{
			NodeMapItr	itr = s->_mapInlined->find(strJsnPtr);

			if (itr != s->_mapInlined->end())
				return itr->second;
		}

		return Node();
	}



	void Node::SchemaInfo::addValidatorSchema(Node& node)
	{
		WJEList*		pList;

		if ( (pList=validators()) || (pList=_createValidators()) )
			pList->push_back(node);
	}



	void Node::SchemaInfo::removeValidatorSchema(Node& node)
	{
		WJEList*		pList = validators();

		if (pList)
		{
			for (WJEList::iterator i = pList->begin(); i != pList->end(); i++)
			{
				if (*i == node._e)
				{
					pList->erase(i);
					break;
				}
			}
		}
	}



	void Node::SchemaInfo::discard()
	{
		if (_e)
		{
			Info* pInfo = _info();
			delete pInfo;
			_e->client = NULL;
		}
	}



	void Node::SchemaInfo::discardValidators()
	{
		if (_e)
		{
			Info* pInfo = _info();

			if (pInfo)
			{
				delete pInfo->_v;
				pInfo->_v = NULL;
			}
		}
	}



	void Node::SchemaInfo::initSchema(CachePtr pCache, URIPtr pUri)
	{
		Info* pInfo;

		// need a wjelement to store the info!!!
		assert(_e);

		if ( (pInfo=_info()) || (pInfo=_createInfo()) )
		{
			// schema info already exists!!!
			assert (!pInfo->_s);

			pInfo->_s = new Schema(pCache, pUri);
		}
	}



	void Node::SchemaInfo::initSchema(CachePtr pCache, const string& strURI)
	{
		Info* pInfo;

		// need a wjelement to store the info!!!
		assert(_e);

		if ( (pInfo=_info()) || (pInfo=_createInfo()) )
		{
			// schema info already exists!!!
			assert (!pInfo->_s);

			pInfo->_s = new Schema(pCache, strURI);
		}
	}





	/*******************************************************************
					Node Implementation
	*******************************************************************/

	/* static */
	Node Node::_loadMetaSchema(CachePtr pCache)
	{
		WJReader					reader = NULL;
		Node						schema;

		try
		{
			reader = WJROpenMemDocument((void*) G_strMetaSchema.c_str(), NULL, 0);

			if (!reader)
				throw runtime_error("JSON Schema draft-04 meta schema: unable to create reader.");

			// Parse the schema
			schema = WJEOpenDocument(reader, NULL, NULL, NULL);

			if (!schema)
				throw runtime_error("JSON Schema draft-04 could not be parsed");

			WJRCloseDocument(reader);
			reader = NULL;

			schema._validateMetaSchemaMember(pCache);
		}
		catch(...)
		{
			if (reader) WJRCloseDocument(reader);
			schema.discard();
			throw;
		}

		return schema;
	}




	void Node::_validateMetaSchemaMember(CachePtr pCache)
	{
		if (!_e) 
			throw runtime_error("_validateMetaSchemaMember message sent to nil node");

		if (!_e->parent && !isObject()) 
			throw runtime_error("The root element of the meta schema must be an object");

		switch (_e->type)
		{
			case WJR_TYPE_OBJECT:
			{
				if (_e->parent)
					_markAsSchema();
				else
					_markRootAsSchema(pCache, "http://json-schema.org/draft-04/schema");

				for (iterator itr = begin(); itr != end(); itr++)
				{
					Node child = *itr;
					child._validateMetaSchemaMember(pCache);
				}

/*
				SchemaInfo		si(*this);

				pBase

				if (_e->parent)
				{
					si.initSchema(pCache, pBaseURI);
				}
				else
				{
					si.initSchema(pCache, "http://json-schema.org/draft-04/schema");
					pBaseURI = si.uri();
				}

				// if this is a $ref mark it as a reference
				if ((*this)[SCHEMA_REF])
					si.isRef(true);

				// give _registerInlineSchema() a chance to register this
				_registerInlineSchema();
*/
				break;
			}

			case WJR_TYPE_ARRAY:
			{
				for (iterator itr = begin(); itr != end(); itr++)
				{
					Node child = *itr;
					child._validateMetaSchemaMember(pCache);
				}

				break;
			}

			default:
				break;
		}
	}



	void Node::_registerInlineSchema()
	{
		Node				self = *this;
		SchemaInfo	si(self);

		// only do this for schemas which are not at the root
		if (self && isSchema() && _e->parent)
		{
			Node			id = self["id"];

			if (id && id.isString())
			{
				si.uri()->setURI(id.getString());

				Node				root = getRoot();
				SchemaInfo	si_root(root);

				si_root.regInlineSchema(*this, si.uri()->getFullURI());
			}
		}
	}



	Node Node::_resolveRef(Node& errors)
	{
		NodeVect	refs;
		Node			ref;

		try
		{
		 ref = _resolveRef(refs, errors);
		}
		catch (std::runtime_error & e)
		{
			if (!errors)
				throw;

			errors._validationError(*this, ref, string("failed to resolve $ref, ") + e.what());

			return *this;
		}

		return ref;
	}



	Node Node::_resolveRef(NodeVect& refs, Node& errors)
	{
		Node				self = *this;
		SchemaInfo	si(self);

		if (self)
		{
			// prevent cyclic references
			for (unsigned int i = 0; i < refs.size(); i++)
			{
				if (*self == *(refs[i]))
					throw runtime_error(string("can't resolve $ref ") + refs[0][SCHEMA_REF].getString() + " because of recursions");
			}

			if (si.isRef())
			{
				refs.push_back(*this);

				Node				ref = self[SCHEMA_REF];

				if (!ref)
					throw runtime_error(asJsonPointer() + " marked as $ref but no $ref node found");

				URI					refURI(si.uri(), ref.getString());
				string			strRefBaseURI = refURI.getBaseURI();
				Node				refSchema = *this;

				// The reference could be to another schema or inlined
				if (!strRefBaseURI.empty())
					refSchema = si.cache()->loadSchema(strRefBaseURI, errors);
				else
					refSchema = refSchema._getRootSchema();

				if (!refSchema)
					throw runtime_error(string("Failed to resolve \"$ref\":\"") + refURI.getFullURI() + "\"");

				if (refURI.hasJsonPointer())
					refSchema = refSchema._getInlinedSchema(refURI);

				SchemaInfo	si_ref(refSchema);

				if (si_ref.isRef())
					return refSchema._resolveRef(refs, errors);

				return refSchema;
			}
		}

		return *this;
	}



	Node Node::_getRootSchema()
	{
		Node s = getRoot();

		if (s && s.isSchema())
			return s;

		return Node();
	}



	Node Node::_getInlinedSchema(URI& uri)
	{
		Node						schema;

		if (isSchema())
		{
			SchemaInfo			si(*this);

			if ( (schema = si.getInlineSchema(uri.getFullURI())) )
				return schema;

			// if the uri is a JSON pointer traverse this
			string					baseURI = uri.getBaseURI();

			if (uri.hasJsonPointer() && (baseURI.empty() || si.uri()->getBaseURI() == baseURI))
			{
				StringVect	vect = uri.getJsonPointerVect();
				Node				curr = *this;

				for (unsigned int i = 0; i < vect.size(); i++)
				{
					try
					{
						curr = curr[vect[i]];
					}
					catch(...)
					{
						return NULL;
					}
				}

				if (curr)
					return curr;
			}

			return Node();			
		}

		return *this;
	}



	/* static */
	Node Node::_createEmptySchema(CachePtr pCache)
	{
		Node		empty = Node::parseJson("{}");
		Node		errors = Node::createArray();

		if (empty && pCache->getMetaSchema().validateInstance(empty, errors))
		{
			errors.discard();
			return empty;
		}

		empty.discard();
		errors.discard();

		return Node();
	}



	bool Node::validateInstance(Node& node, Node& errors, const string& strURI)
	{
		if (!isSchema())
			throw runtime_error("validateInstance message sent to non schema node");

		// this maybe a reference so resolve it
		Node				ref = _resolveRef(errors);
		SchemaInfo	si_schema(*this);

		if (ref.isMetaSchema())
			if (!node._markRootAsSchema(si_schema.cache(), strURI))
				return false;

		return _validateInstance(node, errors, true);
	}



	bool Node::_validateInstance(Node& node, Node& errors, bool log)
	{
		Node				ref = _resolveRef(errors);
		bool				fail = false;
		Node				validator;

		// if this' type is worng lets forget about it
		if (!ref._validateType(node, errors, log))
			return false;

		switch (node.getType())
		{
			case WJR_TYPE_OBJECT:
				fail = !ref._validateObject(node, errors, log) ? true : fail;
				break;

			case WJR_TYPE_ARRAY:
				fail = !ref._validateArray(node, errors, log) ? true : fail;
				break;

			case WJR_TYPE_INTEGER:
			case WJR_TYPE_NUMBER:
				fail = !ref._validateNumber(node, errors, log) ? true : fail;
				break;

			case WJR_TYPE_STRING:
				fail = !ref._validateString(node, errors, log) ? true : fail;
				break;

			case WJR_TYPE_BOOL:
			case WJR_TYPE_TRUE:
			case WJR_TYPE_FALSE:
			case WJR_TYPE_NULL:
			case WJR_TYPE_UNKNOWN:
				// nothing to do here
				break;
		}

		// validate against allOf
		validator = ref["allOf"];

		if (validator)
		{
			for (iterator i = validator.begin(); i != validator.end(); i++)
			{
				Node		schema = *i;

				if (!schema._validateInstance(node, errors, log))
				{
					if (log)
						errors._validationError(schema, node, "instance fails 'allOf' validation");

					fail = true;
				}
				else
				{
					SchemaInfo	si(node);
					si.addValidatorSchema(schema);
				}
			}
		}

		// validate against anyOf
		validator = ref["anyOf"];

		if (validator)
		{
			bool	valid = false;

			for (iterator i = validator.begin(); i != validator.end(); i++)
			{
				Node		schema = *i;

				if (schema._validateInstance(node, errors, false))
				{
					SchemaInfo	si(node);
					si.addValidatorSchema(schema);
					valid = true;
				}
			}

			if (!valid)
			{
				if (log)
					errors._validationError(*this, node, "instance fails 'anyOf' validation");

				fail = true;
			}
		}

		// validate against oneOf
		validator = ref["oneOf"];

		if (validator)
		{
			int	matches = 0;

			for (iterator i = validator.begin(); i != validator.end(); i++)
			{
				Node		schema = *i;

				if (schema._validateInstance(node, errors, false))
				{
					matches++;

					if (matches > 1)
						break;

					SchemaInfo	si(node);
					si.addValidatorSchema(schema);
				}
			}

			if (matches != 1)
			{
				if (log)
					errors._validationError(*this, node, "instance fails 'oneOf' validation");

				fail = true;
			}
		}

		// validate against not
		validator = ref["not"];

		if (validator)
		{
			if (validator._validateInstance(node, errors, false))
			{
				if (log)
					errors._validationError(*this, node, "instance fails 'not' validation");

				fail = true;
			}
		}

		// validate against enum
		validator = ref["enum"];

		if (validator)
		{
			bool valid = false;

			for (iterator i = validator.begin(); i != validator.end(); i++)
			{
				Node		value = *i;

				if (node == value)
				{
					valid = true;
					break;
				}
			}

			if (!valid)
			{
				if (log)
					errors._validationError(*this, node, "instance fails 'enum' validation");

				fail = true;
			}
		}

		return !fail;
	}



	bool Node::_validateNumber(Node& node, Node& errors, bool log)
	{
		bool				fail = false;
		Node&				self = *this;
		SchemaInfo	si(self);

		assert(self && node);

		if (node.isNumber() || node.isInteger())
		{
			Node					schemaProperty;
			double				multipleOf, maximum=0.0, minimum=0.0;
			bool					exclusiveMinimum=false, exclusiveMaximum=false, hasMin = false, hasMax = false;

			schemaProperty = self["multipleOf"];
			multipleOf = schemaProperty ? schemaProperty.getNum() : 0;

			schemaProperty = self["minimum"];
			if (schemaProperty)
			{
				hasMin = true;
				minimum = schemaProperty.getNum();
				schemaProperty = self["exclusiveMinimum"];
				exclusiveMinimum = schemaProperty ? schemaProperty.getBool() : false;
			}

			schemaProperty = (*this)["maximum"];
			if (schemaProperty)
			{
				hasMax = true;
				maximum = schemaProperty.getNum();
				schemaProperty = self["exclusiveMaximum"];
				exclusiveMaximum = schemaProperty ? schemaProperty.getBool() : false;
			}

			if (multipleOf > 0.0)
			{
				double		val = node.getNum();

				if (fmod(val, multipleOf) != 0.0)
				{
					if (log)
					{
						ostringstream ostrm;
						ostrm << "instance is not divisible by " << multipleOf;
						errors._validationError(*this, node, ostrm.str());
					}

					fail = true;
				}
			}

			if (hasMin)
			{
				if (exclusiveMinimum)
				{
					if (node.getNum() <= minimum)
					{
						if (log)
						{
							ostringstream ostrm;
							ostrm << "instance must be greater than " << minimum;
							errors._validationError(*this, node, ostrm.str());
						}

						fail = true;
					}
				}
				else
				{
					if (node.getNum() < minimum)
					{
						if (log)
						{
							ostringstream ostrm;
							ostrm << "instance must be greater than or equal to " << minimum;
							errors._validationError(*this, node, ostrm.str());
						}

						fail = true;
					}
				}
			}

			if (hasMax)
			{
				if (exclusiveMaximum)
				{
					if (node.getNum() >= maximum)
					{
						if (log)
						{
							ostringstream ostrm;
							ostrm << "instance must be less than " << maximum;
							errors._validationError(*this, node, ostrm.str());
						}

						fail = true;
					}
				}
				else
				{
					if (node.getNum() > maximum)
					{
						if (log)
						{
							ostringstream ostrm;
							ostrm << "instance must be less than or equal to " << maximum;
							errors._validationError(*this, node, ostrm.str());
						}

						fail = true;
					}
				}
			}
		}

		return !fail;
	}



	bool Node::_validateString(Node& node, Node& errors, bool log)
	{
		Node&			self = *this;
		bool			fail = false;

		assert(self && node);

		if (node.isString())
		{
			Node						schemaProperty;
			int							minLength, maxLength;
			string					pattern;

			minLength = (schemaProperty = self["minLength"]) ? schemaProperty.getInt()    : 0;
			maxLength = (schemaProperty = self["maxLength"]) ? schemaProperty.getInt()    : 0;
			pattern   = (schemaProperty = self["pattern"])   ? schemaProperty.getString() : "";

			if (minLength && node.getString().size() < (unsigned) minLength)
			{
				if (log)
				{
					ostringstream ostrm;
					ostrm << "instance is shorter than " << minLength;
					errors._validationError(*this, node, ostrm.str());
				}

				fail = true;
			}

			if (maxLength && node.getString().size() > (unsigned) maxLength)
			{
				if (log)
				{
					ostringstream ostrm;
					ostrm << "instance is longer than " << maxLength;
					errors._validationError(*this, node, ostrm.str());
				}

				fail = true;
			}

			if (!pattern.empty())
			{
				try
				{
					if (!boost::regex_match(node.getString(), boost::regex(pattern, boost::regex_constants::ECMAScript)))
					{
						if (log)
							errors._validationError(*this, node, "pattern doesn't match");
							
						fail = true;
					}
				}
				catch (boost::regex_error & b)
				{
					if (log)
					{
						ostringstream ostrm;
						ostrm << "schema pattern '" << pattern << "' is not a valid ECMAScript pattern, " << b.what();
						errors._validationError(*this, node, ostrm.str());
					}

					fail = true;
				}
				catch (runtime_error & s)
				{
					if (log)
					{
						ostringstream ostrm;
						ostrm << "pattern doesn't match, " << s.what();
						errors._validationError(*this, node, ostrm.str());
					}

					fail = true;
				}
			}
		}

		return !fail;
	}



	bool Node::_validateArray(Node& node, Node& errors, bool log)
	{
		Node&				self = *this;
		SchemaInfo	si(self);
		bool				fail = false;

		assert(self && node);

		if (node.isArray())
		{
			Node					schemaProperty;
			Node					additionalItems = self["additionalItems"];
			Node					items = self["items"];
			int						minItems, maxItems;
			bool					uniqueItems;

			minItems =    (schemaProperty = self["minItems"])    ? schemaProperty.getInt()  : 0;
			maxItems =    (schemaProperty = self["maxItems"])    ? schemaProperty.getInt()  : 0;
			uniqueItems = (schemaProperty = self["uniqueItems"]) ? schemaProperty.getBool() : false;

			if (!items && si.cache())
				items = si.cache()->getEmptySchema();

			// Member Validation
			if (items.isObject())
			{
				assert (items && items.isSchema());

				// validate each item of the instance
				for (Node::iterator i = node.begin(); i != node.end(); i++)
				{
					Node	item = *i;

					if (!items._validateInstance(item, errors, log))
						fail = true;
				}
			}
			else if (items.isArray())
			{
				Node::iterator		j;
				iterator					i;

				// validate items in array instance against the the schema in the schema's items array
				for (i = items.begin(), j = node.begin(); i != items.end() && j != node.end(); i++, j++)
				{
					Node		schema = *i;
					Node		item = *j;

					if (!schema._validateInstance(item, errors, log))
					{
						if (log)
							errors._validationError(*this, node, "instance fails 'items' validation");

						fail = true;
					}
				}
			}

			// check additionalItems
			if (additionalItems)
			{
				if (additionalItems.isBoolean())
				{
					if (items && items.isArray() && additionalItems.getBool() == false)
					{
						if (node.size() > items.size())
						{
							if (log)
								errors._validationError(*this, node, "schema does not allow extra items");

							fail = true;
						}
					}
				}
				else if ((!items || items.isArray()) && additionalItems.isObject())
				{
					// validate any extra items against the additionalItems schema
					iterator i = items.begin();

					for (Node::iterator j = node.begin(); j != node.end(); j++)
					{
						if (i == items.end())
						{
							Node	item = *j;
							if (!additionalItems._validateInstance(item, errors, log))
							{
								if (log)
									errors._validationError(*this, node, "instance fails 'additionalItems' validation");

								fail = true;
							}
						}
						else
						{
							i++;
						}
					}
				}
			}

			if (minItems)
			{
				if (node.size() < (unsigned) minItems)
				{
					if (log)
					{
						ostringstream ostrm;
						ostrm << "instance has less than the minimum of " << minItems << " items";
						errors._validationError(*this, node, ostrm.str());
					}

					fail = true;
				}
			}

			if (maxItems)
			{
				if (node.size() > (unsigned) maxItems)
				{
					if (log)
					{
						ostringstream ostrm;
						ostrm << "instance has more than the maximum of " << maxItems << " items";
						errors._validationError(*this, node, ostrm.str());
					}

					fail = true;
				}
			}

			if (uniqueItems)
			{
				Node::iterator	i, j;
				bool						check;

				for (i = node.begin(); i != node.end(); i++)
				{
					Node		lhs = *i;

					check = false;

					for (j = node.begin(); j != node.end(); j++)
					{
						Node		rhs = *j;

						if (check)
						{
							if (lhs != rhs)
								continue;

							if (log)
							{
								ostringstream ostrm;
								ostrm << "array values fail 'unique' validation";
								errors._validationError(*this, node, ostrm.str());
							}

							fail = true;
							break;
						}

						if (lhs._e == rhs._e)
							check = true;
					}
				}
			}
		}

		return !fail;
	}



	bool Node::_validateObject(Node& node, Node& errors, bool log)
	{
		Node&			self = *this;
		bool			fail = false;

		assert(self && this);

		if (node.isObject())
		{
			if (node._getRootSchema())
				node._markAsSchema();

			Node					schemaProperty;
			Node					additionalProperties =	self["additionalProperties"];
			Node					properties =						self["properties"];
			Node					patternProperties =			self["patternProperties"];
			Node					required =							self["required"];
			Node					dependencies =					self["dependencies"];
			int						minProperties, maxProperties;

			minProperties = (schemaProperty = self["minProperties"]) ? schemaProperty.getInt()    : 0;
			maxProperties = (schemaProperty = self["maxProperties"]) ? schemaProperty.getInt()    : 0;

			// Member Validation
			for (Node::iterator i = node.begin(); i != node.end(); i++)
			{
				NodeVect					schemas;
				Node							item = *i;

				// add schema from schema's properties to the ones to validate
				if (properties && properties.isObject())
				{
					Node	schema = properties[item.getName()];
					
					if (schema)
						schemas.push_back(schema);
				}

				// add schema from schema's properties to the ones to validate
				if (patternProperties && patternProperties.isObject())
				{
					for (iterator	i = patternProperties.begin(); i != patternProperties.end(); i++)
					{
						Node			schema = *i;

						if (boost::regex_search(item.getName(), boost::regex(schema.getName(), boost::regex_constants::ECMAScript)))
							schemas.push_back(schema);
					}
				}
				
				// if we haven't got any schemas for this member, add the additional properties schema
				if (schemas.empty() && additionalProperties && additionalProperties.isObject())
					schemas.push_back(additionalProperties);

				// validate the instance against its schemas
				for (unsigned int j = 0; j < schemas.size(); j++)
				{
					Node schema = schemas[j];

					if (!schema._validateInstance(item, errors, log))
						fail = true;
				}
			}

			// Container validation
			if (additionalProperties)
			{
				if (additionalProperties.isBoolean())
				{
					if (additionalProperties.getBool() == false)
					{
						StringVect		members = node.getMembers();

						// remove all elements from members defined in properties
						if (properties && properties.isObject())
						{
							for (iterator	i = properties.begin(); i != properties.end(); i++)
							{
								for (StringVectItr j = members.begin(); j != members.end(); j++)
								{
									if ((*i).getName() == *j)
									{
										members.erase(j);
										break;
									}
								}
							}
						}
						
						// remove all elements from members defined in patterProperties
						if (patternProperties && patternProperties.isObject())
						{
							for (iterator	i = patternProperties.begin(); i != patternProperties.end(); i++)
							{
								Node		pattern = *i;

								boost::regex		rgx(pattern.getName(), boost::regex_constants::ECMAScript);

								for (StringVectItr j = members.begin(); j != members.end();)
								{
									if (boost::regex_match(*j, rgx))
										j = members.erase(j);
									else
										j++;
								}
							}
						}

						if (!members.empty())
						{
							if (log)
							{
								string								strMembers;
								StringVect::iterator	itr;

								for (itr = members.begin(); itr != members.end(); itr++)
								{
									if (itr != members.begin())
										strMembers += ", ";

									strMembers += *itr;
								}

								errors._validationError(*this, node, string("member(s) ") + strMembers + " fail 'properties', 'patternProperties' or 'additionalProperties' validation");
							}

							fail = true;
						}
					}
				}
			}
			
			// check required
			if (required)
			{
				assert(required.isArray());
				
				for (iterator i = required.begin(); i != required.end(); i++)
				{
					Node			rqrd = *i;
					string		str = rqrd.getString();

					if (!node[str])
					{
						if (log)
						{
							ostringstream ostrm;
							ostrm << "instance 'requires' member " << str;
							errors._validationError(*this, node, ostrm.str());
						}

						fail = true;
					}
				}
			}

			// check dependencies
			if (dependencies)
			{
				for (iterator i = dependencies.begin(); i != dependencies.end(); i++)
				{
					Node 	dependency = *i;
					Node 	dependent = node[dependency.getName()];

					if (dependent)
					{
						if (dependency.isArray())
						{
							for (iterator j = dependency.begin(); j != dependency.end(); j++)
							{
								Node		name = *j;
								
								if (!node.isMember(name.getString()))
								{
									if (log)
									{
										ostringstream ostrm;
										ostrm << "instance property " << dependency.getName() << " depends on missing property " << name.getString();
										errors._validationError(*this, node, ostrm.str());
									}

									fail = true;
								}
							}
						}
						else if (dependency.isObject())
						{
							if (!dependency._validateInstance(dependent, errors, log))
								fail = true;
						}
					}
				}
			}
			
			if (minProperties)
			{
				if (node.size() < (unsigned) minProperties)
				{
					if (log)
					{
						ostringstream ostrm;
						ostrm << "instance has less than the minimum of " << minProperties << " properties";
						errors._validationError(*this, node, ostrm.str());
					}

					fail = true;
				}
			}

			if (maxProperties)
			{
				if (node.size() > (unsigned) maxProperties)
				{
					if (log)
					{
						ostringstream ostrm;
						ostrm << "instance has more than the maximum of " << maxProperties << " properties";
						errors._validationError(*this, node, ostrm.str());
					}

					fail = true;
				}
			}
		}

		return !fail;
	}



	bool Node::_validateSingleType(Node& node, const string& type)
	{
		switch (node.getType())
		{
			case WJR_TYPE_OBJECT:		return (type == "object");
			case WJR_TYPE_ARRAY:		return (type == "array");
			case WJR_TYPE_INTEGER:	return (type == "integer" || type == "number");
			case WJR_TYPE_NUMBER: 	return (type == "number");
			case WJR_TYPE_STRING:		return (type == "string");
			case WJR_TYPE_BOOL:			return (type == "boolean");
			case WJR_TYPE_TRUE:			return (type == "boolean");
			case WJR_TYPE_FALSE:		return (type == "boolean");
			case WJR_TYPE_NULL:			return (type == "null");
			case WJR_TYPE_UNKNOWN:	return (type == "unknown");
		}

		return true;
	}



	bool Node::_validateType(Node& node, Node& errors, bool log)
	{
		Node			type = (*this)["type"];
		bool			success = true;

		if (type)
		{
			string		strT, strTypes;

			if (type.isString())
			{
				strT = type.getString();
				strTypes = " " + strT;
				success = _validateSingleType(node, strT);
			} 
			else if (isArray())
			{
				strTypes = " one of";
				for (iterator i = begin(); i != end(); i++)
				{
					strT = (*i).getString();

					if ((success = _validateSingleType(node, strT)))
						break;

					strTypes += " " + strT;
				}
			}

			if (!success && log)
			{
				ostringstream		ostrm;
				ostrm << "expecting type" + strTypes + " but found type " << node.getTypeAsString(); 
				errors._validationError(*this, node, ostrm.str());
			}
		}

		return success;
	}



	void Node::setName(const string& name)
	{
		if (_e)
			WJERename(_e, name.c_str());
	}



	WJElement Node::_addChild(const string& name)
	{
		Cheat u;

		if (!_e)
			throw runtime_error("can't add node to NULL");
			
		switch (_e->type)
		{
			case WJR_TYPE_ARRAY:
				break;
					
			case WJR_TYPE_OBJECT:
				if (isMember(name))
					throw runtime_error("a member with this name already exists");
				break;

			default:
				throw runtime_error("can only add node to array or object");
		}

		u.cp = name.c_str();
		
		WJElement e = WJEChild(_e, u.c, WJE_NEW);
		
		if (!e)
			throw runtime_error("failed to create node");

		return e;
	}



	void Node::_validationError(Node& schema, Node& instance, const std::string& strMessage)
	{
		if (!_e)
			_e = Node::createArray()._e;

		if (_e && _e->type == WJR_TYPE_ARRAY)
		{
			Node			error;
			string		strJPSchema;
			string		strJPInstance;
			bool			add = true;

			if (schema)
				strJPSchema = schema.asJsonPointer();

			if (instance)
				strJPInstance = instance.asJsonPointer(true);

			for (iterator i = begin(); i != end(); i++)
			{
				error = *i;

				if (error["schema"].getString() == strJPSchema &&
						error["instance"].getString() == strJPInstance &&
						error["message"].getString() == strMessage)
				{
					add = false;
					break;
				}
			}

			if (add)
			{
				error = addObject("");
				error.addString("schema", strJPSchema);
				error.addString("instance", strJPInstance);
				error.addString("message", strMessage);
			}
		}
	}



	/* static */
	Node Node::createObject()
	{
		return WJEObject(NULL, NULL, WJE_NEW);
	}



	/* static */
	Node Node::createArray()
	{
		return WJEArray(NULL, NULL, WJE_NEW);
	}



	Node Node::addObject(const string& name)
	{
		WJElement e = _addChild(name);
		return WJEObject(e, NULL, WJE_MOD);
	}



	Node Node::addArray(const string& name)
	{
		WJElement e = _addChild(name);
		return WJEArray(e, NULL, WJE_MOD);
	}



	Node Node::addNull(const string& name)
	{
		WJElement e = _addChild(name);
		return WJENull(e, NULL, WJE_MOD);
	}



	Node Node::addString(const string& name, const string& value)
	{
		Cheat u;
		u.cp = value.c_str();
		WJElement e = _addChild(name);
		WJEString(e, NULL, WJE_MOD, u.c);
		return e;
	}



	Node Node::addBool(const string& name, bool value)
	{
		WJElement e = _addChild(name);
		WJEBool(e, NULL, WJE_MOD, value ? 1 : 0);
		return e;
	}



	Node Node::addInt32(const string& name, int32 value)
	{
		WJElement e = _addChild(name);
		WJEInt32(e, NULL, WJE_MOD, value);
		return e;
	}



	Node Node::addUInt32(const string& name, uint32 value)
	{
		WJElement e = _addChild(name);
		WJEUInt32(e, NULL, WJE_MOD, value);
		return e;
	}



	Node Node::addInt64(const string& name, int64 value)
	{
		WJElement e = _addChild(name);
		WJEInt64(e, NULL, WJE_MOD, value);
		return e;
	}



	Node Node::addUInt64(const string& name, uint64 value)
	{
		WJElement e = _addChild(name);
		WJEUInt64(e, NULL, WJE_MOD, value);
		return e;
	}



	Node Node::addDouble(const string& name, double value)
	{
		WJElement e = _addChild(name);
		WJEDouble(e, NULL, WJE_MOD, value);
		return e;
	}



	bool Node::getBool()											
	{ 
		if (!isBoolean())
			throw std::runtime_error(asJsonPointer() + " doesn't understand getBool()");
		
		return WJEBool(_e, (char*) ".", WJE_GET, NULL) == 1 ? true : false; 
	}



	int32 Node::getInt32()
	{ 
		if (!isInteger())
			throw std::runtime_error(asJsonPointer() + " doesn't understand getInt32()");
		
		return WJEInt32(_e, (char*) ".", WJE_GET, NULL); 
	}



	uint32 Node::getUInt32()
	{ 
		if (!isInteger())
			throw std::runtime_error(asJsonPointer() + " doesn't understand getUInt32()");
		
		return WJEUInt32(_e, (char*) ".", WJE_GET, NULL); 
	}



	int64 Node::getInt64()
	{ 
		if (!isInteger())
			throw std::runtime_error(asJsonPointer() + " doesn't understand getInt64()");
		
		return WJEInt64(_e, (char*) ".", WJE_GET, NULL); 
	}



	uint64 Node::getUInt64()
	{ 
		if (!isInteger())
			throw std::runtime_error(asJsonPointer() + " doesn't understand getUInt64()");
		
		return WJEUInt64(_e, (char*) ".", WJE_GET, NULL); 
	}



	double Node::getNum()
	{ 
		if (!isNumber() && !isInteger())
			throw std::runtime_error(asJsonPointer() + " doesn't understand getNum()");

		if (isInteger())
			return (double) WJEInt64(_e, (char*) ".", WJE_GET, NULL);

		return WJEDouble(_e, (char*) ".", WJE_GET, NULL); 
	}



	string Node::getString() 
	{ 
		if (!isString())
			throw std::runtime_error(asJsonPointer() + " doesn't understand getString()");
		
		return WJEString(_e, (char*) ".", WJE_GET, NULL); 
	}



	void Node::setNull()
	{
		if (!_e || !WJENull(_e, NULL, WJE_MOD))
			throw runtime_error(asJsonPointer() + " failed to set null value");
	}



	void Node::setString(const string& value)
	{
		Cheat u;
		u.cp = value.c_str();
		if (!_e || !WJEString(_e, NULL, WJE_MOD, u.c))
			throw runtime_error(asJsonPointer() + " failed to set string value");
	}



	void Node::setBool(bool value)
	{
		if (!_e || WJEBool(_e, NULL, WJE_MOD, value ? 1 : 0) != (value ? 1 : 0))
			throw runtime_error(asJsonPointer() + " failed to set bool value");
	}



	void Node::setInt32(int32 value)
	{
		if (!_e || WJEInt32(_e, NULL, WJE_MOD, value) != value)
			throw runtime_error(asJsonPointer() + " failed to set int32 value");
	}



	void Node::setUInt32(uint32 value)
	{
		if (!_e || WJEUInt32(_e, NULL, WJE_MOD, value) != value)
			throw runtime_error(asJsonPointer() + " failed to set uint32 value");
	}



	void Node::setInt64(int64 value)
	{
		if (!_e || WJEInt64(_e, NULL, WJE_MOD, value) != value)
			throw runtime_error(asJsonPointer() + " failed to set int64 value");
	}



	void Node::setUInt64(uint64 value)
	{
		if (!_e || WJEUInt64(_e, NULL, WJE_MOD, value) != value)
			throw runtime_error(asJsonPointer() + " failed to set uint64 value");
	}



	void Node::setDouble(double value)
	{
		if (!_e || WJEDouble(_e, NULL, WJE_MOD, value) != value)
			throw runtime_error(asJsonPointer() + " failed to set double value");
	}






	/*******************************************************************
					Cache Implementation
	*******************************************************************/
		
	/* static */
	Cache& Cache::GetCache(SchemaLoaderFunc fnLoader)
	{
		static Cache	g_Cache(fnLoader);
		return g_Cache;
	}



	Cache::~Cache()
	{
		while (!m_mapSchema.empty())
		{
			Node	schema = m_mapSchema.begin()->second;

			if (*schema != *m_metaSchema && *schema != *m_emptySchema)
				schema.discard();
			else
				m_mapSchema.erase(m_mapSchema.begin());
		}

		m_emptySchema.discard();
		m_metaSchema.discard();
	}



	void Cache::_onSchemaCreated(Node& schema, URIPtr pURI)
	{
		if (schema.isRootSchema() && pURI)
		{
			string		strBaseURI = pURI->getBaseURI();

			if (!strBaseURI.empty())
			{
				NodeMapItr	itr = m_mapSchema.find(strBaseURI);

				if (itr != m_mapSchema.end())
				{
					throw runtime_error(string("Cannot add schema ") + schema.asJsonPointer() + " to cache because a schema with id " + strBaseURI + " already exists ");
					return;
				}

				m_mapSchema[strBaseURI] = schema;
			}
		}
	};



	void Cache::_onSchemaDeleted(Node& schema, URIPtr pURI)
	{
		if (schema.isRootSchema() && pURI)
		{
			string		strBaseURI = pURI->getBaseURI();

			if (!strBaseURI.empty())
			{
				NodeMapItr	itr = m_mapSchema.find(strBaseURI);

				if (itr == m_mapSchema.end())
					return;

				m_mapSchema.erase(itr);
			}
		}
	};



	void Cache::_initialize()
	{
		m_metaSchema  = Node::_loadMetaSchema(this);
		m_emptySchema = Node::_createEmptySchema(this);
	}




	Node Cache::loadSchema(const string& strURI, Node& errors)
	{
		NodeMapItr		itr = m_mapSchema.find(URI(strURI).getBaseURI());

		if (itr != m_mapSchema.end())
			return itr->second;

		if (m_metaSchema && m_fnLoader)
		{
			Node				schema;

			try
			{
				string			rsrce = m_fnLoader(strURI);

				if (rsrce.empty())
					throw runtime_error(string("failed to load URI ") + strURI);

				schema = Node::parseJson(rsrce);

				if (!schema)
					throw runtime_error(string("failed parsing JSON resource from URI ") + strURI);

				m_metaSchema.validateInstance(schema, errors, strURI);

				return schema;
			}
			catch(...)
			{
				schema.discard();
				throw;
			}
		}

		return Node();
	}




	bool	Cache::loadSchema(Node& schema, Node& errors)
	{
		if (m_metaSchema && schema)
			return m_metaSchema.validateInstance(schema, errors);

		return false;
	}





	/*******************************************************************
					Cache::IDGenerator Implementation
	*******************************************************************/
		
	string Cache::IDGenerator::getID()
	{
		static Cache::IDGenerator	S_idGenerator;

		string			strID = S_idGenerator.m_strID;
		string&			currID = S_idGenerator.m_strID;

		// Generate the next id
		string::size_type		pos = currID.length() - 1;

		while (true)
		{
			if (currID[pos] < 'a' || currID[pos] > 'z')
			{
				currID += "a";
				break;
			}

			if (currID[pos] == 'z')
			{
				currID[pos] = 'a';
				pos--;
				continue;
			}

			currID[pos] = currID[pos] + 1;
			break;
		}

		return strID;
	}


} /* namespace WJPP */

