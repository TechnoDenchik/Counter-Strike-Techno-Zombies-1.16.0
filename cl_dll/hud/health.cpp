/* =================================================================================== *
	  * =================== TechnoSoftware & Valve Developing =================== *
 * =================================================================================== */

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include "eventscripts.h"
#include "triangleapi.h"
#include "gamemode/mods_const.h"
#include "draw_util.h"

DECLARE_MESSAGE(m_Health, Health )
DECLARE_MESSAGE(m_Health, Damage )
DECLARE_MESSAGE(m_Health, ScoreAttrib )
DECLARE_MESSAGE(m_Health, ClCorpse )
DECLARE_MESSAGE(m_Health, Battery)
DECLARE_MESSAGE(m_Health, ArmorType)

#define PAIN_NAME "sprites/%d_pain.spr"
#define DAMAGE_NAME "sprites/%d_dmg.spr"
#define EPSILON 0.4f

int giDmgHeight, giDmgWidth;

float g_LocationColor[3];

int giDmgFlags[NUM_DMG_TYPES] = 
{
	DMG_POISON,
	DMG_ACID,
	DMG_FREEZE|DMG_SLOWFREEZE,
	DMG_DROWN,
	DMG_BURN|DMG_SLOWBURN,
	DMG_NERVEGAS,
	DMG_RADIATION,
	DMG_SHOCK,
	DMG_CALTROP,
	DMG_TRANQ,
	DMG_CONCUSS,
	DMG_HALLUC
};

enum
{
	ATK_FRONT = 0,
	ATK_RIGHT,
	ATK_REAR,
	ATK_LEFT
};

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

int CHudHealth::Init(void)
{
	HOOK_MESSAGE(Health);
	HOOK_MESSAGE(Damage);
	HOOK_MESSAGE(ScoreAttrib);
	HOOK_MESSAGE(ClCorpse);

	m_iBat = 0;
	m_fFade = 0;
	m_iFlags = 0;
	m_enArmorType = Vest;

	HOOK_MESSAGE(Battery);
	HOOK_MESSAGE(ArmorType);

	m_iHealth = 100;
	m_fFade = 0;
	m_iFlags = 0;
	m_bitsDamage = 0;
	giDmgHeight = 0;
	giDmgWidth = 0;

	for( int i = 0; i < 4; i++ )
		m_fAttack[i] = 0;

	memset(m_dmg, 0, sizeof(DAMAGE_IMAGE) * NUM_DMG_TYPES);

	CVAR_CREATE("cl_corpsestay", "600", FCVAR_ARCHIVE);
	gHUD.AddHudElem(this);
	return 1;
}


void CHudHealth::InitHUDData(void)
{
	m_enArmorType = Vest;
}

void CHudHealth::Reset( void )
{
	// make sure the pain compass is cleared when the player respawns
	for( int i = 0; i < 4; i++ )
		m_fAttack[i] = 0;


	// force all the flashing damage icons to expire
	m_bitsDamage = 0;
	for ( int i = 0; i < NUM_DMG_TYPES; i++ )
	{
		m_dmg[i].fExpire = 0;
	}
}

