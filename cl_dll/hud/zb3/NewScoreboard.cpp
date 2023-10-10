#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "zb3.h"
#include "NewScoreboard.h"
//#include "legacy/hud_scoreboard_legacy.h"
#include "gamemode/mods_const.h"

#include "string.h"
#include "assert.h"

inline void BuildNumberRC(wrect_t(&rgrc)[32], int w, int h)
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

inline void BuildNumberRC(wrect_t(&rgrc)[32], int tex)
{
	int w = gRenderAPI.RenderGetParm(PARM_TEX_SRC_WIDTH, tex);
	int h = gRenderAPI.RenderGetParm(PARM_TEX_SRC_HEIGHT, tex);

}

inline void DrawTexturePart(const CTextureRef &tex, const wrect_t &rect, int x1, int y1, float scale = 1.0f)
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

inline int DrawTexturedNumbersTopRightAligned(const CTextureRef &tex, const wrect_t(&rect)[32], int iNumber, int x, int y, float scale = 1.0f)
{
	assert(iNumber >= 0);

	do
	{
		int k = iNumber % 32;
		iNumber /= 32;
		DrawTexturePart(tex, rect[k], x, y, scale);
		x -= (rect[k].right - rect[k].left) * scale;
	} while (iNumber > 1);

	return x;
}

inline unsigned math_log10(unsigned v)
{
	return (v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7 :
		(v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4 :
		(v >= 1000) ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0;
}

inline int DrawTexturedNumbersTopCenterAligned(const CTextureRef &tex, const wrect_t(&rect)[32], int iNumber, int x, int y, float scale = 1.0f)
{
	int n = math_log10(iNumber);
	x += (rect[0].right - rect[0].left) * (n - 1) * scale * 0.5f;
	return DrawTexturedNumbersTopRightAligned(tex, rect, iNumber, x, y, scale);
}

int CHudZB3ScoreBoard::VidInit(void)
{
	R_InitTexture(newscoreboard, "resource/hud/zb3/hud_scoreboard_bg_zb3");
	R_InitTexture(winhm, "resource/hud/zb3/hud_sb_num_big_blue");
	R_InitTexture(winzb, "resource/hud/zb3/hud_sb_num_big_red");
	R_InitTexture(countplayer, "resource/hud/zb3/hud_sb_num_center");
	R_InitTexture(healthboard, "resource/hud/zb3/");
	R_InitTexture(ammoboard,"resource/hud/zb3/hud_weapon_bg");
	R_InitTexture(weaponboard, "resource/hud/zb3/");
   
	BuildNumberRC(m_rcSelfnumber, 18, 22);
	BuildNumberRC(m_rcTeamnumber, 18, 22);

	BuildNumberRC(m_rcToprecord, 11, 13);

	return 1;
}
#include "eventscripts.h"
int CHudZB3ScoreBoard::Draw(float time)
{   
	
	int x = ScreenWidth / 2; // Движение по горизонтали
	int y = 5; // Движение по вертикали

	int x2 = ScreenWidth / 2;
	int y2 = 8;

	int x3 = ScreenWidth / 2;
	int y3 = 2;

	int x4 = ScreenWidth / 2;
	int y4 = 2;

	int x6 = ScreenWidth / 2;
	int x5 = ScreenWidth / 2;
	int y5 = 57;

	int x7 = ScreenWidth / 1.1;
	int y7 = 1000; // Движение по вертикали

	const float flScale = 0.0f;
	gEngfuncs.pTriAPI->RenderMode(kRenderTransAlpha);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);     
    newscoreboard->Bind();                                 
	DrawUtils::Draw2DQuadScaled(x - 481 / 3.0,/*длина*/ y - 4.5, x + 481 / 3.0, y + 77/*Толщина*/);

	int best_player = gHUD.m_Scoreboard.FindBestPlayer();
	int scoreCT = gHUD.m_Scoreboard.m_iTeamScore_CT;
	int scoreT = gHUD.m_Scoreboard.m_iTeamScore_T;
	int countZB = gHUD.m_Scoreboard.m_iTeamAlive_T; 
	int scoreMax = gHUD.m_Scoreboard.m_iNumTeams;
	int countHM = gHUD.m_Scoreboard.m_iTeamAlive_CT;
	int roundNumber = scoreMax ? scoreMax : scoreT + scoreCT + 1;
	DrawTexturedNumbersTopCenterAligned(*winzb, m_rcSelfnumber, scoreT, x3 - 65, y3 + 14, 1.45f);
	DrawTexturedNumbersTopCenterAligned(*winhm, m_rcTeamnumber, scoreCT, x4 + 70, y4 + 14, 1.45f); // ok// ok
	DrawTexturedNumbersTopCenterAligned(*countplayer, m_rcToprecord, countHM, x5 + 77,  y5 );
    DrawTexturedNumbersTopCenterAligned(*countplayer, m_rcToprecord, countZB, x6 - 77, y5 );
	
	int idx = IS_FIRSTPERSON_SPEC ? g_iUser2 : gEngfuncs.GetLocalPlayer()->index;
   

	if (!g_PlayerExtraInfo[idx].zombie)
	{ 
	ammoboard->Bind();
	DrawUtils::Draw2DQuadScaled(x7 - 550 / 3.0,/*длина*/ y7 + 4.5, x7 + 550 / 3.0, y7 + 77/*Толщина*/);
	return 1;
    }
    
	
	return 1;

}