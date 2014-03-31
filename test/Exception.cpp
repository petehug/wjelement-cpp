#include "exception.h"
#include <iostream>
#include <stdarg.h>
#include <stdio.h>

#define D3_MAX_ERRMSG_SIZE		65536


namespace D3
{
	int snprintf(char* pszBuffer, size_t size, const char* pszFormat, ...)
	{
		size_t    nCount;
		va_list   pArgs;

		va_start(pArgs, pszFormat);
		nCount = _vscprintf(pszFormat, pArgs);
		_vsnprintf_s(pszBuffer, size, _TRUNCATE, pszFormat, pArgs);
		va_end(pArgs);

		return nCount;
	}


	void GenericExceptionHandler(const char * pFormat, va_list vArgs)
	{
		char	xtramsg[D3_MAX_ERRMSG_SIZE];

		xtramsg[0] = '\0';
		vsnprintf(xtramsg, D3_MAX_ERRMSG_SIZE, pFormat, vArgs);

		try
		{
			throw;
		}
		catch(const std::bad_alloc& exBA)
		{
			snprintf(xtramsg, D3_MAX_ERRMSG_SIZE, "%s std::bad_alloc exception: %s", exBA.what());
		}
		catch(const std::exception& exSTD)
		{
			snprintf(xtramsg, D3_MAX_ERRMSG_SIZE, "%s std::exception: %s", xtramsg, exSTD.what());
		}
		catch(const std::string& exStr)
		{
			snprintf(xtramsg, D3_MAX_ERRMSG_SIZE, "%s std::string: %s", xtramsg, exStr.c_str());
		}
		catch(const char* psz)
		{
			snprintf(xtramsg, D3_MAX_ERRMSG_SIZE, "%s char*: %s", xtramsg, psz);
		}
		catch(...)
		{
			snprintf(xtramsg, D3_MAX_ERRMSG_SIZE, "%s Unknown exception", xtramsg);
		}

		std::cout << xtramsg << std::endl;
	}


	void GenericExceptionHandler(const char * pFormat, ...)
	{
		va_list	vArgs;
		va_start(vArgs, pFormat);
		GenericExceptionHandler(pFormat, vArgs);
		va_end(vArgs);
	}

}