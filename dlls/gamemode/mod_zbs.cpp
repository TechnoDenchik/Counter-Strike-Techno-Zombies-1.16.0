#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "client.h"
#include "globals.h"

#include "mod_zbs.h"

#include "zbs/zs_subs.h"
#include "zbs/zbs_const.h"
#include "zbs/monster_entity.h"
#include "zbs/monster_entity2.h"
#include "player/csdm_randomspawn.h"
#include "zbs/monster_manager.h"

#include "player/player_human_level.h"

#include <algorithm>

class PlayerModStrategy_ZBS : public CPlayerModStrategy_Default
{
public:
	PlayerModStrategy_ZBS(CBasePlayer *player, CMod_ZombieScenario *mp) : CPlayerModStrategy_Default(player), m_HumanLevel(player)
	{
		m_listenerAdjustDamage = mp->m_eventAdjustDamage.subscribe(
			[=](CBasePlayer *attacker, float &out) 
			{ 
				if(attacker == m_pPlayer)
					out *= m_HumanLevel.GetAttackBonus(); 
			}
		);
		m_listenerMonsterKilled = mp->m_eventMonsterKilled.subscribe(
			[=](CMonster *victim,  CBaseEntity *attacker)
			{
				if (attacker == m_pPlayer)
				{
					if (victim->m_iKillBonusFrags)
						m_pPlayer->AddPoints(victim->m_iKillBonusFrags, FALSE);
					if (victim->m_iKillBonusMoney)
						m_pPlayer->AddAccount(victim->m_iKillBonusMoney);
					m_HumanLevel.LevelUpHealth();
					m_HumanLevel.LevelUpAttack();
					MESSAGE_BEGIN(MSG_ONE, gmsgZBSTip, NULL, m_pPlayer->pev);
					WRITE_BYTE(ZBS_TIP_KILL);
					MESSAGE_END();
				}
			}
		);
	}
	int  ComputeMaxAmmo( const char *szAmmoClassName, int iOriginalMax) override { return 600; }
	bool CanPlayerBuy(bool display) override
	{
		// is the player alive?
		if (m_pPlayer->pev->deadflag != DEAD_NO)
			return false;

		return true;
	}

	void OnSpawn() override
	{
		m_pPlayer->pev->health += m_HumanLevel.GetHealthBonus();
	}

	void OnInitHUD() override
	{
		m_HumanLevel.UpdateHUD();
	}

protected:
	EventListener m_listenerAdjustDamage;
	EventListener m_listenerMonsterKilled;
	PlayerExtraHumanLevel_ZBS m_HumanLevel;

};



class PlayerModStrategy_ZBS2 : public CPlayerModStrategy_Default
{
public:
	PlayerModStrategy_ZBS2(CBasePlayer* player, CMod_ZombieScenario* mp) : CPlayerModStrategy_Default(player), m_HumanLevel2(player)
	{
		m_listenerAdjustDamage2 = mp->m_eventAdjustDamage.subscribe(
			[=](CBasePlayer* attacker, float& out)
			{
				if (attacker == m_pPlayer)
					out *= m_HumanLevel2.GetAttackBonus();
			}
		);
		m_listenerMonsterKilled2 = mp->m_eventMonsterKilled2.subscribe(
			[=](CMonster2* victim, CBaseEntity* attacker)
			{
				if (attacker == m_pPlayer)
				{
					if (victim->m_iKillBonusFrags)
						m_pPlayer->AddPoints(victim->m_iKillBonusFrags, FALSE);
					if (victim->m_iKillBonusMoney)
						m_pPlayer->AddAccount(victim->m_iKillBonusMoney);
					m_HumanLevel2.LevelUpHealth();
					m_HumanLevel2.LevelUpAttack();
					MESSAGE_BEGIN(MSG_ONE, gmsgZBSTip, NULL, m_pPlayer->pev);
					WRITE_BYTE(ZBS_TIP_KILL);
					MESSAGE_END();
				}
			}
		);
	}
	int  ComputeMaxAmmo(const char* szAmmoClassName, int iOriginalMax) override { return 600; }
	bool CanPlayerBuy(bool display) override
	{
		// is the player alive?
		if (m_pPlayer->pev->deadflag != DEAD_NO)
			return false;

		return true;
	}

	void OnSpawn() override
	{
		m_pPlayer->pev->health += m_HumanLevel2.GetHealthBonus();
	}

	void OnInitHUD() override
	{
		m_HumanLevel2.UpdateHUD();
	}

protected:
	EventListener m_listenerAdjustDamage2;
	EventListener m_listenerMonsterKilled2;
	PlayerExtraHumanLevel_ZBS m_HumanLevel2;

};




