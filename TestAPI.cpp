#include <string>
#include <iostream>
#include <curl/curl.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/optional.hpp>
#include <openssl/hmac.h> 
#include <openssl/sha.h>
#include <ctime>

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
	//Reading the ini file	 
	ptree pt;
	string APIKey, APISecret, URLTradeAPI, URLInfoAPI;
	try{
		read_ini("iniTestAPI.ini",pt);
		APIKey = pt.get<string>("APIKey.Key_Trade");
		APISecret = pt.get<string>("APIKey.Secret_Trade");
		URLTradeAPI = pt.get<string>("General.URL_TradeAPI");
		URLInfoAPI = pt.get<string>("General.URL_InfoAPI");
	}catch(ptree_error& e){
		cout << "ptree_error##" << e.what() << endl;
	}
	
	//Get timestamp for nonce
	time_t current;
	time(&current);
	struct tm* current_tm = localtime(&current);
	char charTime[256];
	strftime(charTime, 255, "%Y%m%d.%H%M%S", current_tm);
	string strTime = charTime;
	cout << "Nonce: " << strTime << endl;

	//Prepare request
	string strKey = "Key: " + APIKey;

    	// The data that we're going to hash using HMAC
	string strParam = "nonce=" + strTime + "&method=get_info";
    	char charParam[256];
	strncpy(charParam, strParam.c_str(), 255);

	// The key to hash
	const char* key = APISecret.c_str();

    	// Using sha1 hash engine here.
    	// You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
	unsigned char* digest;
    	digest = HMAC(EVP_sha512(), key, strlen(key), (unsigned char*)charParam, strlen(charParam), NULL, NULL);    
   	// Be careful of the length of string with the choosen hash engine. SHA1 produces a 20-byte hash value which rendered as 40 characters.
    	// Change the length accordingly with your choosen hash engine
    	char mdString[SHA512_DIGEST_LENGTH*2+1];
    	for(int i = 0; i < SHA512_DIGEST_LENGTH; i++)
	{
         	sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
 	}
	//printf("HMAC digest: %s\n", mdString);
	string strMd = mdString;
	string strSign = "Sign: "+ strMd;

	//Process curl
	CURL *curl;
	CURLcode ret;

        curl = curl_easy_init();
        string chunk;

        if (curl == NULL) {
                cerr << "curl_easy_init() failed" << endl;
                return 1;
        }

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, strKey.c_str());
	headers = curl_slist_append(headers, strSign.c_str());
        
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, charParam);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_URL, URLTradeAPI.c_str()); // /1/ticker/btc_jpy");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callbackWrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
        
	ret = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (ret != CURLE_OK) {
                cerr << "curl_easy_perform() failed." << endl;
                return 1;
        }

        cout << chunk << endl;

	return 0;
}
