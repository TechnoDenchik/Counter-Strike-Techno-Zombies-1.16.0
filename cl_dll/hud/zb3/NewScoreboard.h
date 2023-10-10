#pragma once

#include "hud_sub.h"
#include "r_texture.h"
#include <memory>

class CHudZB3ScoreBoard : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;
	int m_iTeamScore_T, m_iTeamScore_CT;


protected:
	UniqueTexture newscoreboard;
	UniqueTexture healthboard;
	UniqueTexture ammoboard;
	UniqueTexture weaponboard;
	UniqueTexture iconround;
	wrect_t m_rcTeamnumber[32];
	UniqueTexture winhm;
	wrect_t m_rcSelfnumber[32];
	UniqueTexture winzb;
	wrect_t m_rcToprecord[32];
	UniqueTexture countplayer;
	int m_iSelfKills;
	float m_iSelfnumberScale;
	float m_flSelfnumberScaleTime;

private:
	int m_iBGIndex;
	int m_iTextIndex;
	int m_iTTextIndex, m_iCTTextIndex;

	wrect_t m_rcNumber_Large[10];
	wrect_t m_rcNumber_Small[10];

	int m_iOriginalBG;
	int m_iTeamDeathBG;
	int m_iUnitehBG;
	int m_iNum_L;
	int m_iNum_S;
	int m_iText_CT;
	int m_iText_T;
	int m_iText_TR;
	int m_iText_HM;
	int m_iText_ZB;
	int m_iText_1st;
	int m_iText_Kill;
	int m_iText_Round;
};
