//------------------------------------------------------------------
// sqliteConnector.h
//
// Author:           JuanJakobo
// Date:             18.07.2021
// Description: Interface to store itms in a sqlite db
//
//-------------------------------------------------------------------

#ifndef SQLITECONNECTOR
#define SQLITECONNECTOR

#include "sqlite3.h"
#include "pocketModel.h"

#include <string>
#include <vector>

enum UpdateAction
{
    IDOWNLOADED,
    ISTATUS,
    ISTARRED
};

class SqliteConnector
{
public:
    /**
     * Creates a new database object
     *
     * @param DBpath the path where the database is placed
     */
    SqliteConnector(const std::string &DBpath);

    /**
     * Selectes the pocket entries
     *
     * @return vector of PocketItems that has been received
     *
     * @param IsDownloaded the downloaded status of the pocket items
     */
    std::vector<PocketItem> selectPocketEntries(IsDownloaded download = IsDownloaded::INVALID);

    /**
     * Update pocket items in DB
     *
     * @param the entryID that shall be updates
     * @param toUpdate that shall be updated
     * @param value the value that shall be written
     *
     * @return true if updates was succesfull
     */
    bool updatePocketItem(const std::string &entryID, int toUpdate, int value);

    /**
     * Insert new pocketEntries to the DB
     *
     * @param entries that shall be inserted
     *
     * @return true if updates was succesfull
     */
    bool insertPocketEntries(const std::vector<PocketItem> &entries);

private:
    std::string _dbpath;
    sqlite3 *_db;

    /**
     * Opens the DB and creates the table if necessary
     *
     * @return true if was sucessfull
     */
    bool open();
};

#endif
