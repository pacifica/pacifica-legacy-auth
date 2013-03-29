using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Net;

namespace Pacifica.Test
{
	public class Authenticate
	{
		byte[] cookiedata;
		public Authenticate()
		{
			cookiedata = new byte[0];
		}
		public delegate void callback([MarshalAs(UnmanagedType.LPArray, SizeParamIndex=1)] byte[] buffer, int size);
		[DllImport ("pacificaauth.dll")]
		private static extern int pacifica_auth(callback cb);
		private void mycallback(byte[] buffer, int size)
		{
			byte[] tmp;
			tmp = new byte[cookiedata.Length + buffer.Length];
			Buffer.BlockCopy(cookiedata, 0, tmp, 0, cookiedata.Length);
			Buffer.BlockCopy(buffer, 0, tmp, cookiedata.Length, buffer.Length);
			cookiedata = tmp;
		}
		public int process(CookieContainer cc)
		{
			System.Uri uri = null;
			string url = null;
			int res;
			res = pacifica_auth(mycallback);
			if(res == 0)
			{
				string test = System.Text.Encoding.ASCII.GetString(cookiedata);
				string[] lines = test.Split(new Char[] {'\n', '\r'});
				foreach(string lineraw in lines)
				{
					string line = lineraw;
					int cmplen = "URL:".Length;
					if(line.Length >= cmplen && line.Substring(0, cmplen).ToLower().Equals("URL:".ToLower()))
					{
						url = lineraw.Substring(cmplen, lineraw.Length - cmplen).Trim();
						uri = new System.Uri(url);
						continue;
					}
					cmplen = "Set-Cookie:".Length;
					if(line.Length >= cmplen && line.Substring(0, cmplen).ToLower().Equals("Set-Cookie:".ToLower()))
					{
						line = lineraw.Substring(cmplen, lineraw.Length - cmplen).Trim();
						cc.SetCookies(uri, line);
					}
				}
			}
			return res;
		}
	}
	public class HelloWorld
	{
		public static void Main(string[] args)
		{
			Uri uri = new Uri("http://myemsl-dev5.emsl.pnl.gov/");
			CookieContainer cc = new CookieContainer();
			Authenticate a = new Authenticate();
			int res;
			Console.WriteLine("T1a");
			res = a.process(cc);
			if(res == 0)
			{
				Console.WriteLine("Cookies {0}", cc.GetCookieHeader(uri));
				Console.WriteLine("Cookie Count {0} {1}", cc.GetCookies(uri).Count, cc.Capacity);
				foreach(Cookie cookie in cc.GetCookies(uri))
				{
					Console.WriteLine("Cookie {0}", cookie);
				}
				test_auth(new Uri("http://myemsl-dev5.emsl.pnl.gov/myemsl/testauth/"), cc);
				logout(new Uri("http://myemsl-dev5.emsl.pnl.gov/myemsl/logout/"), cc);
			}
			Console.WriteLine("T2");
			Console.WriteLine(res);
		}
		public static int test_auth(Uri uri, CookieContainer cc)
		{
			int res = 1;
			HttpWebRequest request = WebRequest.Create(uri) as HttpWebRequest;
			request.CookieContainer = cc;
			using(HttpWebResponse response = request.GetResponse() as HttpWebResponse)
			{
				if(response.StatusCode == HttpStatusCode.OK)
				{
					Char[] read = new Char[10];
					StreamReader stream = new StreamReader(response.GetResponseStream(), System.Text.Encoding.GetEncoding("utf-8"));
					int len = stream.Read(read, 0, 10);
					if((len == 2 || (len > 2 && read[2] == '\n')) && read[0] == 'o' && read[1] == 'k')
					{
						res = 0;
						Console.WriteLine("OK!");
					}
				}
			}
			return res;
		}
		public static int logout(Uri uri, CookieContainer cc)
		{
			int res = 1;
			HttpWebRequest request = WebRequest.Create(uri) as HttpWebRequest;
			request.CookieContainer = cc;
			using(HttpWebResponse response = request.GetResponse() as HttpWebResponse)
			{
				if(response.StatusCode == HttpStatusCode.OK)
				{
					Console.WriteLine("OK!");
				}
			}
			return res;
		}
	} 
}
