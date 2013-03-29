package auth

import os "os"
import io "io"
import fmt "fmt"
import url "net/url"
import exec "os/exec"
import http "net/http"
import bufio "bufio"
import strings "strings"
import tls "crypto/tls"

/*FIXME Remove dum cookie jar when GO implements a real one.*/
type DumCookieJar struct {
	pcookies []*http.Cookie
}

func (self *DumCookieJar) SetCookies(u *url.URL, cookies []*http.Cookie) {
	newlen := len(self.pcookies) + len(cookies)
	temp := make([]*http.Cookie, newlen, newlen)
	copy(temp, self.pcookies)
	for i := 0; i < len(cookies); i++ {
		temp[len(self.pcookies) + i] = cookies[i]
	}
	self.pcookies = temp
}

func (self *DumCookieJar) Cookies(u *url.URL) []*http.Cookie {
	return self.pcookies
}

/*END Dum cookie jar.*/

func cookie_process(cookiejar http.CookieJar, surl string, cookiedata string) {
	if(cookiedata == "") {
		return
	}
	cookiedata = "HTTP/1.0 200 OK\r\n" + cookiedata + "\r\n\r\n"
	req, err := http.NewRequest("GET", surl, nil)
	if(err != nil) {
		fmt.Println(err)
		return
	}
	res, err := http.ReadResponse(bufio.NewReader(strings.NewReader(cookiedata)), req)
	if(err != nil) {
		fmt.Println(err)
		return
	}
	cookies := res.Cookies();
	turl, err := url.Parse(surl)
	cookiejar.SetCookies(turl, cookies)
}

type Auth struct {
	Services map[string]string
	Settings map[string]string
	CookieJar http.CookieJar
}

func NewAuth(read io.Reader) *Auth {
	res := &Auth {
		Services: make(map[string]string, 1),
		Settings: make(map[string]string, 1),
		CookieJar: &DumCookieJar{},
	}
	if(read == nil) {
		var err error
		cmd := exec.Command("pacificaauth")
/*FIXME doing this instead of cmd.StdoutPipe*/
		data, err := cmd.Output()
		if(err != nil) {
			fmt.Println(err)
			return nil
		}
		read = strings.NewReader(string(data))
/*FIXME why is go broken here? */	
/*
		pipe_read, err := cmd.StdoutPipe()
		if(err != nil) {
			fmt.Println(err)
			return nil
		}
		err = cmd.Start()
		if(err != nil) {
			fmt.Println(err)
			return
		}
//FIXME and wait on process in the end.
*/
	}
	cookiedata := ""
	pipebuf := bufio.NewReader(read)
	surl := ""
	for {
		data, isPrefix, err := pipebuf.ReadLine();
		if(isPrefix) {
			fmt.Println("Line too long");
			return nil
		} else if(err == io.EOF) {
			break;
		} else if(err != nil) {
			fmt.Println(err)
			return nil
		}
		str := string(data)
		if(strings.HasPrefix(str, "URL:")) {
			if(surl != "") {
				cookie_process(res.CookieJar, surl, cookiedata)
			}
			cookiedata = ""
			surl = strings.TrimSpace(str[len("URL:"):])
		} else if(strings.HasPrefix(str, "Set-Cookie:")) {
			cookiedata += str
			cookiedata += "\r\n"
		} else if(strings.HasPrefix(str, "Service:")) {
			tmpstr := strings.TrimSpace(str[len("Service:"):])
			ind := strings.Index(tmpstr, "=")
			if(ind != -1) {
				res.Services[tmpstr[:ind]] = tmpstr[ind + 1:]
			}
		} else if(strings.HasPrefix(str, "Setting:")) {
			tmpstr := strings.TrimSpace(str[len("Service:"):])
			ind := strings.Index(tmpstr, "=")
			if(ind != -1) {
				res.Settings[tmpstr[:ind]] = tmpstr[ind + 1:]
			} else {
				res.Settings[tmpstr] = "true"
			}
		}
	}
	cookie_process(res.CookieJar, surl, cookiedata)
	return res
}

func (auth *Auth) NewClient() *http.Client {
	var client *http.Client;
	var transport *http.Transport;
	if(auth.Settings["insecure"] != "" || auth.Settings["proxy_host"] != "") {
		transport = &http.Transport {}
	}
	if(auth.Settings["insecure"] != "") {
//FIXME
		os.Setenv("HTTP_PROXY", "http://127.0.0.1:10004")
		os.Setenv("NO_PROXY", "")
		var config *tls.Config = &tls.Config {
			InsecureSkipVerify: true,
		}
		transport.TLSClientConfig = config
	}
	if(auth.Settings["proxy_host"] != "") {
			turl, _ := url.Parse("http://" + auth.Settings["proxy_host"] + ":" + auth.Settings["proxy_port"])
			transport.Proxy = http.ProxyURL(turl)
	}
	if(auth.Settings["insecure"] != "" || auth.Settings["proxy_host"] != "") {
		client = &http.Client {
			Jar: auth.CookieJar,
			Transport: transport,
		}
	} else {
		client = &http.Client {
			Jar: auth.CookieJar,
		}
	}
	return client
}
