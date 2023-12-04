#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "client.h"
#include "bmodels.h"
#include "mod_zbshelter_pve.h"
#include <algorithm>
#include <vector>
#include <random>
#include "mod_base.h"
#include "bot_include.h"
#include "util/u_range.hpp"


void CMod_ZombieShelter_coop::Precache() // precache
{
	//PRECACHE_SOUND("zb3/human_death_01.wav");
	//PRECACHE_SOUND("zb3/human_death_02.wav");
	PRECACHE_SOUND("zsh/BGM_start.wav");

	CVAR_SET_FLOAT("sv_maxspeed", 490);
}

void CMod_ZombieShelter_coop::CheckMapConditions()
{
	CVAR_SET_STRING("sv_skyname", "hk"); // it should work, but...
	//	CVAR_SET_FLOAT("sv_skycolor_r", 150);
	//	CVAR_SET_FLOAT("sv_skycolor_g", 150);
	//	CVAR_SET_FLOAT("sv_skycolor_b", 150);

		// create fog, however it doesnt work...
	CBaseEntity* fog = nullptr;
	while ((fog = UTIL_FindEntityByClassname(fog, "env_fog")) != nullptr)
	{
		REMOVE_ENTITY(fog->edict());
	}
	CClientFog* newfog = GetClassPtr<CClientFog>(nullptr);
	MAKE_STRING_CLASS("env_fog", newfog->pev);
	newfog->Spawn();
	newfog->m_fDensity = 0.0016f;
	newfog->pev->rendercolor = { 0,0,0 };

	// light
	LIGHT_STYLE(0, "g"); // previous one is "f"
}

void CMod_ZombieShelter_coop::UpdateGameMode(CBasePlayer* pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, nullptr, pPlayer->edict());
	WRITE_BYTE(MOD_ZSH);
	WRITE_BYTE(0); // Reserved. (weapon restriction? )
	WRITE_BYTE(static_cast<int>(maxrounds.value)); // MaxRound (mp_roundlimit)
	WRITE_BYTE(0); // Reserved. (MaxTime?)

	MESSAGE_END();
}

BOOL CMod_ZombieShelter_coop::ClientConnected(edict_t* pEntity, const char* pszName, const char* pszAddress, char* szRejectReason)
{
	
	return 1;//IBaseMod::ClientConnected(pEntity, pszName, pszAddress, szRejectReason);
}

void CMod_ZombieShelter_coop::ClientDisconnected(edict_t* pClient)
{
	CLIENT_COMMAND(pClient, "mp3 stop\n");

	IBaseMod::ClientDisconnected(pClient);
}

void CMod_ZombieShelter_coop::PlayerSpawn(CBasePlayer* pPlayer)
{
	pPlayer->m_bNotKilled = false;
	IBaseMod::PlayerSpawn(pPlayer);
	pPlayer->AddAccount(16000);
	//EMIT_SOUND(pEntity, CHAN_BODY, "zsh/BGM_start.wav", VOL_NORM, ATTN_NORM);
	// Open buy menu on spawn
	//ShowVGUIMenu(pPlayer, VGUI_Menu_Buy, (MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_3 | MENU_KEY_4 | MENU_KEY_5 | MENU_KEY_6 | MENU_KEY_7 | MENU_KEY_8 | MENU_KEY_0), "#Buy");
	//pPlayer->m_iMenu = Menu_Buy;
}