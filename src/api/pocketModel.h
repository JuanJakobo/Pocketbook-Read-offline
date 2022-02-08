//------------------------------------------------------------------
// pocketModel.h
//
// Author:           JuanJakobo
// Date:             23.04.2021
// Description:      
//-------------------------------------------------------------------

#ifndef POCKETMODEL
#define POCKETMODEL

#include "model.h"

#include <string>

enum IStatus
{
    UNREAD,
    ARCHIVED,
    TODELETE
};

enum IsDownloaded
{
				NOTDOWNLOADED,
				TOBEDOWNLOADED,
				DOWNLOADED
};

struct PocketItem : Entry
{
    std::string title;
    IStatus  status;
    std::string url;
    std::string excerpt;
    std::string path;
    int reading_time; //in min
    bool starred;
    IsDownloaded downloaded = IsDownloaded::NOTDOWNLOADED;
};

#endif
