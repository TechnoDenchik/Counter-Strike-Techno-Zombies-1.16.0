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


#include "mod_zb1.h"
#include "zb2/zb2_const.h"

CMod_ZombieShelter_coop::CMod_ZombieShelter_coop() // precache
{
	PRECACHE_SOUND("zsh/BGM_start.wav");
	CVAR_SET_FLOAT("sv_maxspeed", 990);
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
	IBaseMod::ClientDisconnected(pClient);
}

void CMod_ZombieShelter_coop::Think()
{
	//IBaseMod::Think();

	static int iLastCountDown = -1;
	int const iCountDown = static_cast<int>(gpGlobals->time - m_fRoundCount);

	if (iCountDown != iLastCountDown)
	{
		iLastCountDown = iCountDown;
		if (iCountDown > 0 && iCountDown < 20 && !m_bFreezePeriod)
		{

			UTIL_ClientPrintAll(HUD_PRINTCENTER, "Time Remaining: %s1 Sec", UTIL_dtos1(20 - iCountDown));

			if (iCountDown == 1)
			{
				for (CBasePlayer* player : moe::range::PlayersList())
					CLIENT_COMMAND(player->edict(), "spk sound/zsh/BGM_start.wav");

			}
			
		}
		else if (iCountDown == 20)
		{
			
		}
	
	}

	if (CheckGameOver())   // someone else quit the game already
		return;

	if (CheckTimeLimit())
		return;

	if (IsFreezePeriod())
	{
		CheckFreezePeriodExpired();
	}

	if (m_fTeamCount != 0.0f && m_fTeamCount <= gpGlobals->time)
	{
		RestartRound();
	}

	CheckLevelInitialized();

	if (gpGlobals->time > m_tmNextPeriodicThink)
	{
		CheckRestartRound();
		m_tmNextPeriodicThink = gpGlobals->time + 1.0f;

		if (g_psv_accelerate->value != 5.0f)
		{
			CVAR_SET_FLOAT("sv_accelerate", 5.0);
		}

		if (g_psv_friction->value != 4.0f)
		{
			CVAR_SET_FLOAT("sv_friction", 4.0);
		}

		if (g_psv_stopspeed->value != 75.0f)
		{
			CVAR_SET_FLOAT("sv_stopspeed", 75.0);
		}

		m_iMaxRounds = (int)maxrounds.value;

		if (m_iMaxRounds < 0)
		{
			m_iMaxRounds = 0;
			CVAR_SET_FLOAT("mp_maxrounds", 33);
		}

		m_iMaxRoundsWon = (int)winlimit.value;

		if (m_iMaxRoundsWon < 0)
		{
			m_iMaxRoundsWon = 0;
			CVAR_SET_FLOAT("mp_winlimit", 0);
		}
	}
}


void CMod_ZombieShelter_coop::PlayerSpawn(CBasePlayer* pPlayer)
{
	IBaseMod::PlayerSpawn(pPlayer);
	for (CBasePlayer* pPlayer : moe::range::PlayersList())
		CLIENT_COMMAND(pPlayer->edict(), "spk sound/zsh/BGM_start.wav");
}