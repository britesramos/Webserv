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
		if (str[i] == '#' && start == 0){
			start = i;
			str.erase(start, str.length());
			return ;
		}
		i++;
	}
}

void trim_spaces(std::string &str)
{
	size_t start = str.find_first_not_of(" \t");
	size_t end = str.find_last_not_of(" \t");

	if (start == std::string::npos) {
		str.clear();
		return ;
	}
	str = str.substr(start, end - start + 1);
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
				remove_comments(line);
				trim_spaces(line);
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

void ConfigParser::addServer(ServerConfig &server)
{
	servers.push_back(server);
}

const std::vector<ServerConfig>& ConfigParser::getServer() const
{
	return (this->servers);
}

static bool is_host_valid(std::string value)
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

static bool is_digit_valid(std::string value)
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

bool ConfigParser::is_server_config_load(const std::vector<std::string>& lines)
{
	ServerConfig current;
	int open_curly_b = 0;
	int close_curly_b = 0;
	bool in_server_block = false;

	for (size_t i = 0; i < lines.size(); ++i) {
		std::string str = lines[i];

		if (str.find("server") != std::string::npos && str.find('{') != std::string::npos){
			in_server_block = true;
			current = ServerConfig();
			open_curly_b++;
		}
		if (in_server_block == true)
		{
			if (str.find("==") != std::string::npos){
				std::cout << "Invalid format, provide only one '=' "<< std::endl;
				return false;
			}
			if (str.find('=') != std::string::npos)
			{
				std::string key = str.substr(0, str.find('='));
				std::string value = str.substr(str.find_first_of('=') + 1);
				trim_spaces(key);
				trim_spaces(value);
				if (value.find(';') == std::string::npos){
					std::cout << "Invalid format, end of string need to have ;"<< std::endl;
					return false;
				}
				else
				{
					size_t pos = value.find(';');
					value.erase(pos, 1);
				}
				if (key.find("error_page") != std::string::npos){
					std::string word = "error_page";
					std::size_t start = key.find(word) + word.length();
					key = key.substr(start);
					trim_spaces(key);
					if (!is_digit_valid(key))
					{
						std::cerr << "Not a valid, error page number" << std::endl;
						return false;
					}
					current.setErrorPage(std::stoi(key), value);
				}
				else if (key == "host")
				{
					if (!is_host_valid(value))
					{
						std::cerr << "Not a valid, Host number" << std::endl;
						return false;
					}
					current.setHost(value);
				}
				else if (key == "port")
				{
					if (!is_digit_valid(value))
					{
						std::cerr << "Not a valid, Port number" << std::endl;
						return false;
					}
					if (value.empty())
					{
						std::cerr << "Invalid config, port cannot be empty." << std::endl;
						return false;
					}
					current.setPort(value);
				}
				else if (key == "server_name")
					current.setServerName(value);
				else if (key == "max_client_size")
				{
					if (!is_digit_valid(value))
					{
						std::cerr << "Not a valid, client_size number" << std::endl;
						return false;
					}
					current.setMaxClientSize(std::stoi(value));
				}
			}
			if (str.find('}') != std::string::npos)
			{
				close_curly_b++;
				addServer(current);
				in_server_block = false;
			}
		}
		else
		{
			std::cout << "Invalid Configuration, please provide server block" << std::endl;
			return false;
		}
	}
	if (open_curly_b != close_curly_b){
		std::cerr << "Check your open and close curly brackets" << std::endl;
		return false;
	}
	return true;
}
