#include <string>
#include <iostream>
#include <curl/curl.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/optional.hpp>

using namespace std;
using namespace boost::property_tree;

size_t callbackWrite(char *ptr, size_t size, size_t nmemb, string *stream)
{
	int dataLength = size * nmemb;
	stream->append(ptr, dataLength);
	return dataLength;
}

int main()
{
	 
	ptree pt;
	try{
		read_ini("iniTestAPI.ini",pt);
		cout << "URL_InfoAPI:" << pt.get<string>("General.URL_InfoAPI") << endl;
		cout << "URL_TradeAPI:" << pt.get<string>("General.URL_TradeAPI") << endl;
		cout << "Key_Master:" << pt.get<string>("APIKey.Key_Master") << endl;
		cout << "Key_Trade:" << pt.get<string>("APIKey.Key_Trade") << endl;
	}catch(ptree_error& e){
		cout << "ptree_error##" << e.what() << endl;
	}
	
	CURL *curl;
	CURLcode ret;

        curl = curl_easy_init();
        string chunk;

        if (curl == NULL) {
                cerr << "curl_easy_init() failed" << endl;
                return 1;
        }

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.zaif.jp/api/1/ticker/btc_jpy");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callbackWrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
        ret = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (ret != CURLE_OK) {
                cerr << "curl_easy_perform() failed." << endl;
                return 1;
        }

        cout << chunk << endl;

	//std::cout << "Hello Wolrd!\n";
	
	return 0;
}
