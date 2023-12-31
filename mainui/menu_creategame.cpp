/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "extdll.h"
#include "basemenu.h"
#include "utils.h"
#include "keydefs.h"
#include "menu_btnsbmp_table.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <functional>

#define ART_BANNER		"gfx/shell/head_creategame"

#define ID_BACKGROUND	0
#define ID_BANNER		1
#define ID_ADVOPTIONS	2
#define ID_DONE		3
#define ID_CANCEL		4
#define ID_MAPLIST		5
#define ID_TABLEHINT	6
#define ID_MAXCLIENTS	7
#define ID_HOSTNAME		8
#define ID_PASSWORD		9
#define ID_NAT		10
#define ID_DEDICATED	11

#define ID_MSGBOX	 	12
#define ID_MSGTEXT	 	13
#define ID_BOTNUM		14
#define ID_GAMEMODE		15
#define ID_YES	 	130
#define ID_NO	 	131

#define MAPNAME_LENGTH	20
#define TITLE_LENGTH	20+MAPNAME_LENGTH

typedef struct
{
	char		mapName[UI_MAXGAMES][64];
	char		mapsDescription[UI_MAXGAMES][256];
	char		*mapsDescriptionPtr[UI_MAXGAMES];

	menuFramework_s	menu;

	menuBitmap_s	background;
	menuBitmap_s	banner;
	menuPicButton_s	advOptions;
	menuPicButton_s	done;
	menuPicButton_s	cancel;

	menuField_s	maxClients;
	menuField_s	hostName;
	menuField_s	password;
	menuField_s	botNum;
	menuSpinControl_s gamemode;
	menuCheckBox_s	nat;
	menuCheckBox_s	dedicatedServer;

	// newgame prompt dialog
	menuAction_s	msgBox;
	menuAction_s	dlgMessage1;
	menuAction_s	dlgMessage2;
	menuPicButton_s	yes;
	menuPicButton_s	no;

	menuScrollList_s	mapsList;
	menuAction_s	hintMessage;
	char		hintText[MAX_HINT_TEXT];
} uiCreateGame_t;

static uiCreateGame_t	uiCreateGame;

#define MAX_GAMEMODES 9
static const char *g_szGameModeNames[MAX_GAMEMODES] = 
{
	"Classic", "Death Match", "Gun Dead Match", "Team Death Match", "Zombie Classic", "Zombie Mutation", "Zombie Hero", "Scenario Zombie", "Zombie Shelter"
};

static const char *g_szGameModeCodes[MAX_GAMEMODES] = 
{
	"none", "dm", "gd", "tdm", "zb1", "zb2", "zb3", "zbs", "zsh_pve"
};

/*
=================
UI_CreateGame_Begin
=================
*/
static void UI_CreateGame_Begin(void)
{
	if (!MAP_IS_VALID(uiCreateGame.mapName[uiCreateGame.mapsList.curItem]))
		return;	// bad map

	if (CVAR_GET_FLOAT("host_serverstate"))
	{
		if (CVAR_GET_FLOAT("maxplayers") == 1)
			HOST_ENDGAME("end of the game");
		else
			HOST_ENDGAME("starting new server");
	}

	CVAR_SET_FLOAT("deathmatch", 1.0f);	// start deathmatch as default
	CVAR_SET_FLOAT("maxplayers", atoi(uiCreateGame.maxClients.buffer));
	CVAR_SET_STRING("hostname", uiCreateGame.hostName.buffer);
	CVAR_SET_STRING("defaultmap", uiCreateGame.mapName[uiCreateGame.mapsList.curItem]);
	CVAR_SET_FLOAT("sv_nat", CVAR_GET_FLOAT("public") ? uiCreateGame.nat.enabled : 0);
	CVAR_SET_FLOAT("bot_quota", atoi(uiCreateGame.botNum.buffer));
	CVAR_SET_STRING("mp_gamemode", g_szGameModeCodes[ static_cast<size_t>(uiCreateGame.gamemode.curValue)]);

	BACKGROUND_TRACK(NULL, NULL);

	// all done, start server
	
		HOST_WRITECONFIG(CVAR_GET_STRING("lservercfgfile"));

		char cmd[128];
		sprintf(cmd, "exec %s\n", CVAR_GET_STRING("lservercfgfile"));

		CLIENT_COMMAND(TRUE, cmd);

		// dirty listenserver config form old xash may rewrite maxplayers
		CVAR_SET_FLOAT("maxplayers", atoi(uiCreateGame.maxClients.buffer));

		// hack: wait three frames allowing server to completely shutdown, reapply maxplayers and start new map
		sprintf(cmd, "host_endgame;wait;wait;wait;maxplayers %i;latch;map %s\n",
			atoi(uiCreateGame.maxClients.buffer),
			uiCreateGame.mapName[uiCreateGame.mapsList.curItem]
		);
		CLIENT_COMMAND(FALSE, cmd);

}

