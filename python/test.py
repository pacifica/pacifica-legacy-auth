#!/usr/bin/python

import urllib2
import StringIO
import cookielib
import mimetools
import subprocess

class wrapper_response:
	def __init__(self):
		self.cookie_data = ''
	def add_cookie_data(self, string):
		self.cookie_data += string
	def info(self):
		return mimetools.Message(StringIO.StringIO(self.cookie_data))

def pacifica_auth(cookiejar):
	pipe = subprocess.Popen("./pacificaauth", stdout=subprocess.PIPE).stdout
	url = None
	response = None
	for line in pipe.readlines():
		if line.startswith("URL:"):
			if response:
				cookiejar.extract_cookies(response, urllib2.Request(url))
			response = wrapper_response()
			url = line[len("URL:"):].strip()
		if line.startswith("Set-Cookie:"):
			response.add_cookie_data(line)
	if response:
		cookiejar.extract_cookies(response, urllib2.Request(url))

def main():
	cookiejar = cookielib.CookieJar()
	pacifica_auth(cookiejar)
	opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cookiejar))
	r = opener.open("http://myemsl-dev5.emsl.pnl.gov/myemsl/testauth/")
	if r.getcode() == 200:
		line = r.readline()
		if line == 'ok\n':
			print "OK!"
	r = opener.open("http://myemsl-dev5.emsl.pnl.gov/myemsl/logout/")
	if r.getcode() == 200:
		print "OK!"

if __name__ == "__main__":
	main()