class CMonsterModStrategy_ZBS : public CMonsterModStrategy_Default
{
public:
	CMonsterModStrategy_ZBS(CMonster *p,  CMod_ZombieScenario * pGameRules) : CMonsterModStrategy_Default(p), mp(pGameRules)
	{

	}

	void OnKilled(entvars_t *pevKiller,  int iGib) override
	{
		CMonsterModStrategy_Default::OnKilled(pevKiller, iGib);
		if (pevKiller)
		{
			CBaseEntity *pKiller = CBaseEntity::Instance(pevKiller);
			mp->m_eventMonsterKilled.dispatch(m_pMonster, pKiller);
		}
	}

protected:
	CMod_ZombieScenario * const mp;
};



class CMonsterModStrategy_ZBS2 : public CMonsterModStrategy_Default2
{
public:
	CMonsterModStrategy_ZBS2(CMonster2 *p, CMod_ZombieScenario* pGameRules) : CMonsterModStrategy_Default2(p), mp(pGameRules)
	{

	}

	void OnKilled(entvars_t* pevKiller, int iGib) override
	{
		CMonsterModStrategy_Default2::OnKilled(pevKiller, iGib);
		if (pevKiller)
		{
			CBaseEntity* pKiller = CBaseEntity::Instance(pevKiller);
			mp->m_eventMonsterKilled2.dispatch(m_pMonster, pKiller);
		}
	}

protected:
	CMod_ZombieScenario* const mp;
};





void CMod_ZombieScenario::InstallPlayerModStrategy(CBasePlayer *player)
{
	player->m_pModStrategy.reset(new PlayerModStrategy_ZBS(player, this));
	player->m_pModStrategy.reset(new PlayerModStrategy_ZBS2(player, this));
}

float CMod_ZombieScenario::GetAdjustedEntityDamage(CBaseEntity *victim, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	flDamage = Base::GetAdjustedEntityDamage(victim, pevInflictor, pevAttacker, flDamage, bitsDamageType);

	if(pevAttacker)
	{
		CBaseEntity *pAttackingEnt = CBaseEntity::Instance(pevAttacker);
		if (pAttackingEnt->IsPlayer())
		{
			CBasePlayer *pAttacker = dynamic_cast<CBasePlayer *>(pAttackingEnt);
			m_eventAdjustDamage.dispatch(pAttacker, flDamage);
		}
	}
		
	return flDamage;
}

CMod_ZombieScenario::CMod_ZombieScenario()
{
	m_iRoundTimeSecs = m_iIntroRoundTime = 20 + 2; // keep it from ReadMultiplayCvars
	
	PRECACHE_GENERIC("sound/zbs/Scenario_Ready.mp3");
	PRECACHE_MODEL("models/player/zombi_origin/zombi_origin.mdl");
	PRECACHE_MODEL("models/player/zombi_host/zombi_host.mdl");
	PRECACHE_MODEL("models/player/heavy_zombi_host/heavy_zombi_host.mdl");
	PRECACHE_MODEL("models/player/speed_zombi_origin/speed_zombi_origin.mdl");
}

void CMod_ZombieScenario::UpdateGameMode(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, NULL, pPlayer->edict());
	WRITE_BYTE(MOD_ZBS);
	WRITE_BYTE(0); // Reserved. (weapon restriction? )
	WRITE_BYTE(maxrounds.value); // MaxRound (mp_roundlimit)
	WRITE_BYTE(0); // Reserved. (MaxTime?)
	MESSAGE_END();
}

void CMod_ZombieScenario::CheckMapConditions()
{
	m_vecZombieSpawns.clear();
	CBaseEntity *sp = nullptr;
	while ((sp = UTIL_FindEntityByClassname(sp, "zombiespawn")) != nullptr)
	{
		m_vecZombieSpawns.push_back(static_cast<CZombieSpawn *>(sp));
	}

	// hook from RestartRound()
	m_iRoundTimeSecs = m_iIntroRoundTime = 20 + 2; // keep it from ReadMultiplayCvars

	return Base::CheckMapConditions();
}

void CMod_ZombieScenario::RestartRound()
{
	ClearZombieNPC();
	IBaseMod::RestartRound();
}

void CMod_ZombieScenario::PlayerSpawn(CBasePlayer *pPlayer)
{
	IBaseMod::PlayerSpawn(pPlayer);
}

void CMod_ZombieScenario::WaitingSound()
{
	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		CBaseEntity *entity = UTIL_PlayerByIndex(iIndex);
		if (!entity)
			continue;
		CLIENT_COMMAND(entity->edict(), "mp3 play sound/zbs/Scenario_Ready.mp3\n");
	}
}

