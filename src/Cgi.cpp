#include "../include/Cgi.hpp"

Cgi::Cgi()
{
	std::cout << "construct cgi" << std::endl;
}

Cgi::~Cgi()
{
	std::cout << "deconstruct cgi" << std::endl;
}


void Cgi::run_cgi(Client& client)
{
    int pipe_fd[2];
    pipe(pipe_fd);
    
    pid_t pid = fork();
    if (pid == 0) {
		// child
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]); // close unused read

        exit(1); // fail-safe
    } else {
		// parent
        close(pipe_fd[1]); // parent reads from pipe
        char buffer[4096];
        read(pipe_fd[0], buffer, sizeof(buffer));
        std::cout << buffer; // send this to client
        waitpid(pid, NULL, 0);
    }

}

