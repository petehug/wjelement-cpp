#ifndef _OutboundHttpRequest_h_
#define _OutboundHttpRequest_h_

#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <boost/regex.hpp>

namespace TWO
{
	namespace HTTP
	{
		typedef std::map<std::string, std::string>							MapHeader;
		typedef std::map<std::string, std::string>::iterator		MapHeaderItr;

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
			std::string			m_strURL;
			std::istream*		m_pStrmBody;

		public:
			OutboundHttpRequest() : m_eMethod(OHR_UNDEFINED) {}

			void					initialise(const std::string& strURL, Method eMethod, std::istream* pStrmBody = NULL);

			std::string		sendRequest();

			void					clearHeaders()																											{ clearRequestHeader(); clearResponseHeader(); }
			void					clearRequestHeader()																								{ m_mapRequestHeader.clear(); }
			void					clearResponseHeader()																								{ m_mapResponseHeader.clear(); }

			void					setRequestHeader(const std::string& key, const std::string& value)	{ setHeaderValue(m_mapRequestHeader, key, value); }
			std::string		getRequestHeader(const std::string& key)														{ return getHeaderValue(m_mapRequestHeader, key); }

			void					setResponseHeader(const std::string& key, const std::string& value)	{ setHeaderValue(m_mapResponseHeader, key, value); }
			std::string		getResponseHeader(const std::string& key)														{ return getHeaderValue(m_mapResponseHeader, key); }

		protected:
			void					setHeaderValue(MapHeader & mapHeader, const std::string& key, const std::string& value);
			std::string		getHeaderValue(MapHeader & mapHeader, const std::string& key);
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

			std::string		sendRequest(const std::string& strURL, Method eMethod = OHR_GET, std::istream* pStrmBody = NULL)
			{
				initialise(strURL, eMethod, pStrmBody);
				return OutboundHttpRequest::sendRequest();
			}
		};
	} /* namespace HTTP */
} /* namespace TWO */


#endif /* _OutboundHttpRequest_h_ */
