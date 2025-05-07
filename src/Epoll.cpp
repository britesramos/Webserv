#include "../include/Epoll.hpp"

Epoll::Epoll(){

}

Epoll::~Epoll(){
	close(_epoll_fd);
}

int Epoll::addEpollFd(int new_connection_socket_fd){
	// struct epoll_event event;
	this->_event.data.fd = new_connection_socket_fd;
	this->_event.events = EPOLLIN | EPOLLET; // Edge Triggered ??? Not sure if this is what we want. This option is harder to implement apparently. Other option is level triggered.
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, new_connection_socket_fd, &this->_event) < 0){
		std::cout << RED << "Error adding new connection socket to epoll" << std::endl;
		return 1;
	}
	std::cout << GREEN << "Added new connection socket to epoll" << _event.data.fd << std::endl;
	return 0;
}



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