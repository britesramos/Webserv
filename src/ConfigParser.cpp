#include "../include/ConfigParser.hpp"

ConfigParser::ConfigParser()
{
	this->number_of_server = 0;
}

ConfigParser::~ConfigParser() // this is still being called ?
{
	this->servers.clear();
	this->lines.clear();
}

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
	this->number_of_server++;
	servers.push_back(server);
}

const std::vector<ServerConfig>& ConfigParser::getServer() const
{
	return (this->servers);
}

int ConfigParser::get_number_of_server() const
{
	return (this->number_of_server);
}

void ConfigParser::set_number_of_server(int number)
{
	this->number_of_server = number;
}

bool ConfigParser::config_file_parsing(std::string input)
{
	ServerConfig current_server;
	Location current_location;
	int open_curly_b = 0;
	int close_curly_b = 0;
	bool in_server_block = false;
	bool in_location_block = false;
	std::string path;

	if (is_file_extension_correct(input) == false)
		return (false);
	else
	{
		if (is_possible_use_file(input) == false)
			return (false);
	}
	const std::vector<std::string>& lines = getLines();
	for (size_t i = 0; i < lines.size(); ++i) {
		std::string str = lines[i];

		if (str.find("server") != std::string::npos && str.find('{') != std::string::npos){
			in_server_block = true;
			current_server = ServerConfig();
			open_curly_b++;
		}
		else if (in_server_block == true)
		{
			if (str.find("==") != std::string::npos){
				std::cout << "Invalid format, provide only one '=' "<< std::endl;
				return false;
			}
			if (str.find("location") != std::string::npos && str.find('{') != std::string::npos)
			{
				in_location_block = true;
				current_location = Location();

				std::string word = "location";
				std::size_t start = str.find(word) + word.length();
				path = str.substr(start);
				size_t pos = path.find('{');
				open_curly_b++;
				path.erase(pos, 1);
				trim_spaces(path);
				if (path.empty())
				{
					std::cerr << "Not a valid, Location path" << std::endl;
					return false;
				}
				current_location.setPath(path);
			}
			else if (in_location_block == false)
			{
				if (str.find('=') != std::string::npos)
				{
					if(!is_values_and_keys_set(str, current_server))
					return false;
				}
				else if (str.find('}') != std::string::npos)
				{
					close_curly_b++;
					addServer(current_server);
					in_server_block = false;
				}
				else
				{
					std::cerr << "Check your config file, there is a INVALID Element on the = SERVER = block;" << std::endl;
					return false;
				}
			}
			else if (str.find('}') != std::string::npos)
			{
				close_curly_b++;
				current_server.addLocation(path, current_location);
				in_location_block = false;
			}
			else
			{
				if(is_static_content_load(str, current_location) == false)
				    return false;
			}
		}
		else
		{
			std::cout << "Invalid Configuration, please provide = SERVER = block" << std::endl;
			return false;
		}
	}
	if (open_curly_b != close_curly_b){
		std::cerr << "Check your open and close curly brackets" << std::endl;
		return false;
	}
	return true;
}

bool ConfigParser::is_static_content_load(std::string str, Location& current_location)
{
	// Location Block
	if (str.find('=') != std::string::npos)
	{
		std::string key = str.substr(0, str.find('='));
		std::string value = str.substr(str.find_first_of('=') + 1);
		trim_spaces(key);
		trim_spaces(value);
		if (!is_semicolon_present(value))
			return false;
		if (key == "root")
		{
			if (is_value_empty(key, value) == true)
				return false;
			current_location.setRoot(value);
		}
		else if (key == "index")
		{
			if (is_value_empty(key, value) == true)
				return false;
			if (value.size() < 5 || value.compare(value.size() - 5, 5, ".html") != 0)
			{
				std::cout << "Invalid extention for INDEX, change to .html" << std::endl;
				return false;
			}
			current_location.setIndex(value);
		}
		else if (key == "autoindex")
		{
			if (is_value_empty(key, value) == true)
				return false;
			bool enable;
			if (value == "off")
			{
				enable = OFF;
			}
			else
				enable = ON;
			current_location.setAutoindex(enable);
		}
		else if (key == "allowed_methods")
		{
			if (is_value_empty(key, value))
				return false;
			std::vector<std::string> temp = split_by_whitespace(value);
			for (size_t i = 0; i < temp.size(); ++i)
			{
				if (temp[i].empty())
				{
					std::cerr << "Invalid config to Allowed Methods in - LOCATION - block" << std::endl;
					return false;
				}
				current_location.set_methods(temp[i]);
			}
		}
		else if (key == "return")
		{
			if (is_value_empty(key, value) == true)
				return false;
			std::vector<std::string> temp = split_by_whitespace(value);
			if (temp.size() != 2 || !is_digit_valid(temp[0]) || temp[0].empty() || temp[1].empty())
			{
				std::cerr << "Invalid values from Return in  - LOCATION - block" << std::endl;
				return false;
			}
			current_location.setReturnvalue(temp[0], temp[1]);
		}
		else
		{
			std::cerr << "UNKNOW values inside - LOCATION - block" << std::endl;
			return false;
		}
	}
	else
	{
		std::cerr << "Invalid values inside - LOCATION - block" << std::endl;
		return false;
	}
	return true;
}

bool ConfigParser::is_values_and_keys_set(std::string str, ServerConfig& current_server)
{
	// Server Block
	std::string key = str.substr(0, str.find('='));
	std::string value = str.substr(str.find_first_of('=') + 1);
	trim_spaces(key);
	trim_spaces(value);
	if (!is_semicolon_present(value))
		return false;
	if (key == "host")
	{
		if (!is_host_valid(value) || value.empty())
		{
			std::cerr << "Not a valid, Host number" << std::endl;
			return false;
		}
		if (value == "localhost")
			value = "127.0.0.1";
		current_server.setHost(value);
	}
	else if (key == "port")
	{
		if (!is_digit_valid(value) || value.empty())
		{
			std::cerr << "Not a valid, Port" << std::endl;
			return false;
		}
		current_server.setPort(value);
	}
	else if (key == "server_name")
	{
		if (is_value_empty(key, value))
			return false;
		current_server.setServerName(value);
	}
	else if (key == "max_client_size")
	{
		if (!is_digit_valid(value) || value.empty())
		{
			std::cerr << "Not a valid, client_size number" << std::endl;
			return false;
		}
		current_server.setMaxClientSize(std::stoi(value));
	}
	else if (key.find("error_page") != std::string::npos){
		std::string word = "error_page";
		std::size_t start = key.find(word) + word.length();
		key = key.substr(start);
		trim_spaces(key);
        if (value.size() < 5 || value.compare(value.size() - 5, 5, ".html") != 0)
        {
            std::cout << "Invalid extention for ERROR, change to .html" << std::endl;
            return false;
        }
		if (!is_digit_valid(key) || key.empty() || value.empty())
		{
			std::cerr << "Not a valid, error page" << std::endl;
			return false;
		}
		current_server.setErrorPage(key, value);
	}
	else
	{
		std::cerr << "Check your config file, there is a UNKNOW variable on the = SERVER = block;" << std::endl;
		return false;
	}
	return true;
}

void ConfigParser::clean_up(){
	this->lines.clear();
	this->servers.clear();
	this->number_of_server = 0;
	std::cout << "ConfigParser cleaned up." << std::endl;
}
