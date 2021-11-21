//------------------------------------------------------------------
// pocketViewEntry.h
//
// Author:           JuanJakobo
// Date:             04.08.2020
// Description:      An pocketViewEntry that handles an item of a listview
//-------------------------------------------------------------------

#ifndef POCKETVIEWENTRY
#define POCKETVIEWENTRY

#include "listViewEntry.h"
#include "pocketModel.h"

class PocketViewEntry : public ListViewEntry
{
public:
    /**
        * Creates an pocketViewEntry 
        * 
        * @param Page site of the listView the Entry is shown
        * @param Rect area of the screen the item is positioned
        * @param entry entry that shall be drawn
        */
    PocketViewEntry(int page, const irect &position, const PocketItem &entry);

    /**
        * draws the pocketViewEntry to the screen
        * 
        * @param entryFont font for the entry itself
        * @param entryFontBold bold font for the header
        * @param fontHeight height of the font 
        */
    void draw(const ifont *entryFont, const ifont *entryFontBold, int fontHeight) override;

    PocketItem *get() override { return &_entry; };

private:
    PocketItem _entry;
};
#endif
