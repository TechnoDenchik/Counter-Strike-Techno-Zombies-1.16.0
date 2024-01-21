#include "r_texture.h"
#include <memory>
#include "hud_sub.h"

class CHudZB3ScoreBoard : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

protected:
	
	UniqueTexture newscoreboard;
	UniqueTexture slash;
	UniqueTexture textstring;
	UniqueTexture iconhm;
	UniqueTexture iconzb;
	UniqueTexture ammoboard;
	UniqueTexture weaponboard;
	UniqueTexture iconround;
	UniqueTexture winhm;
	UniqueTexture winzb;
	UniqueTexture countplayer;
	UniqueTexture countplayer2;
	UniqueTexture countround;
	SharedTexture iconwinhm;
	SharedTexture iconwinzb;
	SharedTexture m_pCurTexture;
	
	wrect_t m_rcroundmax[10];
	wrect_t m_rcroundnumber[10];
	wrect_t m_rcTeamnumber[10];
	wrect_t m_rcSelfnumber[10];
	wrect_t m_rcToprecord[10];
	wrect_t m_rcToprecord2[10];

	float m_flDisplayTime;	

};