void CMod_ZombieScenario::Think()
{
	TeamCheck();

	if (m_fTeamCount != 0.0f && m_fTeamCount <= gpGlobals->time)
	{
		if (m_iNumTerroristWins)
			g_fGameOver = TRUE; // Game over, changelevel in CheckGameOver().
		else
			RestartRound();
	}

	if (CheckGameOver())   // someone else quit the game already
		return;

	if (IsFreezePeriod())
	{
		static int iLastCountDown = -1;
		int iCountDown = TimeRemaining();

		if (iCountDown > 0)
		{
			if (iCountDown != iLastCountDown)
			{
				iLastCountDown = iCountDown;
				if (iCountDown > 0 && iCountDown < 20)
				{
					UTIL_ClientPrintAll(HUD_PRINTCENTER, "Waiting for Round Start: %s1 sec(s)", UTIL_dtos1(iCountDown)); // #CSO_ZBS_StartCount
				}

				if (iCountDown == 19)
				{
					WaitingSound();
				}
			}
		}
		else
		{
			RoundStart();
		}

		CheckFreezePeriodExpired();
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
			CVAR_SET_FLOAT("mp_maxrounds", 0);
		}

		m_iMaxRoundsWon = (int)winlimit.value;

		if (m_iMaxRoundsWon < 0)
		{
			m_iMaxRoundsWon = 0;
			CVAR_SET_FLOAT("mp_winlimit", 0);
		}
	}

	if (FRoundStarted() && !m_bRoundTerminating)
	{
	
		if (gpGlobals->time > m_flNextSpawnNPC)
		{
			MakeZombieNPC();
			MakeZombieNPC2();
			MakeZombieNPC3();
			m_flNextSpawnNPC = gpGlobals->time + 3.0f;
		}
	}

	if (TimeRemaining() <= 0 && !m_bRoundTerminating)
		HumanWin();
}

void CMod_ZombieScenario::CheckWinConditions()
{
	// If a winner has already been determined and game of started.. then get the heck out of here
	if (m_bFirstConnected && m_iRoundWinStatus != WINNER_NONE)
	{
		return;
	}

	int NumDeadCT, NumDeadTerrorist, NumAliveTerrorist, NumAliveCT;
	InitializePlayerCounts(NumAliveTerrorist, NumAliveCT, NumDeadTerrorist, NumDeadCT);

	if (!NumAliveCT)
	{
		ZombieWin();
	}

}

void CMod_ZombieScenario::HumanWin()
{
	MESSAGE_BEGIN(MSG_ALL, gmsgZBSTip);
	WRITE_BYTE(ZBS_TIP_ROUNDCLEAR);
	MESSAGE_END();

	TerminateRound(5, WINSTATUS_CTS);
	++m_iNumCTWins;
	UpdateTeamScores();
	ClearZombieNPC();

	if (m_iMaxRoundsWon && m_iNumCTWins >= m_iMaxRoundsWon)
	{
		UTIL_ClientPrintAll(HUD_PRINTCENTER, "Congratulations! You've cleared all the Rounds."); // #CSO_CongAllRoundClear
	}
}

void CMod_ZombieScenario::ZombieWin()
{
	MESSAGE_BEGIN(MSG_ALL, gmsgZBSTip);
	WRITE_BYTE(ZBS_TIP_ROUNDFAIL);
	MESSAGE_END();

	TerminateRound(5, WINSTATUS_TERRORISTS);
	++m_iNumTerroristWins;
	UpdateTeamScores();
}

void CMod_ZombieScenario::TeamCheck()
{
	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		CBaseEntity *entity = UTIL_PlayerByIndex(iIndex);
		if (!entity)
			continue;
		CBasePlayer *player = dynamic_cast<CBasePlayer *>(entity);

		if (player->m_iTeam == TERRORIST)
		{
			player->m_iTeam = CT;
			TeamChangeUpdate(player, player->m_iTeam);
		}
	}
}

void CMod_ZombieScenario::RoundStart()
{
	m_flNextSpawnNPC = gpGlobals->time;
}

BOOL CMod_ZombieScenario::FRoundStarted()
{
	return !IsFreezePeriod();
}

CZombieSpawn *CMod_ZombieScenario::SelectZombieSpawnPoint()
{
	size_t const iSize = m_vecZombieSpawns.size();
	if (!iSize)
		return nullptr;
	return m_vecZombieSpawns[RANDOM_LONG(0, iSize - 1)];
}

