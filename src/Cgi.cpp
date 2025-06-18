#include "../include/Cgi.hpp"

Cgi::Cgi()
{
	this->del = false;
	this->get = false;
	this->post = false;
	this->config_autoindex = false;
	this->last_activity = std::chrono::steady_clock::now();
	this->start_time = std::chrono::steady_clock::now();
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

void Cgi::creating_cgi_env(Client &client)
{
	std::string method = client.get_Request("method");
	this->tmp_env.push_back("REQUEST_METHOD=" + method);
	this->tmp_env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	this->tmp_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	this->tmp_env.push_back("SCRIPT_NAME=" + this->path);
	this->tmp_env.push_back("PATH_INFO=" + client.get_Request("url_path"));
	this->tmp_env.push_back("QUERY_STRING=" + client.get_Request("query_string"));

		std::string content_lenght = client.get_Request("Content-Length:");
		if(!content_lenght.empty())
		this->tmp_env.push_back("CONTENT_LENGTH=" + content_lenght);
		std::string content_type = client.get_Request("Content-Type:");
		if(!content_type.empty())
			this->tmp_env.push_back("CONTENT_TYPE=" + content_type);

	for (size_t i = 0; i < this->tmp_env.size(); ++i)
		this->env.push_back(const_cast<char*>(this->tmp_env[i].c_str()));
	this->env.push_back(nullptr);

	// for (size_t i = 0; i < this->env.size(); ++i)
    // std::cerr <<  YELLOW <<"			CGI ENV: " << this->env[i] << std::endl;
}

void Cgi::start_cgi(Location config)
{
	this->config_root = config.getRoot();
	std::vector<std::string> config_allowed_methods = config.get_methods();
	for (size_t i = 0; i < config_allowed_methods.size(); ++i)
	{
		if (config_allowed_methods[i] == "GET")
			this->get = true;
		if (config_allowed_methods[i] == "POST")
			this->post = true;
		if (config_allowed_methods[i] == "DELETE")
			this->del = true;
	}
	this->config_autoindex = config.getAutoindex();

	if (this->post)
	{
		if (pipe(this->cgi_in) == -1)
		{
			perror("Input Pipe");
			return ;
		}
	}

	if (pipe(cgi_out) == -1)
	{
		if (this->post)
		{
			close(this->cgi_in[READ]);
			close(this->cgi_in[WRITE]);
		}
		perror("Output Pipe");
		return ;
	}
}


void Cgi::run_cgi(Client& client)
{
	std::string method = client.get_Request("method");

	this->pid = fork();
	if (this->pid == -1)
	{
		perror("Fork");
		return ;
	}

	if (this->pid == 0)
	{
		//child
		if (method == "POST" && this->post == true)
		{
			dup2(this->cgi_in[READ], STDIN_FILENO);
			close(this->cgi_in[READ]);
			close(this->cgi_in[WRITE]);
		}
		dup2(this->cgi_out[WRITE], STDOUT_FILENO);
		dup2(this->cgi_out[WRITE], STDERR_FILENO); // TODO: check this adding error from script to the pipe, all the errors are going to print on the browser x.x
		close(this->cgi_out[WRITE]);
		close(this->cgi_out[READ]);

		this->path = this->config_root + client.get_Request("url_path");
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
		this->update_activity();
		this->last_activity = this->start_time;

		if (method == "POST" && this->post == true)
			close(this->cgi_in[READ]);
		close(this->cgi_out[WRITE]);

		client.set_error_code("200");
		int status;
		int result = waitpid(pid, &status, WNOHANG);
		if (result > 0) {
			if (WIFEXITED(status)) {
				int exit_code = WEXITSTATUS(status);
				if (exit_code == 1) {
					client.set_error_code("500");
					std::cout << "error running cgi! exit code = 1" << std::endl;
				}
			} else if (WIFSIGNALED(status)) {
				int sig = WTERMSIG(status);
				client.set_error_code("500");
				std::cout << "CGI process was killed by signal: " << sig << std::endl;
			}
		}
	}
}

bool Cgi::get_method_del() const
{
	return (this->del);
}

bool Cgi::get_method_get() const
{
	return (this->get);
}

bool Cgi::get_method_post() const
{
	return (this->post);
}

bool Cgi::get_config_autoindex() const
{
	return (this->config_autoindex);
}

pid_t Cgi::get_pid() const
{
	return this->pid;
}

std::string Cgi::get_config_root() const
{ 
	return this->config_root;
}

void Cgi::update_activity() {
	last_activity = std::chrono::steady_clock::now();
}

std::chrono::steady_clock::time_point Cgi::get_activity() const {
	return this->last_activity;
}

std::chrono::steady_clock::time_point Cgi::get_start_time() const {
	return this->start_time;
}