int CHudHealth::VidInit(void)
{
	m_hSprite = LoadSprite(PAIN_NAME);

	m_vAttackPos[ATK_FRONT].x = ScreenWidth  / 2 - SPR_Width ( m_hSprite, 0 ) / 2;
	m_vAttackPos[ATK_FRONT].y = ScreenHeight / 2 - SPR_Height( m_hSprite, 0 ) * 3;

	m_vAttackPos[ATK_RIGHT].x = ScreenWidth  / 2 + SPR_Width ( m_hSprite, 1 ) * 2;
	m_vAttackPos[ATK_RIGHT].y = ScreenHeight / 2 - SPR_Height( m_hSprite, 1 ) / 2;

	m_vAttackPos[ATK_REAR ].x = ScreenWidth  / 2 - SPR_Width ( m_hSprite, 2 ) / 2;
	m_vAttackPos[ATK_REAR ].y = ScreenHeight / 2 + SPR_Height( m_hSprite, 2 ) * 2;

	m_vAttackPos[ATK_LEFT ].x = ScreenWidth  / 2 - SPR_Width ( m_hSprite, 3 ) * 3;
	m_vAttackPos[ATK_LEFT ].y = ScreenHeight / 2 - SPR_Height( m_hSprite, 3 ) / 2;


	m_HUD_dmg_bio = gHUD.GetSpriteIndex( "dmg_bio" ) + 1;
	m_HUD_cross = gHUD.GetSpriteIndex( "cross" );

	R_InitTexture(m_health_board, "resource/hud/hud_character_bg_new_bottom");
	R_InitTexture(m_ihealthes_top, "resource/hud/hud_character_bg_new_top");
	R_InitTexture(m_ihealthes, "resource/hud/zb3/hud_sb_num_big_white");
	R_InitTexture(m_plus, "resource/hud/hud_sb_num_plus");
	R_InitTexture(m_armors, "resource/hud/hud_sb_num_defense");

	BuildNumberRC(ihealth, 18, 22);
	BuildNumberRC(iarmors, 18, 22);

	giDmgHeight = gHUD.GetSpriteRect(m_HUD_dmg_bio).right - gHUD.GetSpriteRect(m_HUD_dmg_bio).left;
	giDmgWidth = gHUD.GetSpriteRect(m_HUD_dmg_bio).bottom - gHUD.GetSpriteRect(m_HUD_dmg_bio).top;

	m_hEmpty[Vest].SetSpriteByName("suit_empty");
	m_hFull[Vest].SetSpriteByName("suit_full");
	m_hEmpty[VestHelm].SetSpriteByName("suithelmet_empty");
	m_hFull[VestHelm].SetSpriteByName("suithelmet_full");

	m_iHeight = m_hFull[Vest].rect.bottom - m_hEmpty[Vest].rect.top;
	m_fFade = 0;

	return 1;
}

int CHudHealth:: MsgFunc_Health(const char *pszName,  int iSize, void *pbuf )
{
	BufferReader reader( pszName, pbuf, iSize );
	int x = m_iHealth;
	if (iSize == 2)
	{
		x = reader.ReadShort();
	}
	else
	{
		x = reader.ReadByte();
	}

	m_iFlags |= HUD_DRAW;

	if (x != m_iHealth)
	{
		m_fFade = FADE_TIME;
		m_iHealth = x;
	}

	return 1;
}

int CHudHealth:: MsgFunc_Damage(const char *pszName,  int iSize, void *pbuf )
{
	BufferReader reader( pszName, pbuf, iSize );

	int armor = reader.ReadByte();	// armor
	int damageTaken = reader.ReadByte();	// health
	long bitsDamage = reader.ReadLong(); // damage bits

	vec3_t vecFrom;

	for ( int i = 0 ; i < 3 ; i++)
		vecFrom[i] = reader.ReadCoord();

	UpdateTiles(gHUD.m_flTime, bitsDamage);

	// Actually took damage?
	if ( damageTaken > 0 || armor > 0 )
	{
		CalcDamageDirection(vecFrom);
		if( g_iXash )
		{
			float time = damageTaken * 4.0f + armor * 2.0f;

			if( time > 200.0f ) time = 200.0f;
			gMobileAPI.pfnVibrate( time, 0 );
		}
	}
	return 1;
}

int CHudHealth:: MsgFunc_ScoreAttrib(const char *pszName,  int iSize, void *pbuf )
{
	BufferReader reader( pszName, pbuf, iSize );

	int index = reader.ReadByte();
	unsigned char flags = reader.ReadByte();

	g_PlayerExtraInfo[index].dead   = !!(flags & PLAYER_DEAD);
	g_PlayerExtraInfo[index].has_c4 = !!(flags & PLAYER_HAS_C4);
	g_PlayerExtraInfo[index].vip    = !!(flags & PLAYER_VIP);
	g_PlayerExtraInfo[index].zombie = !!(flags & PLAYER_ZOMBIE);
	return 1;
}

