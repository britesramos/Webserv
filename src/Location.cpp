#include "../include/Location.hpp"

Location::Location()
{
	this->autoindex = OFF;
}

Location::~Location()
{

}

// Setters
void Location::setPath(std::string path)
{
	this->path = path;
}

void Location::setRoot(std::string root)
{
	this->root = root;
}

void Location::setIndex(std::string index)
{
	this->index = index;
}

void Location::set_methods(std::string method)
{
	this->allowed_methods.push_back(method);
}

void Location::setAutoindex(bool value)
{
	this->autoindex = value;
}

void Location::setReturnvalue(int code, std::string value)
{
	this->return_value[code] = value;
}

// Getters
std::string Location::getPath() const
{
	return(this->path);
}

std::string Location::getRoot() const
{
	return(this->root);
}

std::string Location::getIndex() const
{
	return(this->index);
}

const std::vector<std::string>& Location::get_methods() const
{
	return(this->allowed_methods);
}

bool Location::getAutoindex() const
{
	return (this->autoindex);
}

std::string Location::getReturnvalue(int code) const
{
	auto temp = this->return_value.find(code);
	if (temp != this->return_value.end()) // check if is not in the end of the map
		return (temp->second);
	else
		return ("");
}

void Location::print() const {
	std::cout << "  Location: " << path << "\n";
	std::cout << "  Root: " << root << "\n";
	std::cout << "  Autoindex: " << autoindex << "\n";
	std::cout << "  Allowed Methods: ";
	for (size_t i = 0; i < allowed_methods.size(); ++i) {
		std::cout << allowed_methods[i];
		if (i != allowed_methods.size() - 1) std::cout << ", ";
	}
	std::cout << "\nReturn:\n";
	for (std::map<int, std::string>::const_iterator it = return_value.begin(); it != return_value.end(); ++it) {
		std::cout << "  " << it->first << " -> " << it->second << "\n";
	}
	std::cout << "\n";
}