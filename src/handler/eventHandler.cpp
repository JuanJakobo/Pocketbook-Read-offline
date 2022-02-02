//------------------------------------------------------------------
// eventHandler.cpp
//
// Author:           JuanJakobo
// Date:             22.04.2021
//
//-------------------------------------------------------------------

#include "eventHandler.h"
#include "inkview.h"

#include "mainMenu.h"
#include "contextMenu.h"

#include "pocket.h"
#include "pocketModel.h"
#include "pocketView.h"

#include "util.h"
#include "log.h"
#include "sqliteConnector.h"

#include <string>
#include <memory>
#include <map>
#include <vector>

#include <iostream>
#include <fstream>

using std::string;
using std::vector;

std::unique_ptr<EventHandler> EventHandler::_eventHandlerStatic;
pthread_mutex_t mutexEntries;

EventHandler::EventHandler()
{
    //create an copy of the eventhandler to handle methods that require static functions
    _eventHandlerStatic = std::unique_ptr<EventHandler>(this);

    if (iv_access(CONFIG_FOLDER.c_str(), W_OK) != 0)
        iv_mkdir(CONFIG_FOLDER.c_str(), 0777);
    if (iv_access(ARTICLE_FOLDER.c_str(), W_OK) != 0)
        iv_mkdir(ARTICLE_FOLDER.c_str(), 0777);

    if (iv_access(IMAGE_FOLDER.c_str(), W_OK) != 0)
        iv_mkdir(IMAGE_FOLDER.c_str(), 0777);

    _pocket = std::unique_ptr<Pocket>(new Pocket());
    _items = _pocket->getItems(true,false,false);
    if(_items.empty())
        Log::writeInfoLog("no items");
    else
        drawPocketItems(_items);

}
     
EventHandler::~EventHandler()
{
    Log::writeInfoLog("delete eventHandler");
}

int EventHandler::eventDistributor(const int type, const int par1, const int par2)
{
    if (ISPOINTEREVENT(type))
        return EventHandler::pointerHandler(type, par1, par2);
    else if (ISKEYEVENT(type))
        return EventHandler::keyHandler(type, par1, par2);

    return 0;
}

void EventHandler::mainMenuHandlerStatic(const int index)
{
    _eventHandlerStatic->mainMenuHandler(index);
}

void EventHandler::mainMenuHandler(const int index)
{
    switch (index)
    {
        //show downloaded
        case 101:
            {
                break;
            }
            //show unread
        case 102:
            {
                _items = _pocket->getItems(true,false,false);
                if(_items.empty())
                    Log::writeInfoLog("no items");
                else
                    drawPocketItems(_items);
                break;
            }
            //show starred
        case 103:
            {
                _items = _pocket->getItems(true,true,true);
                if(_items.empty())
                    Log::writeInfoLog("no items");
                else
                    drawPocketItems(_items);
                break;
            }
            //sync items
        case 104:
            {
                break;
            }
            //Mark as read till page
        case 105:
            {
                break;
            }
        case 107:
            {
                Message(ICON_INFORMATION, "Info", "Info", 1200);
                break;
            }
            //Exit
        case 108:
            CloseApp();
            break;
        default:
            break;
    }
}

void EventHandler::contextMenuHandlerStatic(const int index)
{
    _eventHandlerStatic->contextMenuHandler(index);
}

void EventHandler::contextMenuHandler(const int index)
{
    switch (index)
    {
        case 101:
            {
                break;
            }
            //Mark/Unmark to download
        case 102:
            {
                break;
            }
            //Unstar/Star
        case 103:
            {
                break;
            }
            //Browser
        case 104:
            {
                Util::openInBrowser(_pocketView->getCurrentEntry()->url);
                break;
            }
        default:
            {
                _pocketView->invertCurrentEntryColor();
                break;
            }
    }
}

