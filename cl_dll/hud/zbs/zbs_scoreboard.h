
#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudZBSScoreBoard : public IBaseHudSub, public CHudBase
{
	friend class CHudSpectatorGui;
	friend class CHudScenarioStatus;
public:
	int VidInit(void) override;
	int Draw(float time) override;


protected:
	UniqueTexture healthboard;
	UniqueTexture healthboard2;
	UniqueTexture ammoboard;
	UniqueTexture weaponboard;
	UniqueTexture iconround;
	UniqueTexture countplayer;
	UniqueTexture icontotalkill;
	UniqueTexture iconkill;

	UniqueTexture m_pBackground;
	wrect_t m_rcTeamnumber[10];
	UniqueTexture m_pTeamnumber;
	wrect_t m_rcSelfnumber[10];
	UniqueTexture m_pSelfnumber;
	UniqueTexture killcount;
	wrect_t m_rcToprecord[10];
	wrect_t m_rcToprecord2[10];
	wrect_t m_rcToprecord3[10];
	UniqueTexture m_pToprecord;
	int m_iSelfKills;
	float m_iSelfnumberScale;
	float m_flSelfnumberScaleTime;
};