/* =================================================================================== *
      * =================== TechnoSoftware & Valve Developing =================== *
 * =================================================================================== */

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_parser.h"
#include <string.h>
#include "draw_util.h"
#include "triangleapi.h"
#include "gamemode/mods_const.h"
#include "zb3/NewScoreboard.h"

DECLARE_MESSAGE(m_Timer, RoundTime)
DECLARE_MESSAGE(m_Timer, ShowTimer)

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

inline int DrawTexturedNumbersTopRightAligned(const CTextureRef &tex, const wrect_t(&rect)[10], int iNumber, int x, int y, float scale = 1.0f)
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

int CHudTimer::Init()
{
	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	HOOK_MESSAGE(RoundTime);
	HOOK_MESSAGE(ShowTimer);
	m_iFlags = 0;
	m_bPanicColorChange = false;
	gHUD.AddHudElem(this);
	return 1;
}

void CHudTimer::Reset(void)
{
	m_closestRight = ScreenWidth;
}

int CHudTimer::VidInit()
{
	R_InitTexture(m_pTexture_Black, "resource/hud/timer_list");
	R_InitTexture(m_timer, "resource/hud/hud_sb_num_big_white");
	R_InitTexture(m_colon, "resource/hud/hud_sb_num_big_white_colon");
	BuildNumberRC(rc_m_timer_m, 18, 22);
	BuildNumberRC(rc_m_timer_s, 18, 22);
	return 1;
}

int CHudTimer::Draw(float fTime)
{
	int r, g, b;
	
	int minutes = max(0, (int)(m_iTime + m_fStartTime - gHUD.m_flTime) / 60);
	int seconds = max(0, (int)(m_iTime + m_fStartTime - gHUD.m_flTime) - (minutes * 60));

	if (minutes * 60 + seconds > 20)
	{
		DrawUtils::UnpackRGB(r, g, b,  RGB_WHITE );
	}
	else
	{
		m_flPanicTime += gHUD.m_flTimeDelta;
		DrawUtils::UnpackRGB(r, g, b, RGB_REDISH);

		if (m_flPanicTime > ((float)seconds / 40.0f) + 0.1f)
		{
			m_flPanicTime = 0;
			m_bPanicColorChange = !m_bPanicColorChange;
		}
		DrawUtils::UnpackRGB(r, g, b, m_bPanicColorChange ? RGB_YELLOWISH : RGB_WHITE);
	}

	int x = ScreenWidth / 2;
	int y = 1.5;
	
	int x2 = ScreenWidth / 2;
	int y2 = ScreenHeight / 1.04;

	int x3 = ScreenWidth / 2;
	int y3 = ScreenHeight / 1.04;

	int x4 = ScreenWidth / 2;
	int y4 = ScreenHeight / 1.05;

	int x5 = ScreenWidth / 2;
	int y5 = 5;

	int x6 = ScreenWidth / 2;
	int y6 = ScreenHeight / 1.04;
	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
	m_pTexture_Black->Bind();
	DrawUtils::Draw2DQuadScaled(x4 - 70, y4, x4 + 70, y4 + 40);

	m_colon->Bind();
	DrawUtils::Draw2DQuadScaled(x6 - 5, y6, x6 + 5, y6 + 23);

	gEngfuncs.pTriAPI->Color4ub(r, g, b, 255);
	switch (gHUD.m_iModRunning)
	{
	case MOD_ZB1:
	case MOD_ZB3:
		DrawTexturedNumbersTopRightAligned(*m_timer, rc_m_timer_m, minutes, x2 - 46, y2, 1.0f);
		DrawTexturedNumbersTopRightAligned(*m_timer, rc_m_timer_s, seconds, x3 + 38, y3, 1.0f);
		break;
	case MOD_NONE:
		DrawTexturedNumbersTopRightAligned(*m_timer, rc_m_timer_m, minutes, x2 - 46, y2, 1.0f);
		DrawTexturedNumbersTopRightAligned(*m_timer, rc_m_timer_s, seconds, x3 + 38, y3, 1.0f);
		break;
	case MOD_DM:
	case MOD_TDM:
		DrawTexturedNumbersTopRightAligned(*m_timer, rc_m_timer_m, minutes, x2 - 46, y2, 1.0f);
		DrawTexturedNumbersTopRightAligned(*m_timer, rc_m_timer_s, seconds, x3 + 38, y3, 1.0f);

		break;
	default:
		DrawTexturedNumbersTopRightAligned(*m_timer, rc_m_timer_m, minutes, x2 - 46, y2, 1.0f);
		DrawTexturedNumbersTopRightAligned(*m_timer, rc_m_timer_s, seconds, x3 + 38, y3, 1.0f);
		break;
	}
	

	return 1;
}

int CHudTimer::MsgFunc_RoundTime(const char* pszName, int iSize, void* pbuf)
{
	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	BufferReader reader(pszName, pbuf, iSize);
	m_iTime = reader.ReadShort();
	m_fStartTime = gHUD.m_flTime;
	m_iFlags = HUD_DRAW;
	return 1;
}

