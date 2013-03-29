#include <QtNetwork>
#include <iostream>
#include <curl/curl.h>
#include "kerberos.h"
#include <string.h>

static size_t wf(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	char *str = (char *)ptr;
	int *ok = (int *)userdata;
	if(size * nmemb > 1 && !strncmp(str, "ok", strlen("ok")) && (str[2] == '\0' || str[2] == '\n'))
	{
		*ok = 1;
	}
        return size * nmemb;
}

static size_t wh( void *ptr, size_t size, size_t nmemb, void *userdata)
{
	char *str = (char *)ptr;
	if(!strncmp(str, "Set-Cookie:", strlen("Set-Cookie:")))
	{
		std::cout.write(str, size * nmemb);
		if(str[size * nmemb - 1] != '\n')
		{
			std::cout << "\r\n";
		}
	}
	return size * nmemb;
}

static int _kerberos(const char *testauth)
{
	QTemporaryFile tf;
	const char *cookie_jar;
	cookie_jar = tf.fileName().toStdString().c_str();
	int ok = 0;
	int res = 0;
	CURLcode curlres;
	curlres = curl_global_init(CURL_GLOBAL_ALL);
	if(curlres)
	{
		res = curlres;
	}
	else
	{
		CURL *ceh;
		ceh = curl_easy_init();
		curl_easy_setopt(ceh, CURLOPT_URL, testauth);
		curl_easy_setopt(ceh, CURLOPT_NOPROGRESS, 1);
		curl_easy_setopt(ceh, CURLOPT_HEADERFUNCTION, wh);
		curl_easy_setopt(ceh, CURLOPT_WRITEFUNCTION, wf);
		curl_easy_setopt(ceh, CURLOPT_WRITEDATA, &ok);
		curl_easy_setopt(ceh, CURLOPT_FAILONERROR, 1);
		curl_easy_setopt(ceh, CURLOPT_BUFFERSIZE, 4096);
		curl_easy_setopt(ceh, CURLOPT_TCP_NODELAY, 1);
		curl_easy_setopt(ceh, CURLOPT_USERPWD, ":");
		curl_easy_setopt(ceh, CURLOPT_HTTPAUTH, CURLAUTH_GSSNEGOTIATE);
		curl_easy_setopt(ceh, CURLOPT_TIMEOUT, 4 * 60);
		curl_easy_setopt(ceh, CURLOPT_CONNECTTIMEOUT, 4 * 60);
		curl_easy_setopt(ceh, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(ceh, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(ceh, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(ceh, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(ceh, CURLOPT_MAXREDIRS, 5);
		curl_easy_setopt(ceh, CURLOPT_COOKIEJAR, cookie_jar);
		res = curl_easy_perform(ceh);
		curl_easy_cleanup(ceh);
		if(!res)
		{
			res = !ok;
		}
		std::cout << "Kerberos: " << ok << " res: " << res << "\n";
	}
	return res;
}

PacificaKerberosAuth::PacificaKerberosAuth(bool skip_kerberos)
{
	_skip_kerberos = skip_kerberos;
}

void PacificaKerberosAuth::process(QHash<QString, QString> *services)
{
	const char *testauth;
	foreach(const QString &key, services->keys())
	{
		std::cout << "Service: " << key.toStdString() << "=" << services->value(key).toStdString() << "\r\n";
	}
	testauth = services->value("testauth").toStdString().c_str();
	std::cout << "URL: " << testauth << "\r\n";
	if(!_skip_kerberos)
	{
		//Run this in a seperate thread.
		int res = _kerberos(testauth);
	}
	ready(services);
}
