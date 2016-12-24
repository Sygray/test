#include <string>
#include <iostream>
#include <curl/curl.h>

using namespace std;

size_t callbackWrite(char *ptr, size_t size, size_t nmemb, string *stream)
{
	int dataLength = size * nmemb;
	stream->append(ptr, dataLength);
	return dataLength;
}

int main()
{
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
