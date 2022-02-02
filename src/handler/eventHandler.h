//------------------------------------------------------------------
// eventHandler.h
//
// Author:           JuanJakob
// Date:             22.04.2021
// Description:      Handles all events and directs them
//-------------------------------------------------------------------

#ifndef EVENT_HANDLER
#define EVENT_HANDLER

#include "inkview.h"

#include "mainMenu.h"
#include "contextMenu.h"

#include "pocket.h"
#include "pocketModel.h"
#include "pocketView.h"

#include "sqliteConnector.h"

#include <string>
#include <memory>
#include <map>
#include <vector>

enum Views
{
    DEFAULTVIEW,
    PKVIEW
};

//TODO upgrade 
const std::string CONFIG_FOLDER = "/mnt/ext1/system/config/readoffline";
const std::string CONFIG_PATH = CONFIG_FOLDER + "/readoffline.cfg";
const std::string ARTICLE_FOLDER = "/mnt/ext1/readoffline";
const std::string IMAGE_FOLDER = "/mnt/ext1/readoffline/img";
const std::string DB_PATH = ARTICLE_FOLDER + "/data.db";

class EventHandler
{
public:
    /**
        * Defines fonds, sets global Event Handler and starts new content 
        */
    EventHandler();

    ~EventHandler();

    /**
        * Handles events and redirects them
        * 
        * @param type event type
        * @param par1 first argument of the event
        * @param par2 second argument of the event
        * @return int returns if the event was handled
        */
    int eventDistributor(const int type, const int par1, const int par2);

private:
    static std::unique_ptr<EventHandler> _eventHandlerStatic;
    std::unique_ptr<PocketView> _pocketView;
    std::vector<PocketItem> _items;
    std::unique_ptr<Pocket> _pocket;
    MainMenu _menu = MainMenu("Readoffline");
    ContextMenu _contextMenu = ContextMenu();
    SqliteConnector _sqliteCon = SqliteConnector(DB_PATH);
    Views _currentView;
    int _pocketViewShownPage = 1;

    void loginDialog();

    /**
        * Function needed to call C function, redirects to real function
        * 
        *  @param index int of the menu that is set
        */
    static void mainMenuHandlerStatic(const int index);

    /**
        * Handles menu events and redirects them
        * 
        * @param index int of the menu that is set
        */
    void mainMenuHandler(const int index);


    /**
        * Function needed to call C function, redirects to real function
        * 
        *  @param index int of the menu that is set
        */
    static void contextMenuHandlerStatic(const int index);

    /**
        * Handlescontext  menu events and redirects them
        * 
        * @param index int of the menu that is set
        */

    void contextMenuHandler(const int index);

    /**
        * Handles pointer Events
        * 
        * @param type event type
        * @param par1 first argument of the event
        * @param par2 second argument of the event
        * @return int returns if the event was handled
        */
    int pointerHandler(const int type, const int par1, const int par2);

    /**
        * Handles key Events
        * 
        * @param type event type
        * @param par1 first argument of the event (is the key)
        * @param par2 second argument of the event
        * @return int returns if the event was handled
        */
    int keyHandler(const int type, const int par1, const int par2);


    bool drawPocketItems(const std::vector<PocketItem> &pocketItems);

    /**
     * 
     * Draws the pocket items to an ListView
     * 
     */
    void filterAndDrawPocket(const std::string &filter);

};
#endif
