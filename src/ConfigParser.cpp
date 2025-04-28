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

// bool ConfigParser::is_static_content_load(std::string str, ServerConfig& server, bool in_location_block)
// {
// 	Location current_location = Location();
// 	// int open_curly_b = 0;
// 	// int close_curly_b = 0;


// 	std::cout << "location str line: " << str << std::endl;
// 	if (str.find("location") != std::string::npos && str.find('{') != std::string::npos)
// 	{
// 		std::string path;
// 		std::string word = "location";
// 		std::size_t start = str.find(word) + word.length();
// 		path = str.substr(start);
// 		if (path.empty())
// 		{
// 			std::cerr << "Not a valid, error page" << std::endl;
// 			return false;
// 		}
// 		size_t pos = path.find('{');
// 		// open_curly_b++;
// 		path.erase(pos, 1);
// 		trim_spaces(path);
// 		current_location.setPath(path);
// 		std::cout << "this is path: " << path << std::endl;
// 	}
// 	// else if (str.find('}') != std::string::npos)
// 	// {
// 	// 	// close_curly_b++;
// 	// 	server.addLocation(current);
// 	// 	in_location_block = false;
// 	// }
// 	// else
// 	// {
// 	// 	std::cerr << "Check your config file, there is a INVALID Element on the location block;" << std::endl;
// 	// 	std::cout << "this is the str: " << str << std::endl;
// 	// 	return false;
// 	// }
// 	// if (open_curly_b != close_curly_b){
// 	// 	std::cerr << "Check your open and close curly brackets from location" << std::endl;
// 	// 	return false;
// 	// }
// 	return true;
// }


bool ConfigParser::is_server_config_load(const std::vector<std::string>& lines)
{
	ServerConfig current;
	Location current_location;
	int open_curly_b = 0;
	int close_curly_b = 0;
	bool in_server_block = false;
	bool in_location_block = false;

	for (size_t i = 0; i < lines.size(); ++i) {
		std::string str = lines[i];

		if (str.find("server") != std::string::npos && str.find('{') != std::string::npos){
			in_server_block = true;
			current = ServerConfig();
			open_curly_b++;
			continue ;
		}
		if (in_server_block == true)
		{
			if (str.find("==") != std::string::npos){
				std::cout << "Invalid format, provide only one '=' "<< std::endl;
				return false;
			}
			if (str.find("location") != std::string::npos && str.find('{') != std::string::npos)
			{
				in_location_block = true;
				// if(!is_static_content_load(str, current, in_location_block))
				// {
				// 	std::cerr << "Misconfiguration on the Static Content part." << std::endl;
				// 	return false;
				// }
				// continue;

				current_location = Location();
			
				std::cout << "location str line: " << str << std::endl;
				std::string path;
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
				std::cout << "this is path: " << path << std::endl;
				current_location.setPath(path);
			}
			if (in_location_block == false)
			{
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
					if (key == "host")
					{
						if (!is_host_valid(value) || value.empty())
						{
							std::cerr << "Not a valid, Host number" << std::endl;
							return false;
						}
						current.setHost(value);
					}
					else if (key == "port")
					{
						if (!is_digit_valid(value) || value.empty())
						{
							std::cerr << "Not a valid, Port" << std::endl;
							return false;
						}
						current.setPort(value);
					}
					else if (key == "server_name")
					{
						if (value.empty())
						{
							std::cerr << "Server_name cannot be set as empty" << std::endl;
							return false;
						}
						current.setServerName(value);
					}
					else if (key == "max_client_size")
					{
						if (!is_digit_valid(value) || value.empty())
						{
							std::cerr << "Not a valid, client_size number" << std::endl;
							return false;
						}
						current.setMaxClientSize(std::stoi(value));
					}
					else if (key.find("error_page") != std::string::npos){
						std::string word = "error_page";
						std::size_t start = key.find(word) + word.length();
						key = key.substr(start);
						trim_spaces(key);
						if (!is_digit_valid(key) || key.empty() || value.empty())
						{
							std::cerr << "Not a valid, error page" << std::endl;
							return false;
						}
						current.setErrorPage(std::stoi(key), value);
					}
					else
					{
						std::cerr << "Check your config file, there is a UNKNOW variable on the server block;" << std::endl;
						// std::cout << "this is the str: " << str << std::endl;
						return false;
					}
					// std::cout << "Assigning key: " << key << " value: " << value << std::endl;
				}
				else if (str.find('}') != std::string::npos)
				{
					close_curly_b++;
					addServer(current);
					in_server_block = false;
				}
				else
				{
					std::cerr << "Check your config file, there is a INVALID Element on the server block;" << std::endl;
					std::cout << "this is the str: " << str << std::endl;
					return false;
				}
			}
			else if (str.find('}') != std::string::npos)
			{
				close_curly_b++;
				current.addLocation(current_location);
				in_location_block = false;
			}
		}
		else
		{
			std::cout << "Invalid Configuration, please provide server block" << std::endl;
			return false;
		}
	}
	if (open_curly_b != close_curly_b){
		std::cerr << "Check your open and close curly brackets " << open_curly_b << "close:" << close_curly_b << std::endl;
		return false;
	}
	return true;
}



// if (str.find('=') != std::string::npos)
// {
// 	std::string key = str.substr(0, str.find('='));
// 	std::string value = str.substr(str.find_first_of('=') + 1);
// 	trim_spaces(key);
// 	trim_spaces(value);
// 	if (value.find(';') == std::string::npos){
// 		std::cout << "Invalid format, end of string need to have ;"<< std::endl;
// 		return false;
// 	}
// 	else
// 	{
// 		size_t pos = value.find(';');
// 		value.erase(pos, 1);
// 	}
// 	std::cout << "this is the key " << key << std::endl;
// 	if (key == "root")
// 	{
// 		if (value.empty())
// 		{
// 			std::cerr << "root cannot be set as empty" << std::endl;
// 			return false;
// 		}
// 		current_location.setRoot(value);
// 	}