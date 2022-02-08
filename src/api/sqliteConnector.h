//------------------------------------------------------------------
// sqliteConnector.h
//
// Author:           JuanJakobo
// Date:             18.07.2021
// Description:
//
//-------------------------------------------------------------------

#ifndef SQLITECONNECTOR
#define SQLITECONNECTOR

#include "sqlite3.h"
#include "pocketModel.h"

#include <string>
#include <vector>

class SqliteConnector
{
public:
    /**
    *  
    */
    SqliteConnector(const std::string &DBpath);

    std::vector<PocketItem> selectPocketEntries();

    std::vector<PocketItem> selectPocketEntries(IsDownloaded downloaded);

    bool updatePocketItem(const std::string &entryID, bool starred);

    bool updateStatusPocketItem(const std::string &entryID, IStatus status);

    bool updateDownloadStatusPocketItem(const std::string &entryID, IsDownloaded downloaded);

    bool insertPocketEntries(const std::vector<PocketItem> &entries);

private:
    std::string _dbpath;
    sqlite3 *_db;

    bool open();
};

#endif
