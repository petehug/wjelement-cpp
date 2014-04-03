#include "Exception.h"
#include <iostream>
#include <stdexcept>

void handleGenericException()
{
	try
	{
		throw;
	}
	catch (const std::runtime_error & e)
	{
		std::cout << "caught exception: " << e.what() << std::endl;
	}
	catch (const char * p)
	{
		std::cout << "caught exception: " << p << std::endl;
	}
}



