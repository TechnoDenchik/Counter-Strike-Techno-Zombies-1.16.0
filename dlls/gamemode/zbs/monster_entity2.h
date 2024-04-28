
#ifndef ENTITY_MONSTER2_H
#define ENTITY_MONSTER2_H
#ifdef _WIN32
#pragma once
#endif

#include "monster_entity.h"
#include "hostage/hostage.h"
#include <memory>
#include <map>
#include <string>
#include <mutex>


class CMonster2;

struct BaseMonsterExtra2
{
public:
	BaseMonsterExtra2(CMonster2* p) : m_pMonster(p) {}

public:
	CMonster2* const m_pMonster;
};

class IBaseMonsterStrategy2 : public BaseMonsterExtra2
{
public:
	IBaseMonsterStrategy2(CMonster2* p) : BaseMonsterExtra2(p) {}
	virtual ~IBaseMonsterStrategy2() = 0;

	virtual void OnSpawn() = 0;
	virtual void OnThink() = 0;
	virtual void OnKilled(entvars_t* pKiller, int iGib) = 0;

	virtual void DeathSound() const = 0;
	virtual bool IsTeamMate(CBaseEntity* that) const = 0;
};

inline IBaseMonsterStrategy2::~IBaseMonsterStrategy2() {}

class CMonsterModStrategy_Default2 : public IBaseMonsterStrategy2
{
public:
	CMonsterModStrategy_Default2(CMonster2* p) : IBaseMonsterStrategy2(p) {}

	void OnSpawn() override;
	void OnThink() override;
	void OnKilled(entvars_t* pKiller, int iGib) override;

	void DeathSound() const override;
	bool IsTeamMate(CBaseEntity* that) const override { return false; }
};

class CMonster2 : public CHostage
{
public:
	void Spawn() override;
	void Precache() override;
	int ObjectCaps() override { return (CBaseMonster::ObjectCaps() | FCAP_MUST_SPAWN); }
	int Classify() override { return CLASS_PLAYER_ALLY; }
	int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	void BecomeDead(void) override;
	void Killed(entvars_t* pevAttacker, int iGib) override;
	int BloodColor() override { return BLOOD_COLOR_RED; }
	void Touch(CBaseEntity* pOther) override;
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) override;

	// RAII support to prevent memory leak.
	CMonster2();
	~CMonster2() override;

public:
	int LookupSequence(const char* label);

public:
	void EXPORT IdleThink();
	void Remove();

	void Wander();

	CBaseEntity* CheckTraceHullAttack(float flDist, int iDamage, int iDmgType);
	bool ShouldAttack(CBaseEntity* target) const;

	void SetAnimation(MonsterAnim anim);

	bool CheckTarget();
	bool CheckAttack();
	bool CheckSequence();

protected:
	// pTarget, bCanSee
	std::pair<CBasePlayer*, bool> FindTarget() const;
	CBasePlayer* GetClosestPlayer(bool bVisible) const;
	float GetModifiedDamage(float flDamage, int nHitGroup) const;

public:
	float m_flAttackDist;
	float m_flAttackRate;
	float m_flAttackAnimTime;
	float m_flAttackDamage;
	int m_iKillBonusMoney;
	int m_iKillBonusFrags;
	int m_iKillBonusMoney2;
	int m_iKillBonusFrags2;
	float m_flTimeLastActive;
	float m_flTargetChange;

	std::map<std::string, int> m_mapLookupSequenceCache;
	std::mutex m_mutexSetAnimation;

public:
	std::unique_ptr<IBaseMonsterStrategy2> m_pMonsterStrategy2;

};

#endif

