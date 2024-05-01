#pragma once
#ifndef PROJECT_ZB2_ZCLASS_DEIMOS2_H
#define PROJECT_ZB2_ZCLASS_DEIMOS2_H

#include "zb2_zclass.h"

class CZombieClass_Deimos2 : public CBaseZombieClass_ZB2
{
	public:
		CZombieClass_Deimos2(CBasePlayer* player, ZombieLevel lv);
		void InitHUD() const override;
		void ResetMaxSpeed() const override;
		float AdjustDamageTaken(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) const override;
		bool ApplyKnockback(CBasePlayer* attacker, const KnockbackData& kbd) override;
		void Pain_Zombie(int m_LastHitGroup, bool HasArmour) override;
		void DeathSound_Zombie() override;
		void OnWeaponDeploy(CBasePlayerItem* item) override;
		ZombieSkillStatus m_iCrazySkillStatus;
};



#endif //PROJECT_ZB2_ZCLASS_DEIMOS2_H
