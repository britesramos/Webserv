#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <unordered_map>
#include <fstream>
#include <vector>
#include "ServerConfig.hpp"

class ConfigParser {
	private:
		std::vector<std::string> lines;
		std::vector<ServerConfig> servers;
		int number_of_server;
	public:
		ConfigParser();
		~ConfigParser();
		void addServer(ServerConfig &server);
		const std::vector<ServerConfig>& getServer() const;
		int get_number_of_server() const;
		void set_number_of_server(int number);

		bool is_file_extension_correct(std::string input);
		bool is_possible_use_file(std::string input);
		const std::vector<std::string>& getLines() const;

		bool config_file_parsing(std::string input);
		bool is_values_and_keys_set(std::string str, ServerConfig& current_server);
		bool is_static_content_load(std::string str, Location& current_location);

		// Utils
		void remove_comments(std::string &str);
		void trim_spaces(std::string &str);
		bool is_host_valid(std::string value);
		bool is_digit_valid(std::string value);
		bool is_value_empty(std::string key, std::string value);
		bool is_semicolon_present(std::string& value);
		std::vector<std::string> split_by_whitespace(const std::string& str);

};

#endif