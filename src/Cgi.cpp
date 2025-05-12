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
	std::vector<std::string> env_tmp;

	std::string method = client.get_Request("method");
	env_tmp.push_back("REQUEST_METHOD=" + method);
	std::string script = client.get_Request("method");
	env_tmp.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env_tmp.push_back("GATEWAY_INTERFACE=CGI/1.1");
	std::string path = client.get_Request("url_path"); // need to parse? 
	env_tmp.push_back("SCRIPT_NAME=" + path);

	if (method == "POST")
	{
		std::string content_lenght = client.get_Request("content_length");
		if (!content_lenght.empty())
		{
			env_tmp.push_back("CONTENT_LENGTH=" + content_lenght);
		}
	}

	for (size_t i = 0; i < env_tmp.size(); ++i)
		this->env.push_back(const_cast<char*>(env_tmp[i].c_str()));

	this->env.push_back(nullptr);

}


void Cgi::run_cgi(Client& client)
{
	// pid_t pid;
	// // int cgi_in[2];
	// // int cgi_out[2];

	// if (pipe(cgi_in) == -1 || pipe(cgi_out) == -1)
	// {
	// 	perror("Pipe");
	// 	return ;
	// }

	// pid = fork();
	// if (pid == -1)
	// {
	// 	perror("Fork");
	// 	return ;
	// }

	// if (pid == 0)
	// {
	// 	//child
	// 	dup2(this->cgi_in[READ], STDIN_FILENO);
	// 	close(this->cgi_in[WRITE]);
	// 	dup2(this->cgi_out[WRITE], STDOUT_FILENO);
	// 	close(this->cgi_out[WRITE]);
	// 	close(this->cgi_out[READ]);

		// const char* cgi_program = client.get_Request("url_path").c_str();
		// const char* argv[] = {"/usr/bin/python3", cgi_program, nullptr};
		// std::string method_str = "REQUEST_METHOD=" + client.get_Request("method");
		// std::string content_length_str = "CONTENT_LENGTH=" + client.get_Request("Content-Length");
		// std::string env[] = { "SERVER_PROTOCOL=HTTP/1.1", "GATEWAY_INTERFACE=CGI/1.1",
		// 	method_str, content_length_str,
		// 	nullptr
		// };

		// int i = 0;
		// while(env[i])
		// {
		// 	std::cout << env << std::endl;
		// 	i++;
		// }

		// std::vector<std::string> env = {
		// 	"SERVER_PROTOCOL=HTTP/1.1",
		// 	"GATEWAY_INTERFACE=CGI/1.1",
		// 	method_str,
		// 	// content_length_str
		// };
		creating_cgi_env(client);
		for (size_t i = 0; i < this->env.size() && env[i] != nullptr; ++i) {
			std::cout << "				" << this->env[i] << std::endl;
		}
		// execve(argv[0], const_cast<char* const*>(argv), const_cast<char* const*>(env));
	// }
	// else
	// {
	// 	//parent
	// }

}


