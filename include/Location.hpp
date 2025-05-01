#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <map>
#include <vector>

#define ON true;
#define OFF false;

class Location {
	private:
		std::string path;
		std::string root;
		std::string index;
		std::vector<std::string> allowed_methods;
		bool autoindex;
		std::map<int, std::string> return_value;
	public:
		Location();
		~Location();

		// Setters
		void setPath(std::string path);
		void setRoot(std::string root);
		void setIndex(std::string index);
		void set_methods(std::string method);
		void setAutoindex(bool value);
		void setReturnvalue(int code, std::string value);

		// Getters
		std::string getPath() const;
		std::string getRoot() const;
		std::string getIndex() const;
		const std::vector<std::string>& get_methods() const;
		bool getAutoindex() const;
		std::string getReturnvalue(int code) const;

		void print() const;
};

#endif