#ifndef _OutboundHttpRequest_h_
#define _OutboundHttpRequest_h_

#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <boost/regex.hpp>

using namespace::std;

namespace TWO
{
	namespace HTTP
	{
		typedef map<string, string>							MapHeader;
		typedef map<string, string>::iterator		MapHeaderItr;

		class OutboundHttpRequest
		{
		public:
			enum Method
			{
				OHR_UNDEFINED,
				OHR_GET,
				OHR_PUT,
				OHR_POST,
				OHR_DELETE
			};

		protected:
			MapHeader				m_mapRequestHeader;
			MapHeader				m_mapResponseHeader;
			Method					m_eMethod;
			string					m_strURL;
			istream*				m_pStrmBody;

		public:
			OutboundHttpRequest() : m_eMethod(OHR_UNDEFINED) {}

			void			initialise(const string& strURL, Method eMethod, istream* pStrmBody = NULL);

			string		sendRequest();

			void			clearHeaders()																						{ clearRequestHeader(); clearResponseHeader(); }
			void			clearRequestHeader()																			{ m_mapRequestHeader.clear(); }
			void			clearResponseHeader()																			{ m_mapResponseHeader.clear(); }

			void			setRequestHeader(const string& key, const string& value)	{ setHeaderValue(m_mapRequestHeader, key, value); }
			string		getRequestHeader(const string& key)												{ return getHeaderValue(m_mapRequestHeader, key); }

			void			setResponseHeader(const string& key, const string& value)	{ setHeaderValue(m_mapResponseHeader, key, value); }
			string		getResponseHeader(const string& key)											{ return getHeaderValue(m_mapResponseHeader, key); }

		protected:
			void			setHeaderValue(MapHeader & mapHeader, const string& key, const string& value);
			string		getHeaderValue(MapHeader & mapHeader, const string& key);
		};



		//! This class simply sets initialises the request object for json schema compatible requests
		class JSONHttpRequest : public OutboundHttpRequest
		{
		public:
			JSONHttpRequest()
			{
				setRequestHeader("Accept", "application/json");  
				setRequestHeader("Accept-Charset", "utf-8");  
				setRequestHeader("Accept-Encoding", "gzip,deflate");
			}

			string		sendRequest(const string& strURL, Method eMethod = OHR_GET, istream* pStrmBody = NULL)
			{
				initialise(strURL, eMethod, pStrmBody);
				return OutboundHttpRequest::sendRequest();
			}
		};
	} /* namespace HTTP */
} /* namespace TWO */


#endif /* _OutboundHttpRequest_h_ */
