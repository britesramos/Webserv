#include <iostream>

int main (int argc, char **argv)
{
	if (argc > 2)
	{
		std::cout << "Wrong Amount of Arguments" << std::endl;
		return 1;
	}
	else
	{
		if(argc == 2)
		{
			std::cout << argv[1] << std::endl;
			//config file should be used as argv[1]
		}
		else
		{
			std::cout << "Default"<< std::endl;
			// should use a default config file
		}
	}
}