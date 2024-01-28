#pragma once

#include "hud_sub.h"
#include "r_texture.h"

class CHudWinOriginal : public IBaseHudSub
{
public:
	int VidInit(void) override;
	int Draw(float time) override;

	void winct();
	void wint();

protected:
	SharedTexture m_pCurTexture;
	SharedTexture iconwinct;
	SharedTexture iconwint;
	float m_flDisplayTime;
};