int CHudHealth::Draw(float flTime)
{
	if( !(gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH ) && !gEngfuncs.IsSpectateOnly() )
	{
		int r, g, b, r2, g2, b2;
		int a = 0;
		int a2 = 0;

		int x2 = ScreenWidth / 75;
		int y2 = ScreenHeight / 1.0215;

		int x3 = ScreenWidth / 9.0;
		int y3 = ScreenHeight / 1.0215;

		int x8 = ScreenWidth / 10.4;
		int y8 = ScreenHeight / 1.1014;

		int x9 = ScreenWidth / 50.5;
		int y9 = ScreenHeight / 1.0215;

		int x10 = ScreenWidth / 98.0;
		int y10 = ScreenHeight / 1.0485;

		int x11 = ScreenWidth / 9.0;
		int y11 = ScreenHeight / 1.0485;

		int x12 = ScreenWidth / 9.8;
		int y12 = ScreenHeight / 1.0485;

		int x13 = ScreenWidth / 215.5;
		int y13 = ScreenHeight / 1.0485;

		int x14 = ScreenWidth / 10.0;
		int y14 = ScreenHeight / 1.0485;

		if (m_iHealth > 25)
		{
			a = 255;
		}

		if (m_iHealth <= 15)
		{
			a = 255;
		}

		if (m_iHealth > 25)
		{
			DrawUtils::UnpackRGB(r, g, b, RGB_WHITE);
		}
		else
		{
			r = 250;
			g = 0;
			b = 0;
		}

		if (m_iBat > 25)
		{
			a2 = 255;
		}

		if (m_iBat <= 15)
		{
			a2 = 255;
		}

		if (m_iBat > 25)
		{
			DrawUtils::UnpackRGB(r2, g2, b2, RGB_WHITE);
		}
		else
		{
			r2 = 250;
			g2 = 0;
			b2 = 0;
		}

		
		if (gHUD.m_iWeaponBits & (1 << (WEAPON_SUIT)))
		{
			gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
			gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
			
			wrect_t rc;

			rc = m_hEmpty[m_enArmorType].rect;
			rc.top += m_iHeight * ((float)(100 - (min(100, m_iBat))) * 0.01f);

			if (m_fFade)
			{
				if (m_fFade > FADE_TIME)
					m_fFade = FADE_TIME;
				m_fFade -= (gHUD.m_flTimeDelta * 20);

			if (m_fFade <= 0)
			{
				a = 128;
				m_fFade = 0;
			}
			a = MIN_ALPHA + (m_fFade / FADE_TIME) * 128;

			}
			else
			{
				a = MIN_ALPHA;
			}

			switch (gHUD.m_iModRunning)
			{
			case MOD_ZB1:
			case MOD_ZB3:

				rc = m_hEmpty[m_enArmorType].rect;
				rc.top += m_iHeight * ((float)(100 - (min(100, m_iBat))) * 0.01f);

				m_health_board->Bind();
				DrawUtils::Draw2DQuadScaled(x8 - 550 / 3.0, y8 + 5.5, x8 + 450 / 3.0, y8 + 95);

				gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
				m_plus->Bind();
				DrawUtils::Draw2DQuadScaled(x2 - 8, y2 - 8, x2 + 8, y2 + 8);
				gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
				m_armors->Bind();
				DrawUtils::Draw2DQuadScaled(x3 - 8, y3 - 8, x3 + 8, y3 + 8);

				m_ihealthes_top->Bind();
				DrawUtils::Draw2DQuadScaled(x8 - 550 / 3.0, y8 + 5.5, x8 + 450 / 3.0, y8 + 95);

				if (m_fFade)
				{
					if (m_fFade > FADE_TIME)
						m_fFade = FADE_TIME;
					m_fFade -= (gHUD.m_flTimeDelta * 20);

					if (m_fFade <= 0)
					{
						a = 128;
						m_fFade = 0;
					}
					a = MIN_ALPHA + (m_fFade / FADE_TIME) * 128;

				}
				else
				{
					a = MIN_ALPHA;
				}

				if (m_iHealth < 10)
				{
					gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, ihealth, m_iHealth, x13 + 42, y13 + 14, 1);
				}
				else if (m_iHealth < 100)
				{
					gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, ihealth, m_iHealth, x13 + 46, y13 + 14, 1);
				}
				else if (m_iHealth < 1000)
				{
					gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, ihealth, m_iHealth, x13 + 64, y13 + 14, 1);
				}
				else if (m_iHealth < 10000)
				{
					gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, ihealth, m_iHealth, x13 + 86, y13 + 14, 1);
				}
				else
				{
					gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, ihealth, m_iHealth, x13 + 106, y13 + 14, 1);
				}

				if (m_iBat < 10)
				{
					gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
					(m_hEmpty[m_enArmorType].rect.right - m_hEmpty[m_enArmorType].rect.left);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, iarmors, m_iBat, x14 + 46, y14 + 14, 1);
				}
				else if (m_iBat < 100)
				{
					gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
					(m_hEmpty[m_enArmorType].rect.right - m_hEmpty[m_enArmorType].rect.left);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, iarmors, m_iBat, x14 + 54, y14 + 14, 1);
				}
				else if (m_iBat < 1000)
				{
					gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
					(m_hEmpty[m_enArmorType].rect.right - m_hEmpty[m_enArmorType].rect.left);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, iarmors, m_iBat, x14 + 70, y14 + 14, 1);
				}
				else if (m_iBat < 10000)
				{
					gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
					(m_hEmpty[m_enArmorType].rect.right - m_hEmpty[m_enArmorType].rect.left);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, iarmors, m_iBat, x14 + 86, y14 + 14, 1);
				}
				else
				{
					gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
					(m_hEmpty[m_enArmorType].rect.right - m_hEmpty[m_enArmorType].rect.left);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, iarmors, m_iBat, x14 + 106, y14 + 14, 1);
				}
				break;
			
			case MOD_ZBS:

				rc = m_hEmpty[m_enArmorType].rect;
				rc.top += m_iHeight * ((float)(100 - (min(100, m_iBat))) * 0.01f);

				m_health_board->Bind();
				DrawUtils::Draw2DQuadScaled(x8 - 550 / 3.0, y8 + 5.5, x8 + 450 / 3.0, y8 + 95);

				gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
				m_plus->Bind();
				DrawUtils::Draw2DQuadScaled(x2 - 8, y2 - 8, x2 + 8, y2 + 8);
				gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
				m_armors->Bind();
				DrawUtils::Draw2DQuadScaled(x3 - 8, y3 - 8, x3 + 8, y3 + 8);

				//m_ihealthes_top->Bind();
				//DrawUtils::Draw2DQuadScaled(x8 - 550 / 3.0, y8 + 5.5, x8 + 450 / 3.0, y8 + 95);

				if (m_fFade)
				{
					if (m_fFade > FADE_TIME)
						m_fFade = FADE_TIME;
					m_fFade -= (gHUD.m_flTimeDelta * 20);

					if (m_fFade <= 0)
					{
						a = 128;
						m_fFade = 0;
					}
					a = MIN_ALPHA + (m_fFade / FADE_TIME) * 128;

				}
				else
				{
					a = MIN_ALPHA;
				}

				if (m_iHealth < 10)
				{
					gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, ihealth, m_iHealth, x13 + 42, y13 + 14, 1);
				}
				else if (m_iHealth < 100)
				{
					gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, ihealth, m_iHealth, x13 + 46, y13 + 14, 1);
				}
				else if (m_iHealth < 1000)
				{
					gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, ihealth, m_iHealth, x13 + 64, y13 + 14, 1);
				}
				else if (m_iHealth < 10000)
				{
					gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, ihealth, m_iHealth, x13 + 86, y13 + 14, 1);
				}
				else
				{
					gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, ihealth, m_iHealth, x13 + 106, y13 + 14, 1);
				}

				if (m_iBat < 10)
				{
					gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
					(m_hEmpty[m_enArmorType].rect.right - m_hEmpty[m_enArmorType].rect.left);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, iarmors, m_iBat, x14 + 46, y14 + 14, 1);
				}
				else if (m_iBat < 100)
				{
					gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
					(m_hEmpty[m_enArmorType].rect.right - m_hEmpty[m_enArmorType].rect.left);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, iarmors, m_iBat, x14 + 54, y14 + 14, 1);
				}
				else if (m_iBat < 1000)
				{
					gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
					(m_hEmpty[m_enArmorType].rect.right - m_hEmpty[m_enArmorType].rect.left);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, iarmors, m_iBat, x14 + 70, y14 + 14, 1);
				}
				else if (m_iBat < 10000)
				{
					gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
					(m_hEmpty[m_enArmorType].rect.right - m_hEmpty[m_enArmorType].rect.left);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, iarmors, m_iBat, x14 + 86, y14 + 14, 1);
				}
				else
				{
					gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
					(m_hEmpty[m_enArmorType].rect.right - m_hEmpty[m_enArmorType].rect.left);
					DrawTexturedNumbersTopRightAligned(*m_ihealthes, iarmors, m_iBat, x14 + 106, y14 + 14, 1);
				}

				break;
				
				default:

					rc = m_hEmpty[m_enArmorType].rect;
					rc.top += m_iHeight * ((float)(100 - (min(100, m_iBat))) * 0.01f);

					m_health_board->Bind();
					DrawUtils::Draw2DQuadScaled(x8 - 550 / 3.0, y8 + 5.5, x8 + 450 / 3.0, y8 + 95);
					
					gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
					m_plus->Bind();
					DrawUtils::Draw2DQuadScaled(x2 - 8, y2 - 8, x2 + 8, y2 + 8);
					gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
					m_armors->Bind();
					DrawUtils::Draw2DQuadScaled(x3 - 8, y3 - 8, x3 + 8, y3 + 8);

					if (m_fFade)
					{
						if (m_fFade > FADE_TIME)
							m_fFade = FADE_TIME;
						m_fFade -= (gHUD.m_flTimeDelta * 20);

						if (m_fFade <= 0)
						{
							a = 128;
							m_fFade = 0;
						}
						a = MIN_ALPHA + (m_fFade / FADE_TIME) * 128;

					}
					else
					{
						a = MIN_ALPHA;
					}

					if (m_iHealth < 10)
					{
						gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
						DrawTexturedNumbersTopRightAligned(*m_ihealthes, ihealth, m_iHealth, x13 + 42, y13 + 14, 1);
					}
					else if(m_iHealth < 100)
					{
						gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
						DrawTexturedNumbersTopRightAligned(*m_ihealthes, ihealth, m_iHealth, x13 + 46, y13 + 14, 1);
					}
					else if (m_iHealth < 1000)
					{
						gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
						DrawTexturedNumbersTopRightAligned(*m_ihealthes, ihealth, m_iHealth, x13 + 64, y13 + 14, 1);
					}
					else if(m_iHealth < 10000)
					{
						gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
						DrawTexturedNumbersTopRightAligned(*m_ihealthes, ihealth, m_iHealth, x13 + 86, y13 + 14, 1);
					}
					else
					{
						gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
						DrawTexturedNumbersTopRightAligned(*m_ihealthes, ihealth, m_iHealth, x13 + 106, y13 + 14, 1);
					}

					if (m_iBat < 10)
					{
						gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
						(m_hEmpty[m_enArmorType].rect.right - m_hEmpty[m_enArmorType].rect.left);
						DrawTexturedNumbersTopRightAligned(*m_ihealthes, iarmors, m_iBat, x14 + 46, y14 + 14, 1); 
					}
					else if (m_iBat < 100)
					{
						gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
						(m_hEmpty[m_enArmorType].rect.right - m_hEmpty[m_enArmorType].rect.left);
						DrawTexturedNumbersTopRightAligned(*m_ihealthes, iarmors, m_iBat, x14 + 54, y14 + 14, 1);
					}
					else if (m_iBat < 1000)
					{
						gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
						(m_hEmpty[m_enArmorType].rect.right - m_hEmpty[m_enArmorType].rect.left);
						DrawTexturedNumbersTopRightAligned(*m_ihealthes, iarmors, m_iBat, x14 + 70, y14 + 14, 1);
					}
					else if (m_iBat < 10000)
					{
						gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
						(m_hEmpty[m_enArmorType].rect.right - m_hEmpty[m_enArmorType].rect.left);
						DrawTexturedNumbersTopRightAligned(*m_ihealthes, iarmors, m_iBat, x14 + 86, y14 + 14, 1);
					}
					else
					{
						gEngfuncs.pTriAPI->Color4ub(r2, g2, b2, 255);
						(m_hEmpty[m_enArmorType].rect.right - m_hEmpty[m_enArmorType].rect.left);
						DrawTexturedNumbersTopRightAligned(*m_ihealthes, iarmors, m_iBat, x14 + 106, y14 + 14, 1);
					}
					




					
					break;

				}
		}
		DrawDamage( flTime );
		DrawPain( flTime );
	}

	return 1;
}

