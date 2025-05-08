#include "../include/Epoll.hpp"

Epoll::Epoll(){

}

Epoll::~Epoll(){
	close(_epoll_fd);
}

int Epoll::init_epoll(){
	this->_epoll_fd = epoll_create1(0);
	if (this->_epoll_fd== -1)
		return 1;
	return 0;
}

int Epoll::addServerSocket(int server_socket_fd){
	this->_event.data.fd = server_socket_fd;
	this->_event.events = EPOLLIN;
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, server_socket_fd, &this->_event) < 0){
		std::cout << RED << "Error adding server socket fd to epoll interest list: " << _event.data.fd << std::endl;
		return 1;
	}
	std::cout << GREEN << "Added server socket fd to epoll interest list: " << _event.data.fd << std::endl;
	return 0;
}

int Epoll::addEpollFd(int new_connection_socket_fd){
	this->_event.data.fd = new_connection_socket_fd;
	this->_event.events = EPOLLIN | EPOLLET; // Edge Triggered ??? Not sure if this is what we want. This option is harder to implement apparently. Other option is level triggered.
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, new_connection_socket_fd, &this->_event) < 0){
		std::cout << RED << "Error adding socket fd to epoll interest list: " << _event.data.fd << std::endl;
		return 1;
	}
	std::cout << GREEN << "Added new socket fd to epoll interest list: " << _event.data.fd << std::endl;
	return 0;
}

//TODO: Add a function to remove a socket from epoll interest list. This is needed when the client closes the connection. Adrii has this in his code, but I am not sure if it is needed. I will leave it for now.


//Setters and Getters
void Epoll::set_epoll_fd(int fd){
	this->_epoll_fd = fd;
}
void Epoll::set_event(int fd, uint32_t events){
	this->_event.data.fd = fd;
	this->_event.events = events;
}
int Epoll::get_epoll_fd() const{
	return this->_epoll_fd;
}
struct epoll_event Epoll::get_event() const{
	return this->_event;
}