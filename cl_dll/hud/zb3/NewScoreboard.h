#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudZB3ScoreBoard : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

protected:
	UniqueTexture newscoreboard;
	wrect_t m_rcTeamnumber[10];
	UniqueTexture winhm;
	wrect_t m_rcSelfnumber[10];
	UniqueTexture winzb;
	wrect_t m_rcToprecord[10];
	UniqueTexture countplayer;
	int m_iSelfKills;
	float m_iSelfnumberScale;
	float m_flSelfnumberScaleTime;
};
