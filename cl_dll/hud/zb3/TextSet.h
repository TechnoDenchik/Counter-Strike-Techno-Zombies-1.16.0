#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudTextZB3 : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void Settext();

protected:
	SharedTexture m_pCurTexture;
	SharedTexture stringtext;
	float m_flDisplayTime;
};