int CHudTimer::MsgFunc_ShowTimer(const char* pszName, int iSize, void* pbuf)
{
	m_iFlags = HUD_DRAW;
	return 1;
}

#define UPDATE_BOTPROGRESS 0
#define CREATE_BOTPROGRESS 1
#define REMOVE_BOTPROGRESS 2

DECLARE_MESSAGE(m_ProgressBar, BarTime)
DECLARE_MESSAGE(m_ProgressBar, BarTime2)
DECLARE_MESSAGE(m_ProgressBar, BotProgress)

int CHudProgressBar::Init()
{
	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	HOOK_MESSAGE(BarTime);
	HOOK_MESSAGE(BarTime2);
	HOOK_MESSAGE(BotProgress);
	m_iFlags = 0;
	m_szLocalizedHeader = NULL;
	m_szHeader[0] = '\0';
	m_fStartTime = m_fPercent = 0.0f;

	gHUD.AddHudElem(this);
	return 1;
}

int CHudProgressBar::VidInit()
{
	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	return 1;
}

int CHudProgressBar::Draw(float flTime)
{
	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	// allow only 0.0..1.0
	if ((m_fPercent < 0.0f) || (m_fPercent > 1.0f))
	{
		m_iFlags = 0;
		m_fPercent = 0.0f;
		return 1;
	}

	if (m_szLocalizedHeader && m_szLocalizedHeader[0])
	{
		int r, g, b;
		DrawUtils::UnpackRGB(r, g, b, RGB_WHITE);
		DrawUtils::DrawHudString(ScreenWidth / 4, ScreenHeight / 2, ScreenWidth, (char*)m_szLocalizedHeader, r, g, b);

		DrawUtils::DrawRectangle(ScreenWidth / 4, ScreenHeight / 2 + gHUD.GetCharHeight(), ScreenWidth / 2, ScreenHeight / 30);
		FillRGBA(ScreenWidth / 4 + 2, ScreenHeight / 2 + gHUD.GetCharHeight() + 2, m_fPercent * (ScreenWidth / 2 - 4), ScreenHeight / 30 - 4, 255, 140, 0, 255);
		return 1;
	}

	// prevent SIGFPE
	if (m_iDuration != 0.0f)
	{
		m_fPercent = ((flTime - m_fStartTime) / m_iDuration);
	}
	else
	{
		m_fPercent = 0.0f;
		m_iFlags = 0;
		return 1;
	}

	DrawUtils::DrawRectangle(ScreenWidth / 4, ScreenHeight * 2 / 3, ScreenWidth / 2, 10);
	FillRGBA(ScreenWidth / 4 + 2, ScreenHeight * 2 / 3 + 2, m_fPercent * (ScreenWidth / 2 - 4), 6, 255, 140, 0, 255);

	return 1;
	
}

int CHudProgressBar::MsgFunc_BarTime(const char* pszName, int iSize, void* pbuf)
{
	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	BufferReader reader(pszName, pbuf, iSize);

	m_iDuration = reader.ReadShort();
	m_fPercent = 0.0f;

	m_fStartTime = gHUD.m_flTime;

	m_iFlags = HUD_DRAW;
	return 1;
}

int CHudProgressBar::MsgFunc_BarTime2(const char* pszName, int iSize, void* pbuf)
{
	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	BufferReader reader(pszName, pbuf, iSize);

	m_iDuration = reader.ReadShort();
	m_fPercent = m_iDuration * (float)reader.ReadShort() / 100.0f;

	m_fStartTime = gHUD.m_flTime;

	m_iFlags = HUD_DRAW;
	return 1;
}

int CHudProgressBar::MsgFunc_BotProgress(const char* pszName, int iSize, void* pbuf)
{
	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	BufferReader reader(pszName, pbuf, iSize);
	m_iDuration = 0.0f; // don't update our progress bar
	m_iFlags = HUD_DRAW;

	float fNewPercent;
	int flag = reader.ReadByte();
	switch (flag)
	{
	case UPDATE_BOTPROGRESS:
	case CREATE_BOTPROGRESS:
		fNewPercent = (float)reader.ReadByte() / 100.0f;
		// cs behavior:
		// just don't decrease percent values
		if (m_fPercent < fNewPercent)
		{
			m_fPercent = fNewPercent;
		}
		strncpy(m_szHeader, reader.ReadString(), sizeof(m_szHeader));
		if (m_szHeader[0] == '#')
			m_szLocalizedHeader = Localize(m_szHeader + 1);
		else
			m_szLocalizedHeader = m_szHeader;
		break;
	case REMOVE_BOTPROGRESS:
	default:
		m_fPercent = 0.0f;
		m_szHeader[0] = '\0';
		m_iFlags = 0;
		m_szLocalizedHeader = NULL;
		break;
	}

	return 1; 
}