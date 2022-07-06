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

    _items = _sqliteCon.selectPocketEntries(PIsDownloaded::PDOWNLOADED);
    for(size_t i = 0; i < _items.size(); i++)
    {
        if (iv_access(_items.at(i).path.c_str(), W_OK) != 0)
        {
            _items.at(i).downloaded =  PIsDownloaded::PNOTDOWNLOADED;
            _sqliteCon.updatePocketItem(_items.at(i).id,UpdateAction::IDOWNLOADED, PIsDownloaded::PNOTDOWNLOADED);
            break;
        }
    }
    drawPocketItems(_items);

}

EventHandler::~EventHandler()
{
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
                _items = _sqliteCon.selectPocketEntries(PIsDownloaded::PDOWNLOADED);
                drawPocketItems(_items);
                break;
            }
            //show unread
        case 102:
            {
                filterAndDrawPocket(IStatus::UNREAD,false);
                break;
            }
            //show starred
        case 103:
            {
                filterAndDrawPocket(IStatus::UNREAD,true);
                break;
            }
            //Mark as read till page
        case 104:
            {
                _pocket->sendItems(PocketAction::ARCHIVE, _pocketView->getEntriesTillPage());
                _items = _sqliteCon.selectPocketEntries(PIsDownloaded::PDOWNLOADED);
                drawPocketItems(_items);
                break;
            }
        case 105:
            {
                Message(ICON_INFORMATION, "Info", "Info", 1200);
                break;
            }
            //Exit
        case 106:
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
    try{

        switch (index)
        {
            //Mark/Unmark to download
            case 101:
                {
                    if(_pocketView->getCurrentEntry()->downloaded != PIsDownloaded::PDOWNLOADED)
                    {
                        _pocket->getText(_pocketView->getCurrentEntry());
                        _pocketView->getCurrentEntry()->downloaded = PIsDownloaded::PDOWNLOADED;
                    }
                    else
                    {
                        remove(_pocketView->getCurrentEntry()->path.c_str());
                        string title = _pocketView->getCurrentEntry()->title;
                        string cmd = "rm -rf \"" + ARTICLE_FOLDER + "/img/" + title + "/\"";
                        system(cmd.c_str());
                        _pocketView->getCurrentEntry()->downloaded = PIsDownloaded::PNOTDOWNLOADED;
                    }
                    _sqliteCon.updatePocketItem(_pocketView->getCurrentEntry()->id, UpdateAction::IDOWNLOADED, _pocketView->getCurrentEntry()->downloaded);
                    _pocketView->reDrawCurrentEntry();
                    HideHourglass();
                    break;
                }
                //Archive/readd
            case 102:
                {
                    if(_pocketView->getCurrentEntry()->status == IStatus::UNREAD)
                    {
                        _pocket->sendItem(PocketAction::ARCHIVE, _pocketView->getCurrentEntry()->id);
                        _pocketView->getCurrentEntry()->status = IStatus::ARCHIVED;
                    }
                    else if(_pocketView->getCurrentEntry()->status == IStatus::ARCHIVED)
                    {
                        _pocket->sendItem(PocketAction::READD, _pocketView->getCurrentEntry()->id);
                        _pocketView->getCurrentEntry()->status = IStatus::UNREAD;
                    }
                    _sqliteCon.updatePocketItem(_pocketView->getCurrentEntry()->id, UpdateAction::ISTATUS, _pocketView->getCurrentEntry()->status);
                    _pocketView->reDrawCurrentEntry();
                    HideHourglass();
                    break;

                }
                //Unstar/Star
            case 103:
                {
                    if(_pocketView->getCurrentEntry()->starred)
                    {
                        _pocket->sendItem(PocketAction::UNFAVORITE, _pocketView->getCurrentEntry()->id);
                    }
                    else
                    {
                        _pocket->sendItem(PocketAction::FAVORITE, _pocketView->getCurrentEntry()->id);
                    }
                    _pocketView->getCurrentEntry()->starred = !_pocketView->getCurrentEntry()->starred;
                    _sqliteCon.updatePocketItem(_pocketView->getCurrentEntry()->id, UpdateAction::ISTARRED, _pocketView->getCurrentEntry()->starred);
                    _pocketView->reDrawCurrentEntry();
                    HideHourglass();
                    break;
                }
            default:
                {
                    _pocketView->invertCurrentEntryColor();
                    break;
                }
        }
    }
    catch (const std::exception &e)
    {

        _pocketView->invertCurrentEntryColor();
        Log::writeErrorLog(e.what());
        Message(ICON_INFORMATION, "Error",e.what(), 1000);
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

            _contextMenu.createMenu(par2, EventHandler::contextMenuHandlerStatic,*_pocketView->getCurrentEntry());
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


                int dialogResult = DialogSynchro(ICON_INFORMATION, "Action",(_pocketView->getCurrentEntry()->title + "\n" + _pocketView->getCurrentEntry()->excerpt + "...").c_str(), "Read article", "Cancel", NULL);
                switch (dialogResult)
                {
                    case 1:
                        {
                            try{
                                if(_pocketView->getCurrentEntry()->downloaded == PIsDownloaded::PNOTDOWNLOADED)
                                {
                                    _pocket->getText(_pocketView->getCurrentEntry());
                                    _sqliteCon.updatePocketItem(_pocketView->getCurrentEntry()->id, UpdateAction::IDOWNLOADED, PIsDownloaded::PDOWNLOADED);
                                    _pocketView->getCurrentEntry()->downloaded = PIsDownloaded::PDOWNLOADED;
                                    _pocketView->reDrawCurrentEntry();
                                }
                                else
                                    _pocketView->invertCurrentEntryColor();

                                OpenBook(_pocketView->getCurrentEntry()->path.c_str(),"",0);
                                break;
                            }
                            catch (const std::exception &e)
                            {

                                Log::writeErrorLog(e.what());
                                Message(ICON_INFORMATION, "Error",e.what(), 1000);
                            }
                        }
                    default:
                        {
                            _pocketView->invertCurrentEntryColor();
                            break;
                        }
                }

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

void EventHandler::filterAndDrawPocket(IStatus status, bool favorited)
{
    try{
        _items.clear();
        vector<PocketItem> newEntries = _pocket->getItems();
        _sqliteCon.insertPocketEntries(newEntries);
        vector<PocketItem> oldEntries = _sqliteCon.selectPocketEntries();
        for(size_t i = 0; i < newEntries.size(); i++)
        {
            for(size_t j = 0; j < oldEntries.size();j++)
            {
                if(newEntries.at(i).id == oldEntries.at(j).id)
                {
                    //TODO filter in DB favorited and status are there, both needed?
                    if(newEntries.at(i).starred != oldEntries.at(j).starred)
                        _sqliteCon.updatePocketItem(oldEntries.at(j).id, UpdateAction::ISTARRED, newEntries.at(i).starred);
                    if(newEntries.at(i).status != oldEntries.at(j).status)
                        _sqliteCon.updatePocketItem(oldEntries.at(j).id, UpdateAction::ISTATUS, newEntries.at(i).status);
                    break;
                }
            }
        }

        oldEntries = _sqliteCon.selectPocketEntries();
        for(size_t j = 0; j < oldEntries.size();j++)
        {
            if(favorited){
                if(oldEntries.at(j).starred == favorited)
                    _items.push_back(oldEntries.at(j));
            }else{
                if(oldEntries.at(j).status == status)
                    _items.push_back(oldEntries.at(j));
            }
        }

        drawPocketItems(_items);
    }
    catch (const std::exception &e)
    {

        Log::writeErrorLog(e.what());
        Message(ICON_INFORMATION, "Error",e.what(), 1000);
    }
}
