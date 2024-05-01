#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>
#include "draw_util.h"

DECLARE_MESSAGE(m_Flash, FlashBat)
DECLARE_MESSAGE(m_Flash, Flashlight)

#define BAT_NAME "sprites/%d_Flashlight.spr"

int CHudFlashlight::Init(void)
{
	m_fFade = 0;
	m_fOn = 0;

	HOOK_MESSAGE(Flashlight);
	HOOK_MESSAGE(FlashBat);

	m_iFlags |= HUD_DRAW;

	gHUD.AddHudElem(this);

	return 1;
}

void CHudFlashlight::Reset(void)
{
	m_fFade = 0;
	m_fOn = 0;
}

int CHudFlashlight::VidInit(void)
{
	m_hSprite1.SetSpriteByName("flash_empty");
	m_hSprite2.SetSpriteByName("flash_full");
	m_hBeam.SetSpriteByName("flash_beam");
	m_iWidth = m_hSprite1.rect.right - m_hSprite1.rect.left;

	return 1;
}

int CHudFlashlight::MsgFunc_FlashBat(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	int x = reader.ReadByte();
	m_iBat = x;
	m_flBat = ((float)x) / 100.0;

	return 1;
}

int CHudFlashlight::MsgFunc_Flashlight(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);
	m_fOn = reader.ReadByte();
	int x = reader.ReadByte();
	m_iBat = x;
	m_flBat = ((float)x) / 100.0;

	return 1;
}

int CHudFlashlight::Draw(float flTime)
{
	int r, g, b, a;
	wrect_t rc;

	if (!(gHUD.m_iWeaponBits & (1 << (WEAPON_SUIT))))
		return 1;

	if (m_fOn)
		a = 225;
	else
		a = MIN_ALPHA;

	if (m_flBat < 0.20)
		DrawUtils::UnpackRGB(r, g, b, RGB_REDISH);
	else
		DrawUtils::UnpackRGB(r, g, b, RGB_WHITE);

	DrawUtils::ScaleColors(r, g, b, a);

	int iY = ScreenHeight - 79;	//y1
	int iX = ScreenWidth - 24 - m_iWidth; //x1

	// Draw the flashlight casing
	SPR_Set(m_hSprite1.spr, r, g, b);
	SPR_DrawAdditive(0, iX, iY, &m_hSprite1.rect);

	if (m_fOn)
	{  // draw the flashlight beam
		iX = ScreenWidth - 24;

		SPR_Set(m_hBeam.spr, r, g, b);
		SPR_DrawAdditive(0, iX, iY, &m_hBeam.rect);
	}

	// draw the flashlight energy level
	iX = ScreenWidth - 24 - m_iWidth;
	int iOffset = m_iWidth * (1.0 - m_flBat);
	if (iOffset < m_iWidth)
	{
		rc = m_hSprite2.rect;
		rc.left += iOffset;

		SPR_Set(m_hSprite2.spr, r, g, b);
		SPR_DrawAdditive(0, iX + iOffset, iY, &rc);
	}

	return 1;
}