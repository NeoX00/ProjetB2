#include "Url.h"

using namespace utils;
using namespace std;


Url::Url(string uri)
{
    this->uri = uri;
    parse();
}


void Url::parse()
{
    bool find=false;
    url ="";
    int findHttps = uri.find("https:");
    if(findHttps != string::npos)
    {
        //url = "https://";
        uri.erase(0,8+findHttps);//on enleve la premiere partie
        find = true;
    }
    else
    {
        int findHttp = uri.find("http:");
        if(findHttp != string::npos)
        {
            //url = "http://";
            uri.erase(0,7+findHttp);//on enleve la premiere partie
            find = true;
        }
    }

    //si ca commence bien par http
    if(find)
    {
        int findSlash = uri.find("/");
        if(findSlash != string::npos)
        {
            url = uri.substr(0,findSlash);
            get = uri.substr(findSlash,uri.size()-findSlash);
        }
        else
        {
            url=uri;
            get="/";
        }


    }
    else
    {
        url = "";
        get = "";
    }
}


Url::~Url()
{
    //dtor
}
