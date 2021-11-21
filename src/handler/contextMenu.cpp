//------------------------------------------------------------------
// contextMenu.cpp
//
// Author:           JuanJakobo
// Date:             14.06.2020
//
//-------------------------------------------------------------------

#include "inkview.h"
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
    free(_browser);
}

int ContextMenu::createMenu(int y, const iv_menuhandler &handler, bool starred, const std::string &downloaded)
{
    std::string text = "Star";
    if(starred)
        text = "Unstar";

    _star = strdup(text.c_str());
	
		_download = strdup(downloaded.c_str());

    imenu contextMenu[] =
        {
            {ITEM_HEADER, 0, _menu, NULL},
            {ITEM_ACTIVE, 102, _download, NULL},
            {ITEM_ACTIVE, 103, _star, NULL},
            {ITEM_ACTIVE, 104, _browser, NULL},

            {0, 0, NULL, NULL}};

    OpenMenu(contextMenu, 0, ScreenWidth(), y, handler);

    return 1;
}