static void UI_PromptDialog(void)
{
	if (!CVAR_GET_FLOAT("host_serverstate") || CVAR_GET_FLOAT("cl_background"))
	{
		UI_CreateGame_Begin();
		return;
	}

	// toggle main menu between active\inactive
	// show\hide quit dialog
	uiCreateGame.advOptions.generic.flags ^= QMF_INACTIVE;
	uiCreateGame.done.generic.flags ^= QMF_INACTIVE;
	uiCreateGame.cancel.generic.flags ^= QMF_INACTIVE;
	uiCreateGame.gamemode.generic.flags ^= QMF_INACTIVE;
	uiCreateGame.maxClients.generic.flags ^= QMF_INACTIVE;
	uiCreateGame.botNum.generic.flags ^= QMF_INACTIVE;
	uiCreateGame.hostName.generic.flags ^= QMF_INACTIVE;
	uiCreateGame.password.generic.flags ^= QMF_INACTIVE;
	uiCreateGame.nat.generic.flags ^= QMF_INACTIVE;
	uiCreateGame.mapsList.generic.flags ^= QMF_INACTIVE;

	uiCreateGame.msgBox.generic.flags ^= QMF_HIDDEN;
	uiCreateGame.dlgMessage1.generic.flags ^= QMF_HIDDEN;
	uiCreateGame.dlgMessage2.generic.flags ^= QMF_HIDDEN;
	uiCreateGame.no.generic.flags ^= QMF_HIDDEN;
	uiCreateGame.yes.generic.flags ^= QMF_HIDDEN;

}

/*
=================
UI_CreateGame_KeyFunc
=================
*/
static const char *UI_CreateGame_KeyFunc(int key, int down)
{
	if (down && key == K_ESCAPE && !(uiCreateGame.dlgMessage1.generic.flags & QMF_HIDDEN))
	{
		UI_PromptDialog();
		return uiSoundNull;
	}
	return UI_DefaultKey(&uiCreateGame.menu, key, down);
}

/*
=================
UI_MsgBox_Ownerdraw
=================
*/
static void UI_MsgBox_Ownerdraw(void *self)
{
	menuCommon_s	*item = (menuCommon_s *)self;

	UI_FillRect(item->x, item->y, item->width, item->height, uiPromptBgColor);
}

/*
=================
UI_CreateGame_GetMapsList
=================
*/
static void UI_CreateGame_GetMapsList(void)
{
	char *afile;

	if (!CHECK_MAP_LIST(FALSE) || (afile = (char *)LOAD_FILE("maps.lst", NULL)) == NULL)
	{
		uiCreateGame.done.generic.flags |= QMF_GRAYED;
		uiCreateGame.mapsList.itemNames = (const char **)uiCreateGame.mapsDescriptionPtr;
		Con_Printf("Cmd_GetMapsList: can't open maps.lst\n");
		return;
	}

	char *pfile = afile;
	char token[1024];
	int numMaps = 0;

	while ((pfile = COM_ParseFile(pfile, token)) != NULL)
	{
		if (numMaps >= UI_MAXGAMES) break;
		StringConcat(uiCreateGame.mapName[numMaps], token, sizeof(uiCreateGame.mapName[0]));
		StringConcat(uiCreateGame.mapsDescription[numMaps], token, MAPNAME_LENGTH);
		AddSpaces(uiCreateGame.mapsDescription[numMaps], MAPNAME_LENGTH);
		if ((pfile = COM_ParseFile(pfile, token)) == NULL) break; // unexpected end of file
		StringConcat(uiCreateGame.mapsDescription[numMaps], token, TITLE_LENGTH);
		AddSpaces(uiCreateGame.mapsDescription[numMaps], TITLE_LENGTH);
		uiCreateGame.mapsDescriptionPtr[numMaps] = uiCreateGame.mapsDescription[numMaps];
		numMaps++;
	}

	if (!numMaps) uiCreateGame.done.generic.flags |= QMF_GRAYED;

	for (; numMaps < UI_MAXGAMES; numMaps++) uiCreateGame.mapsDescriptionPtr[numMaps] = NULL;
	uiCreateGame.mapsList.itemNames = (const char **)uiCreateGame.mapsDescriptionPtr;
	FREE_FILE(afile);
}

