#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <ostream>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

class Epoll{
	private:
		int _epoll_fd;
		struct epoll_event _event;

	public:
		Epoll();
		~Epoll();


		int addEpollFd(int new_connection_socket_fd);

		// Setters and Getters
		void set_epoll_fd(int new_connection_socket_fd);
		void set_event(int fd, uint32_t events);

		int get_epoll_fd() const;
		struct epoll_event get_event() const;

};