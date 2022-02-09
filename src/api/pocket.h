//------------------------------------------------------------------
// pocket.h
//
// Author:           JuanJakobo
// Date:             22.04.2021
// Description: Interface to the pocket API
//
//-------------------------------------------------------------------

#ifndef POCKET
#define POCKET

#include "pocketModel.h"

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

enum PocketAction {
    ADD, //Add a new item
    ARCHIVE, //Move an item to the archive
    READD, //Unarchive an item
    FAVORITE, //Mark an item as favorite
    UNFAVORITE, //unfavorite an item
    DELETE //permanently remove an item
};

const std::string POCKET_URL = "https://getpocket.com/v3/";
const std::string CONSUMER_KEY = "<CONSUMER_KEY>";

class Pocket
{
    public:
        /**
         * Creates a new pocket object containing token to access the api
         *
         */
        Pocket();

        /**
         * Retrieve a Users Pocket Data
         *
         * @return vector of PocketItems that has been received
         */
        std::vector<PocketItem> getItems();

        /**
        * Gets the text and images, creates an html out of it and stores the path inside the item
        *
        * @param item for that the text shall be downloaded
        */
        void getText(PocketItem *item);

        /**
         * Modify a Users Pocket Data
         *
         * @param action that shall be performed
         * @param ids the ids that shall be modified
         */
        void sendItems(PocketAction action, const std::vector<std::string> &ids);

        /**
         * Modify a single item
         *
         * @param action that shall be performed
         * @param id the id that shall be modified
         */
        void sendItem(PocketAction action, const std::string &id);


    private:
        std::string _accessToken;

        /**
         * Opens the login dialog
         */
        void loginDialog();

        /**
         * Get the code to start the login process
         *
         * @return the code that has been received
         */
        std::string getCode();

        /**
         * Receives the AccessToken and the username and stores these
         *
         * @param the code that has been received from the net
         */
        void getAccessToken(const std::string &code);

        /**
         * Determines the action from the enum
         *
         * @param receivces the action enum
         *
         * @returns the action as string
         */
        std::string determineAction(PocketAction action);

        /**
         * Accesses the Pocketapi and returns the result as a json
         *
         * @param apiEndpoint the endpoint that shall be accessed
         * @param data the post data
         *
         * @return the result as a json
         */
        nlohmann::json post(const std::string &apiEndpoint, const std::string &data);

};
#endif
