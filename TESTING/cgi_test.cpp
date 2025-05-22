#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

#define WRITE 1
#define READ 0

class Cgi {
	private:
		int cgi_in[2];
		int cgi_out[2];
		std::vector<char*> env;
		std::vector<std::string> tmp_env;
	public:
		Cgi();
		~Cgi();

		int get_cgi_in(int pos);
		int get_cgi_out(int pos);
		void run_cgi();
		void creating_cgi_env();

};


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

void Cgi::creating_cgi_env() // TODO: REVIEW this function
{
	// std::string method = client.get_Request("method");
	this->tmp_env.push_back("REQUEST_METHOD=GET");
	// std::string script = client.get_Request("method");
	this->tmp_env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	this->tmp_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	// std::string path = client.get_Request("url_path"); // need to parse? 
	// this->tmp_env.push_back("SCRIPT_NAME=" + path);

	// if (method == "POST")
	// {
	// 	std::string content_lenght = client.get_Request("content_length");
	// 	if (!content_lenght.empty())
	// 	{
	// 		this->tmp_env.push_back("CONTENT_LENGTH=" + content_lenght);
	// 	}
	// }

	for (size_t i = 0; i < this->tmp_env.size(); ++i)
		this->env.push_back(const_cast<char*>(this->tmp_env[i].c_str()));
	this->env.push_back(nullptr);
}


void Cgi::run_cgi()
{
	pid_t pid;
	// int cgi_in[2];
	// int cgi_out[2];

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

		const char* cgi_program = "./cgi-bin/hello_world.py";
		const char* argv[] = {"/usr/bin/python3", cgi_program, nullptr};

		creating_cgi_env();
		execve(argv[0], const_cast<char* const*>(argv), env.data());
		perror("execve fail");
		exit(1);
	}
	else
	{
		//parent
		close(this->cgi_out[WRITE]);

		char buffer[1024];
        ssize_t nbytes;
        while ((nbytes = read(this->cgi_out[READ], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[nbytes] = '\0';
            std::cout << buffer;
        }
        close(this->cgi_out[READ]);
        wait(NULL); // wait for child to finis
	}

}

int main()
{
	Cgi cgi;

	cgi.run_cgi();
}