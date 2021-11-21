//------------------------------------------------------------------
// sqliteConnector.cpp
//
// Author:           JuanJakobo
// Date:             18.07.2021
// Description:
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

vector<PocketItem> SqliteConnector::selectPocketEntries()
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;
    vector<PocketItem> entries;

    rs = sqlite3_prepare_v2(_db, "SELECT id, status, title, url, comments_url, content, starred, reading_time, downloaded FROM 'PocketItems';", -1, &stmt, 0);

    auto test = sqlite3_column_count(stmt);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
						PocketItem temp;

						temp.id = sqlite3_column_int(stmt,0);
						//temp.status = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
						temp.title = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
						temp.url =reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
						//temp.comments_url = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
						temp.excerpt =  reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
						temp.starred = (sqlite3_column_int(stmt, 6) == 1) ? true : false;
						temp.reading_time = sqlite3_column_int(stmt, 7);
						temp.downloaded =  static_cast<IsDownloaded>(sqlite3_column_int(stmt,8));
						entries.push_back(temp);

    }

    sqlite3_finalize(stmt);
    sqlite3_close(_db);
    return entries;
}

vector<PocketItem> SqliteConnector::selectPocketEntries(IsDownloaded downloaded)
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;
    vector<PocketItem> entries;

    rs = sqlite3_prepare_v2(_db, "SELECT id, status, title, url, comments_url, content, starred, reading_time, downloaded FROM 'PocketItems' WHERE downloaded = ?;", -1, &stmt, 0);

    rs = sqlite3_bind_int(stmt, 1, downloaded);

    auto test = sqlite3_column_count(stmt);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
						PocketItem temp;

						temp.id = sqlite3_column_int(stmt,0);
						//temp.status = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
						temp.title = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
						temp.url =reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
						//temp.comments_url = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
						temp.excerpt =  reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
						temp.starred = (sqlite3_column_int(stmt, 6) == 1) ? true : false;
						temp.reading_time = sqlite3_column_int(stmt, 7);
						temp.downloaded =  static_cast<IsDownloaded>(sqlite3_column_int(stmt,8));
						entries.push_back(temp);

    }

    sqlite3_finalize(stmt);
    sqlite3_close(_db);
    return entries;
}



bool SqliteConnector::updateDownloadStatusPocketItem(int entryID, IsDownloaded downloaded)
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;

    rs = sqlite3_prepare_v2(_db, "UPDATE 'PocketItems' SET downloaded=? WHERE id=?", -1, &stmt, 0);
    rs = sqlite3_bind_int(stmt, 1, downloaded);
    rs = sqlite3_bind_int(stmt, 2, entryID);
    rs = sqlite3_step(stmt);

    if (rs != SQLITE_DONE)
    {
        Log::writeErrorLog(sqlite3_errmsg(_db) + std::string(" (Error Code: ") + std::to_string(rs) + ")");
    }
    rs = sqlite3_clear_bindings(stmt);
    rs = sqlite3_reset(stmt);

    sqlite3_finalize(stmt);
    sqlite3_close(_db);

    return true;
}

bool SqliteConnector::updatePocketItem(int entryID, bool starred)
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;

    rs = sqlite3_prepare_v2(_db, "UPDATE 'PocketItems' SET starred=? WHERE id=?", -1, &stmt, 0);
    rs = sqlite3_bind_int(stmt, 1, (starred) ? 1 : 0);
    rs = sqlite3_bind_int(stmt, 2, entryID);
    rs = sqlite3_step(stmt);

    if (rs != SQLITE_DONE)
    {
        Log::writeErrorLog(sqlite3_errmsg(_db) + std::string(" (Error Code: ") + std::to_string(rs) + ")");
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

    rs = sqlite3_prepare_v2(_db, "INSERT INTO 'PocketItems' (id, status, title, url, comments_url, content, starred, reading_time, downloaded) VALUES (?,?,?,?,?,?,?,?,?);", -1, &stmt, 0);
    rs = sqlite3_exec(_db, "BEGIN TRANSACTION;", NULL, NULL, NULL);

    for (auto ent : entries)
    {
        rs = sqlite3_bind_int(stmt, 1, ent.id);
        //rs = sqlite3_bind_text(stmt, 2, ent.status.c_str(), ent.status.length(), NULL);
        rs = sqlite3_bind_text(stmt, 3, ent.title.c_str(), ent.title.length(), NULL);
        rs = sqlite3_bind_text(stmt, 4, ent.url.c_str(), ent.url.length(), NULL);
        //rs = sqlite3_bind_text(stmt, 5, ent.comments_url.c_str(), ent.comments_url.length(), NULL);
        rs = sqlite3_bind_text(stmt, 6, ent.excerpt.c_str(), ent.excerpt.length(), NULL);
        rs = sqlite3_bind_int(stmt, 7, (ent.starred) ? 1 : 0);
        rs = sqlite3_bind_int(stmt, 8, ent.reading_time);
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
    rs = sqlite3_exec(_db, "CREATE TABLE IF NOT EXISTS PocketItems (id INT PRIMARY KEY, status TEXT, title TEXT, url TEXT, comments_url TEXT, content TEXT, starred INT, reading_time INT, downloaded INT);", NULL, 0, NULL);

    return true;
}
