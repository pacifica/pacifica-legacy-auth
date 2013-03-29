#include <stdio.h>

#include <pacificaauth.h>

static void callback(const char *buffer, int size, void *data)
{
	fwrite(buffer, size, 1, stdout);
}

int main(int argc, char *argv[])
{
	int res;
	res = pacifica_auth(callback, NULL);
	printf("Got res: %d\n", res);
	return 0;
}
