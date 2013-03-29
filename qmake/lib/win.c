#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "pacificaauth.h"

#define BUFSIZE 1024

__declspec(dllexport) int pacifica_auth(pacifica_auth_cb callback, void *data)
{
	DWORD len;
	char buffer[BUFSIZE];
	HANDLE stdout_read;
	HANDLE stdout_write;
	int res = 0;
	SECURITY_ATTRIBUTES sa;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&sa, 0, sizeof(SECURITY_ATTRIBUTES));
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	res = CreatePipe(&stdout_read, &stdout_write, &sa, 0);
	if(!res)
	{
		return -1;
	}
	else
	{
		res = SetHandleInformation(stdout_read, HANDLE_FLAG_INHERIT, 0);
		memset(&si, 0, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdError = GetStdHandle(STD_OUTPUT_HANDLE);
		si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		si.hStdOutput = stdout_write;
		res = CreateProcess(NULL, TEXT("pacificaauth.exe"), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
		if(!res)
		{
			printf("error %d\n", GetLastError());
			return GetLastError();
		}
		res = CloseHandle(stdout_write);
		if(!res)
		{
			return GetLastError();
		}
		while(1)
		{
			len = 0;
			res = ReadFile(stdout_read, buffer, BUFSIZE, &len, NULL);
			if(!res || len == 0) break;
			callback(buffer, len, data);
		}
		res = GetExitCodeProcess(pi.hProcess, &len);
		if(res)
		{
			res = len;
		}
		else
		{
			res = GetLastError();
		}
	}
	return res;
}
