#include "../include/ConfigParser.hpp"

bool ConfigParser::is_host_valid(std::string value)
{
	if (value == "localhost")
		return true;
	else
	{
		int i = 0;
		while(value[i])
		{
			if (value[i] == '.' && value[0] != '.')
				i++;
			else if (isdigit(value[i]))
				i++;
			else
				return false;
		}
		if (value[i - 1] == '.')
			return false;
	}
	return true;
}

bool ConfigParser::is_digit_valid(std::string value)
{
	for (size_t i = 0; i < value.size(); ++i)
	{
		if (!isdigit(value[i]))
		{
			return false;
		}
	}
	return true;
}

void ConfigParser::trim_spaces(std::string &str)
{
	size_t start = str.find_first_not_of(" \t");
	size_t end = str.find_last_not_of(" \t");

	if (start == std::string::npos) {
		str.clear();
		return ;
	}
	str = str.substr(start, end - start + 1);
}

void ConfigParser::remove_comments(std::string &str)
{
	int i = 0;
	int start = 0;

	while(str[i] != '\0')
	{
		if (str[i] == '#' && start == 0){
			start = i;
			str.erase(start, str.length());
			return ;
		}
		i++;
	}
}
