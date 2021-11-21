//------------------------------------------------------------------
// util.h
//
// Author:           JuanJakobo
// Date:             04.08.2020
// Description:      Various utility methods
//-------------------------------------------------------------------

#ifndef UTIL
#define UTIL

#include "inkview.h"

#include <string>

enum Action
{
    IWriteSecret,
    IReadSecret,
    IWriteString,
    IReadString
};

class Util
{
public:
    /**
    * Handles the return of curl command
    * 
    */
    static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);

    /**
    * Saves the return of curl command
    * 
    */
    static size_t writeData(void *ptr, size_t size, size_t nmemb, FILE *stream);

    /**
    * Checks if a network connection can be established
    * 
    */
    static bool connectToNetwork();

    /**
    * Reads an string from the config file 
    *
    * @action
    * @param name name of the config that shall be read
    * @param value value that shall be written to the config
    *  
    * @return string that has been found in the config file 
    */
    static std::string accessConfig(const Action &action, const std::string &name, const std::string &value = std::string());
    

    static std::string getData(const std::string &url);

    /**
    * Opens a webpage in a browser 
    * 
    * @param url webpage address that shall be opened
    */
    static void openInBrowser(const std::string &url);

    static void decodeHTML(std::string &data);

    static void replaceAll(std::string &data, const std::string &replace, const std::string &by);

private:
    Util() {}
};
#endif