/*
=================
UI_CreateGame_Update
=================
*/
static void UI_CreateGame_Update()
{
	int i = 0;
	i = uiCreateGame.gamemode.curValue;
	uiCreateGame.gamemode.generic.name = g_szGameModeNames[i];
}

/*
=================
UI_CreateGame_Callback
=================
*/
static void UI_CreateGame_Callback(void *self, int event)
{
	menuCommon_s	*item = (menuCommon_s *)self;

	switch (item->id)
	{
	case ID_NAT:
	case ID_DEDICATED:
		if (event == QM_PRESSED)
			((menuCheckBox_s *)self)->focusPic = UI_CHECKBOX_PRESSED;
		else ((menuCheckBox_s *)self)->focusPic = UI_CHECKBOX_FOCUS;
		break;
	}

	if (event == QM_CHANGED)
	{
		UI_CreateGame_Update();
		return;
	}

	if (event != QM_ACTIVATED)
		return;

	switch (item->id)
	{
	case ID_ADVOPTIONS:
		// UNDONE: not implemented
		break;
	case ID_DONE:
		UI_PromptDialog();
		break;
	case ID_CANCEL:
		UI_PopMenu();
		break;
	case ID_YES:
		UI_CreateGame_Begin();
		break;
	case ID_NO:
		UI_PromptDialog();
		break;
	}
}

