#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include "vgui_parser.h"
#include "draw_util.h"

#include "followicon.h"
#include "draw_util.h"
#include "triangleapi.h"

//#include "zb3.h"
//#include "NewScoreboard.h"
#include "legacy/hud_scoreboard_legacy.h"
#include "gamemode/mods_const.h"
#include "eventscripts.h"

#include "string.h"
//#include "assert.h"
DECLARE_MESSAGE(m_Money, Money)
DECLARE_MESSAGE(m_Money, BlinkAcct)
UniqueTexture m_pTexture_Black;

int CHudMoney::Init( )
{
	HOOK_MESSAGE( Money );
	HOOK_MESSAGE( BlinkAcct );
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
	R_InitTexture(m_pTexture_Black, "resource/hud/csgo/blacka");

	return 1;
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

int CHudMoney::Draw(float flTime)
{
	int iDollarWidth = m_hDollar.rect.right - m_hDollar.rect.left;

	int x = ScreenWidth - iDollarWidth * 7;
	int y = ScreenHeight - 3 * gHUD.m_iFontHeight;

	x = 5;
	if (gHUD.m_bMordenRadar)
		y = 0.175 * ScreenWidth + (m_hDollar.rect.bottom - m_hDollar.rect.top) * 2;
	else
		y = 200;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransAlpha);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
	int idx = IS_FIRSTPERSON_SPEC ? g_iUser2 : gEngfuncs.GetLocalPlayer()->index;
	if (!g_PlayerExtraInfo[idx].dead)
	{	
       m_pTexture_Black->Bind();
	   DrawUtils::Draw2DQuadScaled(0, y - 0.5 * gHUD.m_iFontHeight, x + (gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left) * 10, y + 1.5 * gHUD.m_iFontHeight);
	}
	

	if(( gHUD.m_iHideHUDDisplay & ( HIDEHUD_HEALTH ) ))
		return 1;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT))))
		return 1;

	int r, g, b, alphaBalance;
	m_fFade -= gHUD.m_flTimeDelta;
	if( m_fFade < 0)
	{
		m_fFade = 0.0f;
		m_iDelta = 0;
	}
	float interpolate = ( 5 - m_fFade ) / 5;


	// Does weapon have seconday ammo?
	if (gHUD.m_Ammo.FHasSecondaryAmmo())
	{
		y -= gHUD.m_iFontHeight + gHUD.m_iFontHeight / 4;
	}

	if( m_iBlinkAmt )
	{
		m_fBlinkTime += gHUD.m_flTimeDelta;
		DrawUtils::UnpackRGB( r, g, b, m_fBlinkTime > 0.5f? RGB_REDISH : RGB_YELLOWISH );

		if( m_fBlinkTime > 1.0f )
		{
			m_fBlinkTime = 0.0f;
			--m_iBlinkAmt;
		}
	}
	else
	{
		if( m_iDelta != 0 )
		{
			int iDeltaR, iDeltaG, iDeltaB;
			int iDollarHeight = m_hDollar.rect.bottom - m_hDollar.rect.top;
			int iDeltaAlpha = 255 - interpolate * (255);

			DrawUtils::UnpackRGB  (iDeltaR, iDeltaG, iDeltaB, m_iDelta < 0 ? RGB_REDISH : RGB_GREENISH);
			DrawUtils::ScaleColors(iDeltaR, iDeltaG, iDeltaB, iDeltaAlpha);

			if( m_iDelta > 0 )
			{
				r = interpolate * ((RGB_YELLOWISH & 0xFF0000) >> 16);
				g = (RGB_GREENISH & 0xFF00) >> 8;
				b = (RGB_GREENISH & 0xFF);

				// draw delta
				SPR_Set(m_hPlus.spr, iDeltaR, iDeltaG, iDeltaB );
				SPR_DrawAdditive(0, x, y - iDollarHeight * 1.5, &m_hPlus.rect );
			}
			else if( m_iDelta < 0)
			{
				r = (RGB_REDISH & 0xFF0000) >> 16;
				g = ((RGB_REDISH & 0xFF00) >> 8) + interpolate * (((RGB_YELLOWISH & 0xFF00) >> 8) - ((RGB_REDISH & 0xFF00) >> 8));
				b = (RGB_REDISH & 0xFF) - interpolate * (RGB_REDISH & 0xFF);

				SPR_Set(m_hMinus.spr, iDeltaR, iDeltaG, iDeltaB );
				SPR_DrawAdditive(0, x, y - iDollarHeight * 1.5, &m_hMinus.rect );
			}

			DrawUtils::DrawHudNumber2( x + iDollarWidth, y - iDollarHeight * 1.5 , false, 5,
									   m_iDelta < 0 ? -m_iDelta : m_iDelta,
									   iDeltaR, iDeltaG, iDeltaB);
			FillRGBA(x + iDollarWidth / 4, y - iDollarHeight * 1.5 + gHUD.m_iFontHeight / static_cast<double>(4), 2, 2, iDeltaR, iDeltaG, iDeltaB, iDeltaAlpha);
		}
		else DrawUtils::UnpackRGB(r, g, b, RGB_YELLOWISH );
	}

	alphaBalance = 255 - interpolate * (255 - MIN_ALPHA);


	DrawUtils::ScaleColors( r, g, b, alphaBalance );

	SPR_Set(m_hDollar.spr, r, g, b);
	SPR_DrawAdditive(0, x, y, &m_hDollar.rect);

	DrawUtils::DrawHudNumber2( x + iDollarWidth, y, false, 5, m_iMoneyCount, r, g, b );
	FillRGBA(x + iDollarWidth / 4, y + gHUD.m_iFontHeight / 4, 2, 2, r, g, b, alphaBalance );
	return 1;
}

int CHudMoney::MsgFunc_Money(const char *pszName, int iSize, void *pbuf)
{
	BufferReader buf( pszName, pbuf, iSize );
	int iOldCount = m_iMoneyCount;
	m_iMoneyCount = buf.ReadLong();
	m_iDelta = m_iMoneyCount - iOldCount;
	m_fFade = 5.0f; //fade for 5 seconds
	m_iFlags |= HUD_DRAW;
	return 1;
}

int CHudMoney::MsgFunc_BlinkAcct(const char *pszName, int iSize, void *pbuf)
{
	BufferReader buf( pszName, pbuf, iSize );

	m_iBlinkAmt = buf.ReadByte();
	m_fBlinkTime = 0;
	return 1;
}
