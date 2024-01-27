#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "triangleapi.h"
#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "gamemode/mods_const.h"
#include <string.h>
#include "vgui_parser.h"
#include "draw_util.h"
DECLARE_MESSAGE(m_Money, Money)
DECLARE_MESSAGE(m_Money, BlinkAcct)

inline void BuildNumberRC(wrect_t(&rgrc)[10], int w, int h)
{
	int nw = 0;

	for (int i = 0; i < 10; i++)
	{
		rgrc[i].left = nw;
		rgrc[i].top = 0;
		rgrc[i].right = rgrc[i].left + w;
		rgrc[i].bottom = h;

		nw += w;
	}
}

inline void BuildNumberRC(wrect_t(&rgrc)[10], int tex)
{
	int w = gRenderAPI.RenderGetParm(PARM_TEX_SRC_WIDTH, tex);
	int h = gRenderAPI.RenderGetParm(PARM_TEX_SRC_HEIGHT, tex);
	return BuildNumberRC(rgrc, w / 10, h);
}

inline void DrawTexturePart(const CTextureRef& tex, const wrect_t& rect, int x1, int y1, float scale = 1.0f)
{
	tex.Bind();

	float w = tex.w();
	float h = tex.h();

	x1 *= gHUD.m_flScale;
	y1 *= gHUD.m_flScale;
	scale *= gHUD.m_flScale;

	int x2 = x1 + (rect.right - rect.left) * scale;
	int y2 = y1 + (rect.bottom - rect.top) * scale;

	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	gEngfuncs.pTriAPI->TexCoord2f(rect.left / w, rect.top / h);
	gEngfuncs.pTriAPI->Vertex3f(x1, y1, 0);
	gEngfuncs.pTriAPI->TexCoord2f(rect.left / w, rect.bottom / h);
	gEngfuncs.pTriAPI->Vertex3f(x1, y2, 0);
	gEngfuncs.pTriAPI->TexCoord2f(rect.right / w, rect.bottom / h);
	gEngfuncs.pTriAPI->Vertex3f(x2, y2, 0);
	gEngfuncs.pTriAPI->TexCoord2f(rect.right / w, rect.top / h);
	gEngfuncs.pTriAPI->Vertex3f(x2, y1, 0);
	gEngfuncs.pTriAPI->End();
}

inline int DrawTexturedNumbersTopRightAligned(const CTextureRef& tex, const wrect_t(&rect)[10], int iNumber, int x, int y, float scale = 1.0f)
{
	assert(iNumber >= 0);

	do
	{
		int k = iNumber % 10;
		iNumber /= 10;
		DrawTexturePart(tex, rect[k], x, y, scale);
		x -= (rect[k].right - rect[k].left) * scale;
	} while (iNumber > 0);

	return x;
}

