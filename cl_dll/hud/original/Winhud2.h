#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudOrigWins : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void winsct();
	void winst();

protected:
	SharedTexture m_pCurTexture;
	SharedTexture winct;
	SharedTexture wint;
	float m_flDisplayTime;
};