int EventHandler::pointerHandler(const int type, const int par1, const int par2)
{
    if (type == EVT_POINTERLONG)
    {
        if (_currentView == Views::PKVIEW)
        {
            if (_pocketView->checkIfEntryClicked(par1, par2))
                _pocketView->invertCurrentEntryColor();

            string downloaded;
            if(_pocketView->getCurrentEntry()->downloaded == IsDownloaded::TOBEDOWNLOADED) 
                downloaded = "Remove download mark";
            else if(_pocketView->getCurrentEntry()->downloaded == IsDownloaded::NOTDOWNLOADED)
                downloaded = "Add download mark";
            else if(_pocketView->getCurrentEntry()->downloaded == IsDownloaded::DOWNLOADED)
                downloaded = "Remove item";

            _contextMenu.createMenu(par2, EventHandler::contextMenuHandlerStatic,true , downloaded);
            return 1;
        }
    }
    else if (type == EVT_POINTERUP)
    {
        //if menu is clicked
        if (IsInRect(par1, par2, _menu.getMenuButtonRect()) == 1)
        {
            auto mainView = true;

            return _menu.createMenu(mainView, EventHandler::mainMenuHandlerStatic);
        }
        else if (_currentView == Views::PKVIEW)
        {
            if (_pocketView->checkIfEntryClicked(par1, par2))
            {
                _pocketView->invertCurrentEntryColor();


                int dialogResult = DialogSynchro(ICON_QUESTION, "Action",(_pocketView->getCurrentEntry()->title + "\n" + _pocketView->getCurrentEntry()->excerpt + "...").c_str(), "Read article", "Cancel", NULL);
                switch (dialogResult)
                {
                    case 1:
                        {
                            Message(ICON_INFORMATION, "Information","currently not implemented", 1000);
                            break;
                        }
                    default:
                        break;
                }
                _pocketView->invertCurrentEntryColor();

            }
            return 1;
        }
    }
    return 0;
}

int EventHandler::keyHandler(const int type, const int par1, const int par2)
{
    //menu button
    if (type == EVT_KEYPRESS)
    {

        if (_currentView == Views::PKVIEW)
        {
            if (par1 == 23)
            {
                _pocketView->firstPage();
                return 1;
            }
            //left button -> pre page
            else if (par1 == 24)
            {
                _pocketView->prevPage();
                return 1;
            }
            //right button -> next page
            else if (par1 == 25)
            {
                _pocketView->nextPage();
                return 1;
            }
        }
    }

    return 0;
}

bool EventHandler::drawPocketItems(const vector<PocketItem> &pocketItems)
{
    if (pocketItems.size() > 0)
    {
        //_sqliteCon.insertMfEntries(mfEntries);
        _pocketView.reset(new PocketView(_menu.getContentRect(),pocketItems,1));
        _pocketView->draw();
        _currentView = Views::PKVIEW;
        return true;
    }
    else
    {
        FillAreaRect(_menu.getContentRect(), WHITE);
        DrawTextRect2(_menu.getContentRect(), "no entries to show");
        _pocketView.reset();
        _currentView = Views::DEFAULTVIEW;
        PartialUpdate(_menu.getContentRect()->x, _menu.getContentRect()->y, _menu.getContentRect()->w, _menu.getContentRect()->h);
        return false;
    }

}

void EventHandler::filterAndDrawPocket(const string &filter)
{
    if (!filter.empty())
    {
        try{
            if(!Util::connectToNetwork())
                return;
            /*
            vector<PocketItem> mfEntries = _pocket->getEntries(filter);
            vector<PocketItem> oldEntries = _sqliteCon.selectMfEntries();
            for(size_t i = 0; i < mfEntries.size(); i++)
            {
                for(size_t j = 0; j < oldEntries.size();j++)
                {
                    if(mfEntries.at(i).id == oldEntries.at(j).id)
                    {
                        mfEntries.at(i).downloaded = oldEntries.at(j).downloaded;
                        break;
                    }
                }
            }
            drawPocketEntries(mfEntries);
            */
        }
        catch (const std::exception &e)
        {
            //TODO use errro message 
            Message(ICON_INFORMATION, "Error","An error occured", 1000);
        }
    }
}
