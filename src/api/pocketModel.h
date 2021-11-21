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

enum IsDownloaded
{
				NOTDOWNLOADED,
				TOBEDOWNLOADED,
				DOWNLOADED
};

struct PocketItem : Entry
{
    std::string title;
    std::string status;
    std::string url;
    std::string excerpt;
    int reading_time; //in min
    bool starred;
    IsDownloaded downloaded = IsDownloaded::NOTDOWNLOADED;
};

#endif
