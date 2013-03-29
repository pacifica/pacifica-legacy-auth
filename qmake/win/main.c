#include <windows.h>
#include <stdio.h>

void my_wstrcpy(WCHAR *dest, WCHAR *src, int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		dest[i] = src[i];
	}
	dest[i] = '\0';
}

/*FIXME not tested for anything other then direct = */
int wstrncmp(WCHAR *a, WCHAR *b, int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		if(a[i] == '\0')
		{
			return -1;
		}
		else if(b[i] == '\0')
		{
			return 1;
		}
		if(a[i] - b[i])
		{
			return a[i] - b[i];
		}
	}
	return 0;
}

#define BUFFSIZE 1024 * 4
int main(int argc, char *argv[])
{
	WCHAR *wpath = TEXT("c:\\Program Files (x86)\\Pacifica\\Auth");
	int wpath_size = strlen("c:\\Program Files (x86)\\Pacifica\\Auth");
	LONG res;
	DWORD size = BUFFSIZE * sizeof(WCHAR);
	DWORD type = REG_EXPAND_SZ;
	HKEY key;
	WCHAR data[BUFFSIZE];
	WCHAR *final_path;
	WCHAR *start;
	WCHAR *end;
	int offset = 0;
	int i;
	int remove = 0;
	int args_start = 1;
//	FILE *file;
	if(argc < 2)
	{
		return -1;
	}
	if(argv[1][0] == '/' && argv[1][1] == 'r' && argv[1][2] == '\0')
	{
		remove = 1;
		args_start = 2;
		if(argc < 3)
		{
			return -1;
		}
	}
	wpath_size = strlen(argv[args_start]);
	wpath = (WCHAR*)LocalAlloc(LPTR, sizeof(WCHAR) * (wpath_size + 1));
	if(!wpath)
	{
		return -1;
	}
	for(i = 0; i < wpath_size + 1; i++)
	{
		wpath[i] = argv[args_start][i];
	}
	for(i = wpath_size - 1; i >= 0; i--)
	{
		if(wpath[i] == ' ' || wpath[i] == '\\')
		{
			wpath[i] = '\0';
			wpath_size--;
		}
		else
		{
			break;
		}
	}
//	file = fopen("c:\\pacificaauth\\foo.txt", "w");
//		fprintf(file, "type %d\n", remove);
//	for(i = args_start; i < argc; i++)
//	{
//		fprintf(file, "%s\n", argv[i]);
//	}
//	fclose(file);
	res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"), NULL, KEY_ALL_ACCESS, &key);
	if(res != ERROR_SUCCESS)
	{
		printf("%ld\n", res);
		return -1;
	}
	res = RegQueryValueEx(key, TEXT("Path"), NULL, &type, (LPBYTE) data, &size);
	if(res != ERROR_SUCCESS)
	{
		printf("%ld\n", res);
		return -1;
	}
	if(((WCHAR*)data)[(size - 1)/2] == (WCHAR)'\0')
	{
		size -= 2;
	}
	final_path = (WCHAR*)LocalAlloc(LPTR, sizeof(WCHAR) * (size + 4 + wpath_size));
	if(!final_path)
	{
		return -1;
	}
	for(start = data; start < data + (size/2);)
	{
		for(end = start + 1; *end != (WCHAR)'\0' && *end != (WCHAR)';'; end++);
		if(wstrncmp(start, wpath, end-start) && *start != '\0')
		{
			if(start != data)
			{
				final_path[offset] = ';';
				offset++;
			}
			my_wstrcpy(final_path + offset, start, end-start);
			offset += end-start;
		}
		start = end + 1;
	}
	if(!remove)
	{
		if(start != data)
		{
			final_path[offset] = ';';
			offset++;
		}
		my_wstrcpy(final_path + offset, wpath, wpath_size);
		offset += wpath_size;
	}
	final_path[offset] = '\0';
	wprintf(TEXT("%s\n"), final_path);
	res = RegSetValueEx(key, TEXT("Path"), NULL, type, (LPBYTE) final_path, offset * sizeof(WCHAR));
	if(res != ERROR_SUCCESS)
	{
		printf("%ld\n", res);
		return -1;
	}
	RegCloseKey(key);
	return 0;
}