int CHudHealth::MsgFunc_Battery(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);

	m_iFlags |= HUD_DRAW;
	int x = reader.ReadShort();

	if (x != m_iBat)
	{
		m_fFade = FADE_TIME;
		m_iBat = x;
	}

	return 1;
}

int CHudHealth::MsgFunc_ArmorType(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader(pszName, pbuf, iSize);

	m_enArmorType = (armortype_t)reader.ReadByte();

	return 1;
}

void CHudHealth::CalcDamageDirection( Vector vecFrom )
{
	Vector	forward, right, up;
	float	side, front, flDistToTarget;

	if( vecFrom.IsNull() )
	{
		for( int i = 0; i < 4; i++ )
			m_fAttack[i] = 0;
		return;
	}

	vecFrom = vecFrom - gHUD.m_vecOrigin;
	flDistToTarget = vecFrom.Length();
	vecFrom = vecFrom.Normalize();
	AngleVectors (gHUD.m_vecAngles, forward, right, up);

	front = DotProduct (vecFrom, right);
	side = DotProduct (vecFrom, forward);

	if (flDistToTarget <= 50)
	{
		for( int i = 0; i < 4; i++ )
			m_fAttack[i] = 1;
	}
	else
	{
		if (side > EPSILON)
			m_fAttack[0] = max(m_fAttack[0], side);
		if (side < -EPSILON)
			m_fAttack[1] = max(m_fAttack[1], 0 - side );
		if (front > EPSILON)
			m_fAttack[2] = max(m_fAttack[2], front);
		if (front < -EPSILON)
			m_fAttack[3] = max(m_fAttack[3], 0 - front );
	}
}

