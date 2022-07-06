//------------------------------------------------------------------
// sqliteConnector.cpp
//
// Author:           JuanJakobo
// Date:             18.07.2021
//
//-------------------------------------------------------------------

#include "sqlite3.h"
#include "pocketModel.h"
#include "sqliteConnector.h"
#include "log.h"

#include <string>
#include <vector>

using std::vector;
using std::string;

SqliteConnector::SqliteConnector(const string &DBpath) : _dbpath(DBpath)
{
}

vector<PocketItem> SqliteConnector::selectPocketEntries(PIsDownloaded downloaded)
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;
    vector<PocketItem> entries;

    if(downloaded == PIsDownloaded::PINVALID)
    {
        rs = sqlite3_prepare_v2(_db, "SELECT id, status, title, url, excerpt, path, reading_time, starred, downloaded FROM 'PocketItems';", -1, &stmt, 0);
    }
    else
    {
        rs = sqlite3_prepare_v2(_db, "SELECT id, status, title, url, excerpt, path, reading_time, starred, downloaded FROM 'PocketItems' WHERE downloaded = ?;", -1, &stmt, 0);
        rs = sqlite3_bind_int(stmt, 1, downloaded);
    }

    auto test = sqlite3_column_count(stmt);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
						PocketItem temp;

						temp.id = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
						temp.status =  static_cast<IStatus>(sqlite3_column_int(stmt,1));
						temp.title = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
						temp.url =reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
						temp.excerpt =  reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
						temp.path =  reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
						temp.reading_time = sqlite3_column_int(stmt, 6);
						temp.starred = (sqlite3_column_int(stmt, 7) == 1) ? true : false;
						temp.downloaded =  static_cast<PIsDownloaded>(sqlite3_column_int(stmt,8));
						entries.push_back(temp);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(_db);
    return entries;
}

bool SqliteConnector::updatePocketItem(const string &entryID, int toUpdate, int value)
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;

    switch (toUpdate)
    {
        case UpdateAction::IDOWNLOADED:
            {
                rs = sqlite3_prepare_v2(_db, "UPDATE 'PocketItems' SET downloaded=? WHERE id=?", -1, &stmt, 0);
                rs = sqlite3_bind_int(stmt, 1, value);
                break;
            }
        case UpdateAction::ISTATUS:
            {
                rs = sqlite3_prepare_v2(_db, "UPDATE 'PocketItems' SET status=? WHERE id=?", -1, &stmt, 0);
                rs = sqlite3_bind_int(stmt, 1, value);
                break;
            }
        case UpdateAction::ISTARRED:
            {
                rs = sqlite3_prepare_v2(_db, "UPDATE 'PocketItems' SET starred=? WHERE id=?", -1, &stmt, 0);
                rs = sqlite3_bind_int(stmt, 1, value);
                break;
            }
    }

    rs = sqlite3_bind_text(stmt,2, entryID.c_str(), entryID.length(),NULL);
    rs = sqlite3_step(stmt);

    if (rs != SQLITE_DONE)
    {
        Log::writeErrorLog(sqlite3_errmsg(_db) + std::string(" (Error Code: ") + std::to_string(rs) + ")");
        return false;
    }
    rs = sqlite3_clear_bindings(stmt);
    rs = sqlite3_reset(stmt);

    sqlite3_finalize(stmt);
    sqlite3_close(_db);

    return true;
}

bool SqliteConnector::insertPocketEntries(const std::vector<PocketItem> &entries)
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;

    rs = sqlite3_prepare_v2(_db, "INSERT INTO 'PocketItems' (id, status, title, url, excerpt, path, reading_time, starred, downloaded) VALUES (?,?,?,?,?,?,?,?,?);", -1, &stmt, 0);
    rs = sqlite3_exec(_db, "BEGIN TRANSACTION;", NULL, NULL, NULL);

    for (auto ent : entries)
    {
        rs = sqlite3_bind_text(stmt, 1, ent.id.c_str(), ent.id.length(),NULL);
        rs = sqlite3_bind_int(stmt, 2, ent.status);
        rs = sqlite3_bind_text(stmt, 3, ent.title.c_str(), ent.title.length(), NULL);
        rs = sqlite3_bind_text(stmt, 4, ent.url.c_str(), ent.url.length(), NULL);
        rs = sqlite3_bind_text(stmt, 5, ent.excerpt.c_str(), ent.excerpt.length(), NULL);
        rs = sqlite3_bind_text(stmt, 6, ent.path.c_str(), ent.path.length(), NULL);
        rs = sqlite3_bind_int(stmt, 7, ent.reading_time);
        rs = sqlite3_bind_int(stmt, 8, (ent.starred) ? 1 : 0);
        rs = sqlite3_bind_int(stmt, 9, ent.downloaded);

        rs = sqlite3_step(stmt);
        if (rs == SQLITE_CONSTRAINT)
        {
            //TODO what if item is already there? update? --> use replace?
        }
        else if (rs != SQLITE_DONE)
        {
            Log::writeErrorLog(sqlite3_errmsg(_db) + std::string(" (Error Code: ") + std::to_string(rs) + ")");
        }
        rs = sqlite3_clear_bindings(stmt);
        rs = sqlite3_reset(stmt);
    }

    sqlite3_exec(_db, "END TRANSACTION;", NULL, NULL, NULL);

    sqlite3_finalize(stmt);
    sqlite3_close(_db);

    return true;
}


bool SqliteConnector::open()
{
    int rs;

    rs = sqlite3_open(_dbpath.c_str(), &_db);


    if (rs)
    {
        Log::writeErrorLog("Could not open DB at " + _dbpath);
    }
    rs = sqlite3_exec(_db, "CREATE TABLE IF NOT EXISTS PocketItems (id TEXT PRIMARY KEY, status INT, title TEXT, url TEXT, excerpt TEXT, path TEXT, reading_time INT, starred INT, downloaded INT);", NULL, 0, NULL);
    return true;
}
