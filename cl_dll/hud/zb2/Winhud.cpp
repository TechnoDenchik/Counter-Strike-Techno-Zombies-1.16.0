#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

//#include "zb1/mod_zb1.h"
#include "Winhud.h"
#include "string.h"
#include "assert.h"
#include <numeric>
#include <tuple>

CHudWinhudZB1 m_hudzb1;

int CHudWinhudZB1::VidInit(void)
{
	if (!iconwinhm)
		iconwinhm = R_LoadTextureShared("resource/hud/zb3/humanwin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	if (!iconwinzb)
		iconwinzb = R_LoadTextureShared("resource/hud/zb3/zombiewin", TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP);
	return 1;
}

int CHudWinhudZB1::Draw(float time)
{
	if (!m_pCurTexture)
		return 1;

	if (time > m_flDisplayTime + 5.0f)
	{
		m_pCurTexture = nullptr;
		return 1;
	}

	int x = ScreenWidth / 2;
	int y = ScreenHeight / 4;
	const float flScale = 0.0f;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255 * std::min(5.0f - (time - m_flDisplayTime), 1.0f));
	m_pCurTexture->Bind();
	DrawUtils::Draw2DQuadScaled(x - 373 / 2, y, x + 373 / 2, y + 51);
	return 1;
}

void CHudWinhudZB1::WinHuman()
{
	m_pCurTexture = iconwinhm;
	m_flDisplayTime = gHUD.m_flTime;
	//gEngfuncs.pfnClientCmd("speak \"win_human\"\n");
}

void CHudWinhudZB1::WinZombie()
{
	m_pCurTexture = iconwinzb;
	m_flDisplayTime = gHUD.m_flTime;
	//gEngfuncs.pfnClientCmd("speak \"win_zombi\"\n");
}
