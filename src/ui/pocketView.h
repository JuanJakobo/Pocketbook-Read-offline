//------------------------------------------------------------------
// listView.h
//
// Author:           JuanJakobo
// Date:             04.08.2020
// Description:      An UI class to display items in a listview
//-------------------------------------------------------------------

#ifndef POCKETVIEW
#define POCKETVIEW

#include "pocketModel.h"
#include "listView.h"
#include "pocketViewEntry.h"

#include <vector>
#include <memory>

class PocketView final : public ListView
{
public:
    /**
     * Displays a list view
     *
     * @param ContentRect area of the screen where the list view is placed
     * @param Items items that shall be shown in the listview
     * @param page page that is shown, default is 1
     */
    PocketView(const irect *contentRect, const std::vector<PocketItem> &pocketItems, int page = 1);

    /**
     * Gets the current page and returns the itemIds till the page
     *
     * @return a vector containing the itemIds
     */
    std::vector<std::string> getEntriesTillPage();

    PocketItem *getCurrentEntry() { return getEntry(_selectedEntry); };

    PocketItem *getEntry(int entryID) { return std::dynamic_pointer_cast<PocketViewEntry>(_entries.at(entryID))->get(); };
};
#endif
