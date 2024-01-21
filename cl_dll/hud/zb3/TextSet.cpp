/* =================================================================================== *
	  * =================== TechnoSoftware & Valve Developing =================== *
 * =================================================================================== */


#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "TextSet.h"

#include "string.h"
#include "assert.h"
#include <numeric>
#include <tuple>

CHudTextZB3 m_hudzb1;

int CHudTextZB3::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/hud_string_bg", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudTextZB3::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 2.0f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 2;
	int y = ScreenHeight / 1.5;

	int x2 = ScreenWidth / 2;
	int y2 = ScreenHeight / 1.5;
	const float flScale = 0.0f;
	const int r = 255, g = 255, b = 0;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 - std::min(5.0f - (time - m_flDisplayTime), 1.0f));
	m_pCurTexture->Bind();
	DrawUtils::Draw2DQuadScaled(x - 500 / 2, y, x + 500 / 2, y + 23);
	DrawUtils::DrawHudString(x2 - 170, y2 + 3, ScreenWidth, "Ќавык можно использовать только один раз за раунд", r, g, b, 0.0f);
	
	return 1;
}

void CHudTextZB3::Settext()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}