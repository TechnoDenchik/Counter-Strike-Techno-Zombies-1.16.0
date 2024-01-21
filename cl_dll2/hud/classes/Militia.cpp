#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "Militia.h"

#include "string.h"
#include "assert.h"
#include <numeric>
#include <tuple>

CHudMilitia m_hudzb1;

int CHudMilitia::VidInit(void)
{
	if (!stringtext)
		stringtext = R_LoadTextureShared("resource/hud/zb3/militia", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudMilitia::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 2.0f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 2;
	int y = ScreenHeight / 4;
	const float flScale = 0.0f;
	const int r = 255, g = 255, b = 255;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
	m_pCurTexture->Bind();

	DrawUtils::Draw2DQuadScaled(x - 500 / 2, y, x + 500 / 2, y + 23);
	//DrawUtils::DrawHudString(x + 20, y + 5, ScreenWidth, "Время до появления зомби: %time Sec" , r, g, b, flScale);

	return 1;
}

void CHudMilitia::Setclass()
{
	m_pCurTexture = stringtext;
	m_flDisplayTime = gHUD.m_flTime;
}