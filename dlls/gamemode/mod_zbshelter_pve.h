#ifndef MOD_ZSH
#define MOD_ZSH_H
#ifdef _WIN32
#pragma once
#endif

#include "mods.h"
#include "ruleof350.h"
#include "monsters.h"
#include <memory>
#include "mod_base.h"

class CBaseEntity;
class CBasePlayer;
typedef struct entvars_s entvars_t;

class CMod_ZombieShelter_coop : public IBaseMod, public CHalfLifeMultiplay, ruleof350::unique
{
public:
	// CMod_ZombieShelter_coop();
	 virtual void Precache();
public:
   
	void CheckMapConditions() override;
	BOOL IsTeamplay(void) override { return TRUE; }
	void UpdateGameMode(CBasePlayer* pPlayer) override;
	//void RestartRound() override;
	void PlayerSpawn(CBasePlayer* pPlayer) override;
	//void Think() override;
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