CBaseEntity *CMod_ZombieScenario::MakeZombieNPC()
{
	CMonster *monster = GetClassPtr<CMonster>(nullptr);

	if (!monster)
		return nullptr;

	edict_t *pent = monster->edict();

	CZombieSpawn *sp = SelectZombieSpawnPoint();
	if (sp)
	{
		monster->pev->origin = sp->pev->origin;
		monster->pev->angles = sp->pev->angles;
	}
	else
	{
		Vector backup_v_angle = monster->pev->v_angle;
		CSDM_DoRandomSpawn(monster);
		monster->pev->v_angle = backup_v_angle;
	}

	pent->v.spawnflags |= SF_NORESPAWN;
	
	DispatchSpawn(pent);

	// default settings
	monster->pev->health = monster->pev->max_health = 100 + m_iNumCTWins * 15;
	monster->pev->maxspeed = 200.0f + (m_iNumCTWins / static_cast<float>(3)) * 15;
	monster->m_flAttackDamage = (0.2f * m_iNumCTWins + 1) * (0.2f * m_iNumCTWins + 1);

	if (m_iNumCTWins < 5 || RANDOM_LONG(0, 3))
	{
		monster->pev->health = monster->pev->max_health = monster->pev->max_health / 2;
		SET_MODEL(monster->edict(), "models/player/zombi_origin/zombi_origin.mdl");
		UTIL_SetSize(monster->pev, VEC_HULL_MIN, VEC_HULL_MAX);
	}

	monster->m_pMonsterStrategy.reset(new CMonsterModStrategy_ZBS(monster, this));

	return monster;
}

CBaseEntity* CMod_ZombieScenario::MakeZombieNPC2()
{
	CMonster* monster = GetClassPtr<CMonster>(nullptr);

	if (!monster)
		return nullptr;

	edict_t* pent = monster->edict();

	CZombieSpawn* sp = SelectZombieSpawnPoint();
	if (sp)
	{
		monster->pev->origin = sp->pev->origin;
		monster->pev->angles = sp->pev->angles;
	}
	else
	{
		Vector backup_v_angle = monster->pev->v_angle;
		CSDM_DoRandomSpawn(monster);
		monster->pev->v_angle = backup_v_angle;
	}

	pent->v.spawnflags |= SF_NORESPAWN;

	DispatchSpawn(pent);

	// default settings
	monster->pev->health = monster->pev->max_health = 100 + m_iNumCTWins * 15;
	monster->pev->maxspeed = 200.0f + (m_iNumCTWins / static_cast<float>(3)) * 15;
	monster->m_flAttackDamage = (0.2f * m_iNumCTWins + 1) * (0.2f * m_iNumCTWins + 1);

	if (m_iNumCTWins < 5 || RANDOM_LONG(0, 3))
	{
		monster->pev->health = monster->pev->max_health = monster->pev->max_health / 2;
		SET_MODEL(monster->edict(), "models/player/heavy_zombi_host/heavy_zombi_host.mdl");
		UTIL_SetSize(monster->pev, VEC_HULL_MIN, VEC_HULL_MAX);
	}

	monster->m_pMonsterStrategy.reset(new CMonsterModStrategy_ZBS(monster, this));

	return monster;
}

CBaseEntity* CMod_ZombieScenario::MakeZombieNPC3()
{
	CMonster2* monster = GetClassPtr<CMonster2>(nullptr);

	if (!monster)
		return nullptr;

	edict_t* pent = monster->edict();

	CZombieSpawn* sp = SelectZombieSpawnPoint();
	if (sp)
	{
		monster->pev->origin = sp->pev->origin;
		monster->pev->angles = sp->pev->angles;
	}
	else
	{
		Vector backup_v_angle = monster->pev->v_angle;
		CSDM_DoRandomSpawn(monster);
		monster->pev->v_angle = backup_v_angle;
	}

	pent->v.spawnflags |= SF_NORESPAWN;

	DispatchSpawn(pent);

	// default settings
	monster->pev->health = monster->pev->max_health = 100 + m_iNumCTWins * 15;
	monster->pev->maxspeed = 200.0f + (m_iNumCTWins / static_cast<float>(3)) * 15;
	monster->m_flAttackDamage = (0.2f * m_iNumCTWins + 1) * (0.2f * m_iNumCTWins + 1);

	if (m_iNumCTWins < 5 || RANDOM_LONG(0, 3))
	{
		monster->pev->health = monster->pev->max_health = monster->pev->max_health / 2;
		SET_MODEL(monster->edict(), "models/player/speed_zombi_origin/speed_zombi_origin.mdl");
		UTIL_SetSize(monster->pev, VEC_HULL_MIN, VEC_HULL_MAX);
	}

	monster->m_pMonsterStrategy2.reset(new CMonsterModStrategy_ZBS2(monster, this));

	return monster;
}


void CMod_ZombieScenario::ClearZombieNPC()
{
	CBaseEntity *npc = nullptr;
	while ((npc = UTIL_FindEntityByClassname(npc, "monster_entity")) != nullptr)
	{
		npc->Killed(nullptr, GIB_NORMAL);
	}
}