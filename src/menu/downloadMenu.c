/***************************************************************************
 * This file is part of NUSspli.                                           *
 * Copyright (c) 2019-2020 Pokes303                                        *
 * Copyright (c) 2020 V10lator <v10lator@myway.de>                         *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the Free Software Foundation, Inc., *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.             *
 ***************************************************************************/

#include <wut-fixups.h>

#include <config.h>
#include <downloader.h>
#include <input.h>
#include <ioThread.h>
#include <renderer.h>
#include <status.h>
#include <usb.h>
#include <utils.h>
#include <cJSON.h>
#include <menu/download.h>
#include <menu/utils.h>

#include <stdio.h>
#include <string.h>

#include <coreinit/memdefaultheap.h>
#include <vpad/input.h>

bool vibrateWhenFinished = true;

void drawDownloadFrame1()
{
	startNewFrame();
	textToFrame(0, 0, "Input a title ID to download its content [Ex: 000500001234abcd]");
	lineToFrame(MAX_LINES - 3, SCREEN_COLOR_WHITE);
	textToFrame(MAX_LINES - 2, 0, "Press \uE000 to show the keyboard [Only input hexadecimal numbers]");
	textToFrame(MAX_LINES - 1, 0, "Press \uE001 to return");
	drawFrame();
}

void drawDownloadFrame2(const char *titleID, const char *titleVer, const char *folderName, bool usbMounted, bool dlToUSB, bool keepFiles)
{
	startNewFrame();
	textToFrame(0, 0, "Provided title ID [Only 16 digit hexadecimal]:");
	textToFrame(1, 3, titleID);
	
	textToFrame(2, 0, "Provided title version [Only numbers]:");
	textToFrame(3, 3, titleVer[0] == '\0' ? "<LATEST>" : titleVer);
	
	textToFrame(4, 0, "Custom folder name [Only text and numbers]:");
	textToFrame(5, 3, folderName);
	
	char toFrame[128];
	strcpy(toFrame, "Press \uE045 to ");
	strcat(toFrame, vibrateWhenFinished ? "deactivate" : "activate");
	strcat(toFrame, " the vibration after installing");
	textToFrame(MAX_LINES - 1, 0, toFrame); //Thinking to change this to activate HOME Button led
	
	int line = MAX_LINES - 2;
	if(usbMounted)
	{
		strcpy(toFrame, "Press \uE046 to download to ");
		strcat(toFrame, dlToUSB ? "SD" : "USB");
		textToFrame(line--, 0, toFrame);
	}
	if(dlToUSB)
		textToFrame(line--, 0, "WARNING: Files on USB will always be deleted after installing!");
	else
	{
		strcpy(toFrame, "Press \uE07B to ");
		strcat(toFrame, keepFiles ? "delete" : "keep");
		strcat(toFrame, " downloaded files after the installation");
		textToFrame(line--, 0, toFrame);
	}
	
	lineToFrame(line--, SCREEN_COLOR_WHITE);
	
	textToFrame(line--, 0, "Press \uE07A to set a custom name to the download folder");
	textToFrame(line--, 0, "Press \uE07C to set the title version");
	textToFrame(line--, 0, "Press \uE079 to set the title ID");
	lineToFrame(line--, SCREEN_COLOR_WHITE);
	
	textToFrame(line--, 0, "Press \uE001 to return");
	
	strcpy(toFrame, "Press \uE003 to download to ");
	strcat(toFrame, dlToUSB ? "USB" : "SD");
	strcat(toFrame, " only");
	textToFrame(line--, 0, toFrame);
	
	textToFrame(line--, 0, "Press \uE002 to install to NAND");
	textToFrame(line--, 0, "Press \uE000 to install to USB");
	lineToFrame(line--, SCREEN_COLOR_WHITE);
	
	drawFrame();
}

void downloadMenu()
{
	char titleID[17];
	char titleVer[33];
	char folderName[FILENAME_MAX - 11];
	titleID[0] = titleVer[0] = folderName[0] = '\0';
	
	drawDownloadFrame1();
	
	while(AppRunning())
	{
		if(app == APP_STATE_BACKGROUND)
			continue;
		if(app == APP_STATE_RETURNING)
			drawDownloadFrame1();
		
		showFrame();
		
		if(vpad.trigger & VPAD_BUTTON_A)
		{
			if(showKeyboard(KEYBOARD_TYPE_RESTRICTED, titleID, CHECK_HEXADECIMAL, 16, true, "00050000101", NULL))
				 break;
			 drawDownloadFrame1();
		}
		else if(vpad.trigger & VPAD_BUTTON_B)
			return;
	}
	if(!AppRunning())
		return;
	
	toLowercase(titleID);
	
	bool usbMounted = mountUSB();
	bool dlToUSB = false;
	bool keepFiles = true;
	drawDownloadFrame2(titleID, titleVer, folderName, usbMounted, dlToUSB, keepFiles);
	
	bool loop = true;
	bool inst, toUSB;
	bool redraw = false;
	while(loop && AppRunning())
	{
		if(app == APP_STATE_BACKGROUND)
			continue;
		if(app == APP_STATE_RETURNING)
			drawDownloadFrame2(titleID, titleVer, folderName, usbMounted, dlToUSB, keepFiles);
		
		showFrame();
		
		if(vpad.trigger & VPAD_BUTTON_B)
			return;
		
		if(vpad.trigger & VPAD_BUTTON_A)
		{
			inst = toUSB = true;
			loop = false;
		}
		else if(vpad.trigger & VPAD_BUTTON_Y)
			inst = toUSB = loop = false;
		else if(vpad.trigger & VPAD_BUTTON_X)
		{
			inst = true;
			toUSB = loop = false;
		}
		
		if(vpad.trigger & VPAD_BUTTON_UP)
		{
			char tmpTitleID[17];
			if(showKeyboard(KEYBOARD_TYPE_RESTRICTED, tmpTitleID, CHECK_HEXADECIMAL, 16, true, titleID, NULL))
			{
				toLowercase(tmpTitleID);
				strcpy(titleID, tmpTitleID);
				redraw = true;
			}
		}
		else if(vpad.trigger & VPAD_BUTTON_RIGHT)
		{
			if(!showKeyboard(KEYBOARD_TYPE_RESTRICTED, titleVer, CHECK_NUMERICAL, 5, false, titleVer, NULL))
				titleVer[0] = '\0';
			redraw = true;
		}
		else if(vpad.trigger & VPAD_BUTTON_DOWN)
		{
			if(!showKeyboard(KEYBOARD_TYPE_NORMAL, folderName, CHECK_NOSPECIAL, FILENAME_MAX - 11, false, folderName, NULL))
				folderName[0] = '\0';
			redraw = true;
		}
		
		if(vpad.trigger & VPAD_BUTTON_MINUS)
		{
			if(usbMounted)
			{
				dlToUSB = !dlToUSB;
				keepFiles = !dlToUSB;
				redraw = true;
			}
		}
		if(vpad.trigger & VPAD_BUTTON_PLUS)
		{
			vibrateWhenFinished = !vibrateWhenFinished;
			redraw = true;
		}
		if(vpad.trigger & VPAD_BUTTON_LEFT)
		{
			if(!dlToUSB)
			{
				keepFiles = !keepFiles;
				redraw = true;
			}
		}
		
		if(redraw)
		{
			drawDownloadFrame2(titleID, titleVer, folderName, usbMounted, dlToUSB, keepFiles);
			redraw = false;
		}
	}
	
	if(!AppRunning())
		return;
	
	downloadTitle(titleID, titleVer, folderName, inst, dlToUSB, toUSB, keepFiles);
}
