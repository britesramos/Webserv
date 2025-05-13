#include "../include/Cgi.hpp"

Cgi::Cgi()
{

}

Cgi::~Cgi()
{

}

int Cgi::get_cgi_in(int pos)
{
	return this->cgi_in[pos];
}

int Cgi::get_cgi_out(int pos)
{
	return this->cgi_out[pos];
}

void Cgi::creating_cgi_env(Client &client) // TODO: REVIEW this function
{
	std::string method = client.get_Request("method");
	this->tmp_env.push_back("REQUEST_METHOD=" + method);
	this->tmp_env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	this->tmp_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	this->tmp_env.push_back("SCRIPT_NAME=" + this->path);

	if (method == "POST")
	{
		std::string content_lenght = client.get_Request("content_length");
		if (!content_lenght.empty())
		{
			this->tmp_env.push_back("CONTENT_LENGTH=" + content_lenght);
		}
	}

	for (size_t i = 0; i < this->tmp_env.size(); ++i)
		this->env.push_back(const_cast<char*>(this->tmp_env[i].c_str()));
	this->env.push_back(nullptr);

	for (size_t i = 0; i < this->env.size() && env[i] != nullptr; ++i) {
		std::cout << "				" << this->env[i] << std::endl;
	}
}


void Cgi::run_cgi(Client& client)
{
	pid_t pid;

	if (pipe(cgi_in) == -1 || pipe(cgi_out) == -1)
	{
		perror("Pipe");
		return ;
	}

	pid = fork();
	if (pid == -1)
	{
		perror("Fork");
		return ;
	}

	if (pid == 0)
	{
		//child
		// dup2(this->cgi_in[READ], STDIN_FILENO);
		// close(this->cgi_in[WRITE]);
		dup2(this->cgi_out[WRITE], STDOUT_FILENO);
		close(this->cgi_out[WRITE]);
		close(this->cgi_out[READ]);

		this->path = "." + client.get_Request("url_path"); // just to test, will not have the dot
		const char* cgi_program = path.c_str();
		const char* argv[] = {"/usr/bin/python3", cgi_program, nullptr};

		creating_cgi_env(client);
		execve(argv[0], const_cast<char* const*>(argv), env.data());
		perror("execve fail");
		exit(1);
	}
	else
	{
		//parent
		close(this->cgi_out[WRITE]); // TODO: return pipe for the write function 

		char buffer[1024]; // just for testing
        ssize_t nbytes;
        while ((nbytes = read(this->cgi_out[READ], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[nbytes] = '\0';
            std::cout << "CGI output: \n" << buffer; // to print in the terminal
        }
        close(this->cgi_out[READ]);
		int status;
		waitpid(pid, &status, WNOHANG);
	}

}
