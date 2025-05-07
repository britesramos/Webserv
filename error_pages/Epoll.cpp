#include "../include/Epoll.hpp"
#include <unistd.h>

Epoll::Epoll(){

}

Epoll::~Epoll(){
	close(_epoll_fd);
}