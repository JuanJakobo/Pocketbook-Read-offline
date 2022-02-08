//------------------------------------------------------------------
// contextMenu.cpp
//
// Author:           JuanJakobo
// Date:             14.06.2020
//
//-------------------------------------------------------------------

#include "contextMenu.h"

#include <string>

ContextMenu::ContextMenu()
{
}

ContextMenu::~ContextMenu()
{
    free(_menu);
    free(_star);
    free(_download);
}

int ContextMenu::createMenu(int y, const iv_menuhandler &handler, const PocketItem &item)
{
    std::string text = "Remove item";
    if(item.downloaded == IsDownloaded::NOTDOWNLOADED)
        text = "Download item";
    _download = strdup(text.c_str());

    text = "Mark unread";
    if(item.status == IStatus::UNREAD)
        text = "Archive";
    _archive = strdup(text.c_str());

    text = "Star";
    if(item.starred)
        text = "Unstar";
    _star = strdup(text.c_str());

    imenu contextMenu[] =
        {
            {ITEM_HEADER, 0, _menu, NULL},
            {ITEM_ACTIVE, 101, _download, NULL},
            {ITEM_ACTIVE, 102, _archive, NULL},
            {ITEM_ACTIVE, 103, _star, NULL},
            {0, 0, NULL, NULL}};

    OpenMenu(contextMenu, 0, ScreenWidth(), y, handler);

    return 1;
}
