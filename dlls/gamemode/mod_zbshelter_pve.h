#ifndef MOD_ZSH_H
#define MOD_ZSH_H
#ifdef _WIN32
#pragma once
#endif

#include "mods.h"
#include "ruleof350.h"
#include "monsters.h"
#include <memory>
#include "mod_base.h"

#include "zb1/zb1_zclass.h"
#include <vector>
#include <utility>
#include "player/player_mod_strategy.h"
#include "EventDispatcher.h"

class CBaseEntity;
class CBasePlayer;
typedef struct entvars_s entvars_t;

class CMod_ZombieShelter_coop : public TBaseMod_RemoveObjects<TBaseMod_RandomSpawn<>>
{ 
public:
	 CMod_ZombieShelter_coop();
public:
   
	void CheckMapConditions() override;
	//BOOL IsTeamplay(void) override { return TRUE; }
	void UpdateGameMode(CBasePlayer* pPlayer) override;
	//void RestartRound() override;
	void PlayerSpawn(CBasePlayer* pPlayer) override;
	void Think() override;
	BOOL ClientConnected(edict_t* pEntity, const char* pszName, const char* pszAddress, char* szRejectReason) override;
	void ClientDisconnected(edict_t* pClient) override;
	//BOOL FPlayerCanTakeDamage(CBasePlayer* pPlayer, CBaseEntity* pAttacker) override;
	//void CheckWinConditions() override;
	//int IPointsForKill(CBasePlayer* pAttacker, CBasePlayer* pKilled) override;
	//void PlayerKilled(CBasePlayer* pVictim, entvars_t* pKiller, entvars_t* pInflictor) override;

protected:

private:
	


};

#endif