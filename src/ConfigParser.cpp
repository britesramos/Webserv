#include "../include/ConfigParser.hpp"

bool ConfigParser::is_file_extension_correct(std::string input)
{
	if (input.substr(input.find_last_of(".") + 1) == "conf")
	{
		return (true);
	}
	else
	{
		std::cerr << "Please, provide \".conf\" file" << std::endl;
		return (false);
	}
}

static void remove_comments(std::string &str)
{
	int i = 0;
	int start = 0;
	while(str[i] != '\0')
	{
		if (str[i] == '#')
		{
			start = i;
			str.erase(start, str.length());
			return ;
		}
		i++;
	}
}

static void trim_spaces(std::string &str)
{
	int begin = 0;

	// begin
	while(isspace(str[begin]))
		begin++;
	str.erase(0, begin);

	// end
	int end = str.length();
	while(end > 0 && isspace(str[end]))
		end--;
	str.erase(end);
}

bool ConfigParser::is_possible_use_file(std::string input)
{
	std::ifstream	infile(input);
	std::string		line;
	if (infile.is_open())
	{
		if (infile.peek() == EOF)
		{
			std::cerr << "Your config file is empty." << std::endl;
			infile.close();
			return false;
		}
		else {
			while(std::getline(infile, line))
			{
				trim_spaces(line);
				remove_comments(line);
				if (!line.empty())
					this->lines.push_back(line);
			}
		}
		infile.close();
		return true;
	}
	else
	{
		std::cerr << "Are you sure the file \"" << input << "\" can be open?" << std::endl;
		return false;
	}
}

const std::vector<std::string>& ConfigParser::getLines() const
{
	return (this->lines);
}