void CHudHealth::DrawPain(float flTime)
{
	if (m_fAttack[0] == 0 &&
		m_fAttack[1] == 0 &&
		m_fAttack[2] == 0 &&
		m_fAttack[3] == 0)
		return;

	float a, fFade = gHUD.m_flTimeDelta * 2;

	for( int i = 0; i < 4; i++ )
	{
		if( m_fAttack[i] > EPSILON )
		{
			a = max( m_fAttack[i], 0.5f );

			SPR_Set( m_hSprite, 255 * a, 255 * a, 255 * a);
			SPR_DrawAdditive( i, m_vAttackPos[i].x, m_vAttackPos[i].y, NULL );
			m_fAttack[i] = max( 0.0f, m_fAttack[i] - fFade );
		}
		else
			m_fAttack[i] = 0;
	}
}

void CHudHealth::DrawDamage(float flTime)
{
	int r, g, b, a;

	if (!m_bitsDamage)
		return;

	DrawUtils::UnpackRGB(r,g,b, RGB_WHITE);
	
	a = (int)( fabs(sin(flTime*2)) * 256.0);

	DrawUtils::ScaleColors(r, g, b, a);
	int i;

	for (i = 0; i < NUM_DMG_TYPES; i++)
	{
		if (m_bitsDamage & giDmgFlags[i])
		{
			DAMAGE_IMAGE *pdmg = &m_dmg[i];
			SPR_Set(gHUD.GetSprite(m_HUD_dmg_bio + i), r, g, b );
			SPR_DrawAdditive(0, pdmg->x, pdmg->y, &gHUD.GetSpriteRect(m_HUD_dmg_bio + i));
		}
	}

	for ( i = 0; i < NUM_DMG_TYPES; i++ )
	{
		DAMAGE_IMAGE *pdmg = &m_dmg[i];

		if ( m_bitsDamage & giDmgFlags[i] )
		{
			pdmg->fExpire = min( flTime + DMG_IMAGE_LIFE, pdmg->fExpire );

			if ( pdmg->fExpire <= flTime		// when the time has expired
				 && a < 40 )						// and the flash is at the low point of the cycle
			{
				pdmg->fExpire = 0;

				int y = pdmg->y;
				pdmg->x = pdmg->y = 0;

				// move everyone above down
				for (int j = 0; j < NUM_DMG_TYPES; j++)
				{
					pdmg = &m_dmg[j];
					if ((pdmg->y) && (pdmg->y < y))
						pdmg->y += giDmgHeight;

				}

				m_bitsDamage &= ~giDmgFlags[i];  // clear the bits
			}
		}
	}
}

