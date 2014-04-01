#include "OutboundHttpRequest.h"

namespace TWO
{
	namespace HTTP
	{
		static size_t header_callback(void *ptr, size_t size, size_t nmemb, void *mapHdr)
		{
			MapHeader&								mapHeader = *((MapHeader*) mapHdr);
			std::string								key, value, tmp;
			std::string::size_type		pos;

			tmp = (char*) ptr;

			// hit the end of header?
			if (tmp.size() == 2 && tmp[0] == '\r' && tmp[1] == '\n')
				return 2;

			pos = tmp.find(':');

			if (pos != std::string::npos)
			{
				key = tmp.substr(0, pos);
				pos++;
				while (tmp[pos] == ' ') pos++;
				value = tmp.substr(pos);
			}
			else
			{
				key = tmp;
				value = "";
			}

			if (!key.empty())
				mapHeader[key] = value;

			return (curl_off_t)strlen((char*) ptr) / size;
		}



		static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
		{
			std::istream&							strm = *((std::istream*) stream);
			size_t										retcode;
			curl_off_t								nread;
			char*											pout = (char*)ptr;
			int												i=0;

			while ((size_t)(pout - (char*) ptr) < (nmemb * size))
			{
				if (strm.eof()) break;

				strm >> *pout; 
				i++;

				pout++;
			}

			retcode = (pout - (char*) ptr) / size;

			nread = (curl_off_t)retcode;

			return retcode;
		}



		static size_t write_callback( char *ptr, size_t size, size_t nmemb, void *stream)
		{
			std::ostringstream &		strm(*((std::ostringstream*) stream));

			if (true)
			{
				size_t	nLength = nmemb * size;
				char *	p = ptr;		
					
				while (nLength > 0)
				{
					strm << *p++;
					nLength--;
				}
			}

			return nmemb;
		}



		void OutboundHttpRequest::setHeaderValue(MapHeader & mapHeader, const std::string& key, const std::string& value)
		{
			mapHeader[key] = value;
		}



		std::string OutboundHttpRequest::getHeaderValue(MapHeader & mapHeader, const std::string& key)
		{
			MapHeaderItr		itr = mapHeader.find(key);

			if (itr != mapHeader.end())
				return itr->second;

			return "";
		}



		void OutboundHttpRequest::initialise(const std::string& strURL, Method eMethod, std::istream* pStrmBody)
		{
			// clear the response header
			clearResponseHeader();
			m_eMethod = eMethod;
			m_strURL = strURL;
			m_pStrmBody = pStrmBody;
		}



		std::string OutboundHttpRequest::sendRequest()
		{   
			CURL*										curl = NULL;
			std::ostringstream			strm;
			std::ostringstream			strrqst;
			MapHeaderItr						itr;
			curl_slist*							headers=NULL; // init to NULL is important 


			try
			{
				strrqst << m_strURL;

				switch (m_eMethod)
				{
					case OHR_GET:
						strrqst << "(GET)";
						break;
					case OHR_PUT:
						strrqst << "(PUT)";
						break;
					case OHR_POST:
						strrqst << "(POST)";
						break;
				}

				// sanity checks
				if (m_eMethod == OHR_UNDEFINED || m_strURL.empty())
					throw std::runtime_error("OutboundHttpRequest requires valid URL and HTTP/1.1 mode");

				curl = curl_easy_init();

				if (!curl) 
					throw std::runtime_error("unable to initialize curl");

				if (!m_mapRequestHeader.empty())
				{
					for ( itr =  m_mapRequestHeader.begin();
								itr != m_mapRequestHeader.end();
								itr++)
					{
						headers = curl_slist_append(headers, (std::string(itr->first) + ": " + itr->second).c_str());  
					}

					curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
				}

				curl_easy_setopt(curl, CURLOPT_URL, m_strURL.c_str());

				switch (m_eMethod)
				{
					case OHR_GET:
						curl_easy_setopt(curl, CURLOPT_HTTPGET, 1); 
						break;
					case OHR_PUT:
						curl_easy_setopt(curl, CURLOPT_PUT, 1); 
						break;
					case OHR_POST:
						curl_easy_setopt(curl, CURLOPT_POST, 1); 
						break;
					default:
						throw std::runtime_error("invalid HTTP method");
				}

				if (m_pStrmBody)
				{
					curl_easy_setopt(curl, CURLOPT_READDATA, m_pStrmBody);
					curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
				}

				curl_easy_setopt(curl, CURLOPT_HEADERDATA, &m_mapResponseHeader);
				curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);

				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strm);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

				if (curl_easy_perform(curl) != CURLE_OK)
					throw std::runtime_error("curl_easy_perform failed.");

				curl_slist_free_all(headers);
				curl_easy_cleanup(curl);

				return strm.str();
			}
			catch(std::runtime_error& rte)
			{
				if (headers)
					curl_slist_free_all(headers);

				if (curl)
					curl_easy_cleanup(curl);

				throw std::runtime_error(strrqst.str() + ". " + rte.what());
			}

			return "";
		}

	} /* namespace HTTP */
} /* namespace TWO */