inline unsigned math_log10(unsigned v)
{
	return (v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7 :
		(v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4 :
		(v >= 1000) ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0;
}

inline int DrawTexturedNumbersTopCenterAligned(const CTextureRef& tex, const wrect_t(&rect)[10], int iNumber, int x, int y, float scale = 1.0f)
{
	int n = math_log10(iNumber);
	x += (rect[0].right - rect[0].left) * (n - 1) * scale * 0.5f;
	return DrawTexturedNumbersTopRightAligned(tex, rect, iNumber, x, y, scale);
}

int CHudMoney::Init()
{
	HOOK_MESSAGE(Money);
	HOOK_MESSAGE(BlinkAcct);
	gHUD.AddHudElem(this);
	m_fFade = 0;
	m_iFlags = 0;
	return 1;
}

int CHudMoney::VidInit()
{
	m_hDollar.SetSpriteByName("dollar");
	m_hMinus.SetSpriteByName("minus");
	m_hPlus.SetSpriteByName("plus");
	R_InitTexture(m_iDollarBG, "resource/hud/hud_dollar_bg");
	R_InitTexture(m_iDollar, "resource/hud/zsh_icon_dollar");
	R_InitTexture(money_count, "resource/hud/hud_sb_num_big_white");
	BuildNumberRC(rc_money_count, 18, 22);
	

	return 1;
}

int CHudMoney::Draw(float flTime)
{
	if ((gHUD.m_iHideHUDDisplay & (HIDEHUD_HEALTH)))
		return 1;

	if (!(gHUD.m_iWeaponBits & (1 << (WEAPON_SUIT))))
		return 1;

	int r, g, b, alphaBalance;

	m_fFade -= gHUD.m_flTimeDelta;
	if (m_fFade < 0)
	{
		m_fFade = 0.0f;
		m_iDelta = 0;
	}

	float interpolate = (5 - m_fFade) / 5;

	int iDollarWidth = m_hDollar.rect.right - m_hDollar.rect.left;

	int x = ScreenWidth - iDollarWidth * 7;
	int y = ScreenHeight - 3 * gHUD.m_iFontHeight;

	int x2 = ScreenWidth / 20;
	int y2 = 240;

	int x3 = ScreenWidth / 31;
	int y3 = 250;

	int x4 = ScreenWidth / 2;
	int y4 = ScreenHeight / 1.05;

	int x5 = ScreenWidth / 20;
	int y5 = 240;

	int x6 = ScreenWidth / 90;
	int y6 = 190;

	int x7 = ScreenWidth / 29;
	int y7 = 245;

	int x8 = ScreenWidth / 90;
	int y8 = 190;

	x = 5;

	if (gHUD.m_bMordenRadar)
		y = 0.175 * ScreenWidth + (m_hDollar.rect.bottom - m_hDollar.rect.top) * 2;
	else
		y = 200;

	if (gHUD.m_Ammo.FHasSecondaryAmmo())
	{
		y -= gHUD.m_iFontHeight + gHUD.m_iFontHeight / 4;
	}
	
	if (m_iBlinkAmt)
	{
		m_fBlinkTime += gHUD.m_flTimeDelta;
		DrawUtils::UnpackRGB(r, g, b, m_fBlinkTime > 0.5f ? RGB_REDISH : RGB_WHITE );

		if (m_fBlinkTime > 1.0f)
		{
			m_fBlinkTime = 0.0f;
			--m_iBlinkAmt;
		}
	}
	else
	{
		if (m_iDelta != 0)
		{
			int iDeltaR, iDeltaG, iDeltaB;
			int iDollarHeight = m_hDollar.rect.right - m_hDollar.rect.left;
			int iDeltaAlpha = 255 - interpolate * (255);

			DrawUtils::UnpackRGB(iDeltaR, iDeltaG, iDeltaB, m_iDelta < 0 ? RGB_REDISH : RGB_GREENISH);
			DrawUtils::ScaleColors(iDeltaR, iDeltaG, iDeltaB, iDeltaAlpha);

			if (m_iDelta > 0)//add money
			{
				r = interpolate * ((RGB_WHITE & 0xFF0000) >> 16);
				g = (RGB_GREENISH & 0xFF00) >> 8;
				b = (RGB_GREENISH & 0xFF);

				// draw delta
				SPR_Set(m_hPlus.spr, iDeltaR, iDeltaG, iDeltaB);
				SPR_DrawAdditive(0, x8, y8 + iDollarHeight * 1.5, &m_hPlus.rect);
			}
			else if (m_iDelta < 0)//buy enough money
			{
				r = (RGB_REDISH & 0xFF0000) >> 16;
				g = ((RGB_REDISH & 0xFF00) >> 8) + interpolate * (((RGB_YELLOWISH & 0xFF00) >> 8) - ((RGB_REDISH & 0xFF00) >> 8));
				b = (RGB_REDISH & 0xFF) - interpolate * (RGB_REDISH & 0xFF);

				SPR_Set(m_hMinus.spr, iDeltaR, iDeltaG, iDeltaB);
				SPR_DrawAdditive(0, x8, y8 + iDollarHeight * 1.5, &m_hMinus.rect);
			}   
			
			if (m_iDelta < 0)
				DrawUtils::DrawHudNumber2(x6, y6 + iDollarHeight * 1.5, false, 5, m_iDelta < 0 ? -m_iDelta : m_iDelta, iDeltaR, iDeltaG, iDeltaB);

			else if (m_iDelta > 0)
				DrawUtils::DrawHudNumber2(x6, y6 + iDollarHeight * 1.5, false, 5, m_iDelta < 0 ? -m_iDelta : m_iDelta, iDeltaR, iDeltaG, iDeltaB);
		
			FillRGBA(x + iDollarWidth / 4, y - iDollarHeight * 1.5 + gHUD.m_iFontHeight / static_cast<double>(4), 2, 2, iDeltaR, iDeltaG, iDeltaB, iDeltaAlpha);
		}
		else DrawUtils::UnpackRGB(r, g, b,  RGB_WHITE );
	}

	alphaBalance = 255 - interpolate * (255 - MIN_ALPHA);

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
	m_iDollarBG->Bind();
	DrawUtils::Draw2DQuadScaled(x2 - 110, y2, x2 + 153, y2 + 38);
	gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
	DrawTexturedNumbersTopRightAligned(*money_count, rc_money_count, m_iMoneyCount, x3 + 77, y3, 0.80f);
	alphaBalance = 255;
	DrawUtils::ScaleColors(r, g, b, alphaBalance);

	m_iDollar->Bind();
	DrawUtils::Draw2DQuadScaled(x7 - 15, y7, x7 + 15, y7 + 28);

	//SPR_Set(m_hDollar.spr, 255, 255, 255);
	//SPR_DrawAdditive(0, x7, y7 + iDollarWidth * 1.5, &m_hDollar.rect);
	
	return 1;
}

int CHudMoney::MsgFunc_Money(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	int iOldCount = m_iMoneyCount;
	m_iMoneyCount = buf.ReadLong();
	m_iDelta = m_iMoneyCount - iOldCount;
	m_fFade = 5.0f; //fade for 5 seconds
	m_iFlags |= HUD_DRAW;
	return 1;
}

int CHudMoney::MsgFunc_BlinkAcct(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);

	m_iBlinkAmt = buf.ReadByte();
	m_fBlinkTime = 0;
	return 1;
}
