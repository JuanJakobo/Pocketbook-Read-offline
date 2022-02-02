//------------------------------------------------------------------
// pocket.h
//
// Author:           JuanJakobo
// Date:             22.04.2021
// Description:
//
//-------------------------------------------------------------------

#ifndef POCKET
#define POCKET

#include "pocketModel.h"

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

const std::string POCKET_URL = "https://getpocket.com/v3/";
const std::string CONSUMER_KEY = "<CONSUMER_KEY>";

class Pocket
{
    public:
        /**
         * creatas a new Miniflux object containing the url and token to access the api
         *  
         */
        Pocket();

        /**
         * Retrieve a Users Pocket Data
         *
         * @return vetor of PocketItems that has been received
         */
        std::vector<PocketItem> getItems(bool unread, bool archive, bool favorited);



    private:
        std::string _accessToken;

        void loginDialog();
        std::string getCode();
        std::string getAccessToken(const std::string &code);
        nlohmann::json post(const std::string &apiEndpoint, const std::string &data);

};
#endif
