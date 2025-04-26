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

// std::vector<std::string> ConfigParser::tokenize_line(const std::string& line)
// {
//     std::vector<std::string> token_list;
//     std::string token;
// 	int i = 0;

// 	while (line[i]) {
//         char c = line[i];

//         if (std::isspace(c)) {
//             if (!token.empty()) {
//                 token_list.push_back(token);
//                 token.clear();
//             }
//         } else if (c == '{' || c == '}' || c == '=' || c == ';') {
//             if (!token.empty()) {
//                 token_list.push_back(token);
//                 token.clear();
//             }
//             token_list.push_back(std::string(1, c)); // transform 1 char into a string to put on the vector
//         } else {
// 			printf("this is char:   %c\n", c);
//             token += c;
//         }
// 		printf("this is the token at the moment: %s \n", token.c_str());
// 		i++;
//     }

//     if (!token.empty()) { // checks if all the tokens was add to the token list
//         token_list.push_back(token);
//     }

//     return token_list;
// }


void ConfigParser::addServer(ServerConfig &server)
{
	servers.push_back(server);
}

const std::vector<ServerConfig>& ConfigParser::getServer() const
{
	return (this->servers);
}

// TODO: ADD parameteres to the class ServerConfig, because if they are in a unordered list the same key with different value will be deleted
// bool ConfigParser::is_map_filled(const std::vector<std::string>& lines)
// {
// 	std::unordered_map<std::string, std::string> parameteres;

// 	for (size_t i = 0; i < lines.size(); ++i) {
// 		std::string str = lines[i];
// 		if (str.find("==") != std::string::npos){
// 			std::cout << "Invalid format, provide only one '=' "<< std::endl;
// 			return false;
// 		}
// 		if (str.find('=') != std::string::npos)
// 		{
// 			std::string key = str.substr(0, str.find('='));
// 			std::string value = str.substr(str.find_first_of('=') + 1);
// 			trim_spaces(key);
// 			trim_spaces(value);
// 			if (value.find(';') == std::string::npos){
// 				std::cout << "Invalid format, end with ;"<< std::endl;
// 				return false;
// 			}
// 			if (key.find("error_page") != std::string::npos){
// 				std::string word = "error_page";
// 				std::size_t start = key.find(word) + word.length();
// 				key = key.substr(start);
// 				trim_spaces(key);
// 			}
// 			parameteres[key] = value;
// 		}
// 		else
// 		{
// 			parameteres[str] = "";
// 		}
// 	}
// 	for (auto it = parameteres.begin(); it != parameteres.end(); ++it) {
// 		std::cout << it->first << ":" << it->second << std::endl;
// 	}
// 	return true;
// }

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
					std::cout << "Invalid format, end with ;"<< std::endl;
					return false;
				}
				if (key.find("error_page") != std::string::npos){
					std::string word = "error_page";
					std::size_t start = key.find(word) + word.length();
					key = key.substr(start);
					trim_spaces(key);
				}
				// parameteres[key] = value;
			}
			std::cout << "Entrei aqui\n" << std::endl;
			if (str.find('}') != std::string::npos)
			{
				close_curly_b++;
				this->servers.push_back(current);
				in_server_block = false;
			}
		}
		else
		{
			std::cout << "Invalid Configuration, please provide server block" << std::endl;
			return false;
		}
	}
	std::cout << "open: " << open_curly_b << " and close: " << close_curly_b << std::endl;
	if (open_curly_b != close_curly_b){
		std::cerr << "Check your open and close curly brackets" << std::endl;
		return false;
	}
	return true;
}