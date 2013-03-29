#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

#include <pacificaauth.h>

#define BUFSIZE 1024

int pacifica_auth(pacifica_auth_cb callback, void *data)
{
	ssize_t size;
	char buffer[BUFSIZE];
	int status;
	int res = 0;
	int client_stdout[2];
	pid_t pid;
	res = pipe(client_stdout);
	if(res)
	{
		return res;
	}
	pid = fork();
	if(pid < 0)
	{
		res = 0;
	}
	else if(pid == 0)
	{
		close(client_stdout[0]);
		res = dup2(client_stdout[1], STDOUT_FILENO);
		if(res == -1)
		{
			return res;
		}
		res = execlp("pacificaauth", "pacificaauth", NULL);
		return res;
	}
	else
	{
		close(client_stdout[1]);
		while(1)
		{
			size = read(client_stdout[0], buffer, BUFSIZE);
			if(size < 0)
			{
				res = size;
				break;
			}
			if(size == 0)
			{
				break;
			}
			callback(buffer, size, data);
//			fwrite(buffer, size, 1, stdout);
		}
		res = wait(&status);
		if(res != -1 && WIFEXITED(status))
		{
 			res = WEXITSTATUS(status);
		}
		else
		{
			res = -1;
		}
	}
	return res;
}
