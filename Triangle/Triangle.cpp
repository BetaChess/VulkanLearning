//
// Program entry point
//


#include "phm_app.h"

#include <iostream>
#include <cstdlib>
#include <stdexcept>



int main()
{
	phm::Application app{ };

	try
	{
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what();
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