void CHudHealth::UpdateTiles(float flTime, long bitsDamage)
{	
	DAMAGE_IMAGE *pdmg;

	long bitsOn = ~m_bitsDamage & bitsDamage;
	
	for (int i = 0; i < NUM_DMG_TYPES; i++)
	{
		pdmg = &m_dmg[i];

		// Is this one already on?
		if (m_bitsDamage & giDmgFlags[i])
		{
			pdmg->fExpire = flTime + DMG_IMAGE_LIFE; // extend the duration
			if (!pdmg->fBaseline)
				pdmg->fBaseline = flTime;
		}

		// Are we just turning it on?
		if (bitsOn & giDmgFlags[i])
		{
			// put this one at the bottom
			pdmg->x = giDmgWidth/8;
			pdmg->y = ScreenHeight - giDmgHeight * 2;
			pdmg->fExpire=flTime + DMG_IMAGE_LIFE;
			
			// move everyone else up
			for (int j = 0; j < NUM_DMG_TYPES; j++)
			{
				if (j == i)
					continue;

				pdmg = &m_dmg[j];
				if (pdmg->y)
					pdmg->y -= giDmgHeight;

			}
			pdmg = &m_dmg[i];
		}
	}

	m_bitsDamage |= bitsDamage;
}

int CHudHealth :: MsgFunc_ClCorpse(const char *pszName, int iSize, void *pbuf)
{
#if 0
	BufferReader reader(pbuf, iSize);

	char szModel[64];

	char *pModel = reader.ReadString();
	Vector origin;
	origin.x = reader.ReadLong() / 128.0f;
	origin.y = reader.ReadLong() / 128.0f;
	origin.z = reader.ReadLong() / 128.0f;
	Vector angles;
	angles.x = reader.ReadCoord();
	angles.y = reader.ReadCoord();
	angles.z = reader.ReadCoord();
	float delay = reader.ReadLong() / 100.0f;
	int sequence = reader.ReadByte();
	int classID = reader.ReadByte();
	int teamID = reader.ReadByte();
	int playerID = reader.ReadByte();

	if( !cl_minmodels->value )
	{
		if( !strstr(pModel, "models/") )
		{
			snprintf(szModel, sizeof(szModel), "models/player/%s/%s.mdl", pModel, pModel );
		}
	}
	else if( teamID == 1 ) // terrorists
	{
		int modelidx = cl_min_t->value;
		if( BIsValidTModelIndex(modelidx) )
			strncpy(szModel, sPlayerModelFiles[modelidx], sizeof(szModel));
		else strncpy(szModel, sPlayerModelFiles[1], sizeof(szModel) ); // set leet.mdl
	}
	else if( teamID == 2 ) // ct
	{
		int modelidx = cl_min_ct->value;

		if( g_PlayerExtraInfo[playerID].vip )
			strncpy( szModel, sPlayerModelFiles[3], sizeof(szModel) ); // vip.mdl
		else if( BIsValidCTModelIndex( modelidx ) )
			strncpy( szModel, sPlayerModelFiles[ modelidx ], sizeof(szModel));
		else strncpy( szModel, sPlayerModelFiles[2], sizeof(szModel) ); // gign.mdl
	}
	else strncpy( szModel, sPlayerModelFiles[0], sizeof(szModel) ); // player.mdl

	CreateCorpse( &origin, &angles, szModel, delay, sequence, classID );
#endif
   return 0;
}

/*
============
CL_IsDead

Returns 1 if health is <= 0
============
*/
bool CL_IsDead()
{
	if( gHUD.m_Health.m_iHealth <= 0 )
		return true;
	return false;
}