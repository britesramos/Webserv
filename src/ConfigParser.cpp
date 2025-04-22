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

std::vector<std::string> ConfigParser::tokenize_line(const std::string& line)
{
    std::vector<std::string> token_list;
    std::string token;
	int i = 0;

	while (line[i]) {
        char c = line[i];

        if (std::isspace(c)) {
            if (!token.empty()) {
                token_list.push_back(token);
                token.clear();
            }
        } else if (c == '{' || c == '}' || c == '=' || c == ';') {
            if (!token.empty()) {
                token_list.push_back(token);
                token.clear();
            }
            token_list.push_back(std::string(1, c)); // transform 1 char into a string to put on the vector
        } else {
			printf("this is char:   %c\n", c);
            token += c;
        }
		printf("this is the token at the moment: %s \n", token.c_str());
		i++;
    }

    if (!token.empty()) { // checks if all the tokens was add to the token list
        token_list.push_back(token);
    }

    return token_list;
}