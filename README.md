# Pocketbook Read offline
A basic client to access an pocket instance via a Pocketbook ebook-reader. 

<img src="/screenshots/ListView.bmp" width="15%" height="15%">&nbsp;&nbsp;<img src="/screenshots/ContentDialog.bmp" width="15%" height="15%">&nbsp;&nbsp;<img src="/screenshots/ItemMenu.bmp" width="15%" height="15%">&nbsp;&nbsp;<img src="/screenshots/MainMenu.bmp" width="15%" height="15%">

## Features

* Show Pocket articles the user has stored
* Download articles to read them later offline TODO
* Show unread and starred items TODO
* Open content in Pocketbook reader TODO
* Handle everything by touch or keys

## Installation
Download and unzip the file from releases and place the Readoffline.app into the "applications" folder of your pocketbook. 
Once you disconnect the Pocketbook from the PC, the application should be visibile in the application launcher.

## How to build

First you need to install the basic build tools for linux.

Then you have to download the Pocketbook SDK (https://github.com/pocketbook/SDK_6.3.0/tree/5.19).

In the CMakeLists.txt of this project you have to set the root of the TOOLCHAIN_PATH to the location where you saved the SDK. 
This could be for example:

`SET (TOOLCHAIN_PATH "../../SDK/SDK_6.3.0/SDK-B288")`

Then you have to setup cmake by:

`cmake .`

To build the application run:

`make` 

In the file pocket.h an Consumer_Key has to be added.

## Disclamer
Use as your own risk! 
Even though the possibility is really low, the application could harm your device or even break it.
