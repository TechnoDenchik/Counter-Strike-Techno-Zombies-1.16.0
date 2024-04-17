#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "zbs.h"
#include "zbs_level.h"

#include "string.h"

CHudZBSLevel::CHudZBSLevel(void)
{
	m_iLevel_HP = m_iLevel_ATK = m_iLevel_Wall = 0;
}

int CHudZBSLevel::VidInit(void)
{
	R_InitTexture( textbg, "resource/hud/zbs/textbg_right");
	//m_iFlags |= HUD_DRAW;

	m_iLevel_HP = m_iLevel_ATK = m_iLevel_Wall = 1;
	return 1;
}

int CHudZBSLevel::Draw(float time)
{
	int x = ScreenWidth - 2012;
	int y = ScreenHeight - 210;
	int x2 = ScreenWidth - 1950;
	int y2 = ScreenHeight - 210;
	const float flScale = 0.0f;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransAlpha);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	//textbg->Bind();
	//DrawUtils::Draw2DQuadScaled(x + 140, y + 140, x + 140 , y + 85);

	const int r = 255, g = 255, b = 255;

	//DrawUtils::DrawHudString(x + 20, y + 5, ScreenWidth, "HP", r, g, b, flScale);
	//DrawUtils::DrawHudString(x + 20, y + 50, ScreenWidth, "ATT", r, g, b, flScale);

	char szBuffer[64];

	// HP Level
	if (m_iLevel_HP >= 100)
		sprintf(szBuffer, "Lv. MAX");
	else
		sprintf(szBuffer, "Lv. %d", m_iLevel_HP);
	DrawUtils::DrawHudString(x + 100, y + 5, ScreenWidth, szBuffer, r, g, b, flScale);

	// Wall Level
	if (m_iLevel_Wall)
	{
		y -= 32;

		DrawUtils::DrawHudString(x + 20, y + 5, ScreenWidth, "Durability", r, g, b, flScale);
		if (m_iLevel_Wall >= 100)
			sprintf(szBuffer, "Lv. MAX");
		else
			sprintf(szBuffer, "Lv. %d", m_iLevel_Wall);
		DrawUtils::DrawHudString(x + 100, y + 5, ScreenWidth, szBuffer, r, g, b, flScale);
	}


	return 1;
}