/*
=================
UI_CreateGame_Init
=================
*/
static void UI_CreateGame_Init(void)
{
	memset(&uiCreateGame, 0, sizeof(uiCreateGame_t));

	//uiCreateGame.menu.vidInitFunc = UI_CreateGame_Init;
	uiCreateGame.menu.keyFunc = UI_CreateGame_KeyFunc;

	StringConcat(uiCreateGame.hintText, "Map", MAPNAME_LENGTH);
	AddSpaces(uiCreateGame.hintText, MAPNAME_LENGTH);
	StringConcat(uiCreateGame.hintText, "Title", TITLE_LENGTH);
	AddSpaces(uiCreateGame.hintText, TITLE_LENGTH);

	uiCreateGame.background.generic.id = ID_BACKGROUND;
	uiCreateGame.background.generic.type = QMTYPE_BITMAP;
	uiCreateGame.background.generic.flags = QMF_INACTIVE;
	uiCreateGame.background.generic.x = 0;
	uiCreateGame.background.generic.y = 0;
	uiCreateGame.background.generic.width = uiStatic.width;
	uiCreateGame.background.generic.height = 768;
	uiCreateGame.background.pic = ART_BACKGROUND;

	uiCreateGame.banner.generic.id = ID_BANNER;
	uiCreateGame.banner.generic.type = QMTYPE_BITMAP;
	uiCreateGame.banner.generic.flags = QMF_INACTIVE | QMF_DRAW_ADDITIVE;
	uiCreateGame.banner.generic.x = UI_BANNER_POSX;
	uiCreateGame.banner.generic.y = UI_BANNER_POSY;
	uiCreateGame.banner.generic.width = UI_BANNER_WIDTH;
	uiCreateGame.banner.generic.height = UI_BANNER_HEIGHT;
	uiCreateGame.banner.pic = ART_BANNER;

	uiCreateGame.advOptions.generic.id = ID_ADVOPTIONS;
	uiCreateGame.advOptions.generic.type = QMTYPE_BM_BUTTON;
	uiCreateGame.advOptions.generic.flags = QMF_HIGHLIGHTIFFOCUS | QMF_DROPSHADOW | QMF_GRAYED;
	uiCreateGame.advOptions.generic.x = 72;
	uiCreateGame.advOptions.generic.y = 230;
	uiCreateGame.advOptions.generic.name = "Adv. Options";
	uiCreateGame.advOptions.generic.statusText = "Open the LAN game advanced options menu";
	uiCreateGame.advOptions.generic.callback = UI_CreateGame_Callback;

	UI_UtilSetupPicButton(&uiCreateGame.advOptions, PC_ADV_OPT);

	uiCreateGame.done.generic.id = ID_DONE;
	uiCreateGame.done.generic.type = QMTYPE_BM_BUTTON;
	uiCreateGame.done.generic.flags = QMF_HIGHLIGHTIFFOCUS | QMF_DROPSHADOW;
	uiCreateGame.done.generic.x = 72;
	uiCreateGame.done.generic.y = 280;
	uiCreateGame.done.generic.name = "Ok";
	uiCreateGame.done.generic.statusText = "Start the multiplayer game";
	uiCreateGame.done.generic.callback = UI_CreateGame_Callback;

	UI_UtilSetupPicButton(&uiCreateGame.done, PC_OK);

	uiCreateGame.cancel.generic.id = ID_CANCEL;
	uiCreateGame.cancel.generic.type = QMTYPE_BM_BUTTON;
	uiCreateGame.cancel.generic.flags = QMF_HIGHLIGHTIFFOCUS | QMF_DROPSHADOW;
	uiCreateGame.cancel.generic.x = 72;
	uiCreateGame.cancel.generic.y = 330;
	uiCreateGame.cancel.generic.name = "Cancel";
	uiCreateGame.cancel.generic.statusText = "Return to LAN game menu";
	uiCreateGame.cancel.generic.callback = UI_CreateGame_Callback;

	UI_UtilSetupPicButton(&uiCreateGame.cancel, PC_CANCEL);


	uiCreateGame.nat.generic.id = ID_NAT;
	uiCreateGame.nat.generic.type = QMTYPE_CHECKBOX;
	uiCreateGame.nat.generic.flags = QMF_HIGHLIGHTIFFOCUS | QMF_ACT_ONRELEASE | QMF_DROPSHADOW;
	uiCreateGame.nat.generic.name = "NAT";
	uiCreateGame.nat.generic.x = 72;
	uiCreateGame.nat.generic.y = 635;
	uiCreateGame.nat.generic.callback = UI_CreateGame_Callback;
	uiCreateGame.nat.generic.statusText = "Use NAT Bypass instead of direct mode";
	uiCreateGame.nat.enabled = true;

	uiCreateGame.hintMessage.generic.id = ID_TABLEHINT;
	uiCreateGame.hintMessage.generic.type = QMTYPE_ACTION;
	uiCreateGame.hintMessage.generic.flags = QMF_INACTIVE | QMF_SMALLFONT;
	uiCreateGame.hintMessage.generic.color = uiColorHelp;
	uiCreateGame.hintMessage.generic.name = uiCreateGame.hintText;
	uiCreateGame.hintMessage.generic.x = 590;
	uiCreateGame.hintMessage.generic.y = 245;

	uiCreateGame.mapsList.generic.id = ID_MAPLIST;
	uiCreateGame.mapsList.generic.type = QMTYPE_SCROLLLIST;
	uiCreateGame.mapsList.generic.flags = QMF_HIGHLIGHTIFFOCUS | QMF_SMALLFONT;
	uiCreateGame.mapsList.generic.x = 590;
	uiCreateGame.mapsList.generic.y = 245; // 245;
	uiCreateGame.mapsList.generic.width = 640;
	uiCreateGame.mapsList.generic.height = 440; // 440;
	uiCreateGame.mapsList.generic.callback = UI_CreateGame_Callback;

	uiCreateGame.hostName.generic.id = ID_HOSTNAME;
	uiCreateGame.hostName.generic.type = QMTYPE_FIELD;
	uiCreateGame.hostName.generic.flags = QMF_HIGHLIGHTIFFOCUS | QMF_DROPSHADOW;
	uiCreateGame.hostName.generic.name = "Èìÿ Ñåðâåðà";
	uiCreateGame.hostName.generic.x = 350;
	uiCreateGame.hostName.generic.y = 260;
	uiCreateGame.hostName.generic.width = 205;
	uiCreateGame.hostName.generic.height = 32;
	uiCreateGame.hostName.generic.callback = UI_CreateGame_Callback;
	uiCreateGame.hostName.maxLength = 16;
	strcpy(uiCreateGame.hostName.buffer, CVAR_GET_STRING("hostname"));

	uiCreateGame.maxClients.generic.id = ID_MAXCLIENTS;
	uiCreateGame.maxClients.generic.type = QMTYPE_FIELD;
	uiCreateGame.maxClients.generic.flags = QMF_HIGHLIGHTIFFOCUS | QMF_DROPSHADOW | QMF_NUMBERSONLY;
	uiCreateGame.maxClients.generic.name = "Max Players:";
	uiCreateGame.maxClients.generic.x = 350;
	uiCreateGame.maxClients.generic.y = 360;
	uiCreateGame.maxClients.generic.width = 205;
	uiCreateGame.maxClients.generic.height = 32;
	uiCreateGame.maxClients.maxLength = 2;

	if (CVAR_GET_FLOAT("maxplayers") <= 1)
		strcpy(uiCreateGame.maxClients.buffer, "8");
	else sprintf(uiCreateGame.maxClients.buffer, "%i", (int)CVAR_GET_FLOAT("maxplayers"));

	uiCreateGame.password.generic.id = ID_PASSWORD;
	uiCreateGame.password.generic.type = QMTYPE_FIELD;
	uiCreateGame.password.generic.flags = QMF_CENTER_JUSTIFY | QMF_HIGHLIGHTIFFOCUS | QMF_DROPSHADOW | QMF_HIDEINPUT;
	uiCreateGame.password.generic.name = "Password:";
	uiCreateGame.password.generic.x = 350;
	uiCreateGame.password.generic.y = 460;
	uiCreateGame.password.generic.width = 205;
	uiCreateGame.password.generic.height = 32;
	uiCreateGame.password.generic.callback = UI_CreateGame_Callback;
	uiCreateGame.password.maxLength = 16;

	uiCreateGame.botNum.generic.id = ID_BOTNUM;
	uiCreateGame.botNum.generic.type = QMTYPE_FIELD;
	uiCreateGame.botNum.generic.flags = QMF_HIGHLIGHTIFFOCUS | QMF_DROPSHADOW | QMF_NUMBERSONLY;
	uiCreateGame.botNum.generic.name = "Bot Quota:";
	uiCreateGame.botNum.generic.x = 350;
	uiCreateGame.botNum.generic.y = 560;
	uiCreateGame.botNum.generic.width = 205;
	uiCreateGame.botNum.generic.height = 32;
	uiCreateGame.botNum.maxLength = 2;
	strcpy(uiCreateGame.botNum.buffer, CVAR_GET_STRING("bot_quota"));

	uiCreateGame.gamemode.generic.id = ID_GAMEMODE;
	uiCreateGame.gamemode.generic.type = QMTYPE_SPINCONTROL;
	uiCreateGame.gamemode.generic.flags = QMF_CENTER_JUSTIFY | QMF_HIGHLIGHTIFFOCUS | QMF_DROPSHADOW;
	uiCreateGame.gamemode.generic.x = 620;
	uiCreateGame.gamemode.generic.y = 205;
	uiCreateGame.gamemode.generic.width = 580;
	uiCreateGame.gamemode.generic.height = 26;
	uiCreateGame.gamemode.generic.callback = UI_CreateGame_Callback;
	uiCreateGame.gamemode.minValue = 0;
	uiCreateGame.gamemode.maxValue = MAX_GAMEMODES - 1;
	uiCreateGame.gamemode.range = 1;

	const char *szGameModeCode = CVAR_GET_STRING("mp_gamemode");
	uiCreateGame.gamemode.curValue = 0;
	for (int i = 0; i < MAX_GAMEMODES; ++i)
	{
		if (stricmp(szGameModeCode, g_szGameModeCodes[i]))
			continue;
		uiCreateGame.gamemode.curValue = i;
		break;
	}
	/*using std::placeholders::_1;
	auto iter = std::find_if_not(std::begin(g_szGameModeCodes), std::end(g_szGameModeCodes), std::bind(stricmp, szGameModeCode, _1));
	uiCreateGame.gamemode.curValue = iter != std::end(g_szGameModeCodes) ? std::distance(std::begin(g_szGameModeCodes), iter) : 0;*/

	UI_CreateGame_Update();

	uiCreateGame.msgBox.generic.id = ID_MSGBOX;
	uiCreateGame.msgBox.generic.type = QMTYPE_ACTION;
	uiCreateGame.msgBox.generic.flags = QMF_INACTIVE | QMF_HIDDEN;
	uiCreateGame.msgBox.generic.ownerdraw = UI_MsgBox_Ownerdraw; // just a fill rectangle
	uiCreateGame.msgBox.generic.x = DLG_X + 192;
	uiCreateGame.msgBox.generic.y = 260;
	uiCreateGame.msgBox.generic.width = 630;
	uiCreateGame.msgBox.generic.height = 250;

	uiCreateGame.dlgMessage1.generic.id = ID_MSGTEXT;
	uiCreateGame.dlgMessage1.generic.type = QMTYPE_ACTION;
	uiCreateGame.dlgMessage1.generic.flags = QMF_INACTIVE | QMF_HIDDEN | QMF_DROPSHADOW;
	uiCreateGame.dlgMessage1.generic.name = "Starting a new game will exit";
	uiCreateGame.dlgMessage1.generic.x = DLG_X + 248;
	uiCreateGame.dlgMessage1.generic.y = 280;

	uiCreateGame.dlgMessage2.generic.id = ID_MSGTEXT;
	uiCreateGame.dlgMessage2.generic.type = QMTYPE_ACTION;
	uiCreateGame.dlgMessage2.generic.flags = QMF_INACTIVE | QMF_HIDDEN | QMF_DROPSHADOW;
	uiCreateGame.dlgMessage2.generic.name = "any current game, OK to exit?";
	uiCreateGame.dlgMessage2.generic.x = DLG_X + 248;
	uiCreateGame.dlgMessage2.generic.y = 310;

	uiCreateGame.yes.generic.id = ID_YES;
	uiCreateGame.yes.generic.type = QMTYPE_BM_BUTTON;
	uiCreateGame.yes.generic.flags = QMF_HIGHLIGHTIFFOCUS | QMF_HIDDEN | QMF_DROPSHADOW;
	uiCreateGame.yes.generic.name = "Ok";
	uiCreateGame.yes.generic.x = DLG_X + 380;
	uiCreateGame.yes.generic.y = 460;
	uiCreateGame.yes.generic.callback = UI_CreateGame_Callback;

	UI_UtilSetupPicButton(&uiCreateGame.yes, PC_OK);

	uiCreateGame.no.generic.id = ID_NO;
	uiCreateGame.no.generic.type = QMTYPE_BM_BUTTON;
	uiCreateGame.no.generic.flags = QMF_HIGHLIGHTIFFOCUS | QMF_HIDDEN | QMF_DROPSHADOW;
	uiCreateGame.no.generic.name = "Cancel";
	uiCreateGame.no.generic.x = DLG_X + 530;
	uiCreateGame.no.generic.y = 460;
	uiCreateGame.no.generic.callback = UI_CreateGame_Callback;

	UI_UtilSetupPicButton(&uiCreateGame.no, PC_CANCEL);

	UI_CreateGame_GetMapsList();

	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.background);
	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.banner);
	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.advOptions);
	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.done);
	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.cancel);
	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.maxClients);
	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.hostName);
	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.password);
	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.botNum);
	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.gamemode);
	if (CVAR_GET_FLOAT("public"))
		UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.nat);
	//UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.hintMessage);
	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.mapsList);
	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.msgBox);
	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.dlgMessage1);
	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.dlgMessage2);
	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.no);
	UI_AddItem(&uiCreateGame.menu, (void *)&uiCreateGame.yes);
}

/*
=================
UI_CreateGame_Precache
=================
*/
void UI_CreateGame_Precache(void)
{
	PIC_Load(ART_BACKGROUND);
	PIC_Load(ART_BANNER);
}

/*
=================
UI_CreateGame_Menu
=================
*/
void UI_CreateGame_Menu(void)
{
	if (gMenu.m_gameinfo.gamemode == GAME_SINGLEPLAYER_ONLY)
		return;

	UI_CreateGame_Precache();
	UI_CreateGame_Init();

	UI_PushMenu(&uiCreateGame.menu);
}
