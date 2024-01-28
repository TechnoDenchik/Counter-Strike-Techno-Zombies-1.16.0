/* =================================================================================== *
	  * =================== TechnoSoftware & Valve Developing =================== *
 * =================================================================================== */

#include "hud.h"
#include "cl_util.h"
#include "draw_util.h"
#include "NewScoreboard.h"
#include "gamemode/mods_const.h"
#include "eventscripts.h"
#include "triangleapi.h"
#include "player/player_const.h"

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

int CHudZB3ScoreBoard::VidInit(void)
{ 
	R_InitTexture(newscoreboard, "resource/hud/hud_scoreboard_bg_gundeath");
    R_InitTexture(weaponboard, "resource/hud/zb3/weapon_list_new");
    R_InitTexture(ammoboard,"resource/hud/zb3/hud_weapon_bg");
	R_InitTexture(textstring, "resource/hud/zb3/hud_string_bg");
	R_InitTexture(slash, "resource/hud/hud_sb_num_center_slash");
	R_InitTexture(winhm, "resource/hud/zb3/hud_sb_num_big_blue");
	R_InitTexture(winzb, "resource/hud/zb3/hud_sb_num_big_red");
	R_InitTexture(countround, "resource/hud/zb3/hud_sb_num_center");
	R_InitTexture(countkill, "resource/hud/zb3/hud_sb_num_center");
	R_InitTexture(countplayer, "resource/hud/hud_sb_num_small_blue");
	R_InitTexture(countplayer2, "resource/hud/hud_sb_num_small_red");
	R_InitTexture(iconround, "resource/hud/zb3/hud_text_icon_round");
	R_InitTexture(iconhm, "resource/hud/zb3/hud_text_icon_hm_left");
	R_InitTexture(iconzb, "resource/hud/zb3/hud_text_icon_zb_right");
	R_InitTexture(iconct, "resource/hud/zb3/hud_text_icon_ct_left");
	R_InitTexture(icont, "resource/hud/zb3/hud_text_icon_tr_right");

	BuildNumberRC( m_rcSelfnumber, 18, 22);
	BuildNumberRC( m_rcTeamnumber, 18, 22);
	BuildNumberRC( m_rcToprecord, 11, 13);
	BuildNumberRC( m_rcToprecord2, 8, 11);
	BuildNumberRC( m_rcroundmax, 11, 13);
	BuildNumberRC( m_rcroundnumber, 11, 13);

    return 1;
}

int CHudZB3ScoreBoard::Draw(float time)
{
	int x = ScreenWidth / 2; // Движение по горизонтали
	int y = 5; // Движение по вертикали

	int x2 = ScreenWidth / 2;
	int y2 = 8;

	int x3 = ScreenWidth / 2;
	int y3 = 10;

	int x4 = ScreenWidth / 2;
	int y4 = 10;

	int x5 = ScreenWidth / 2;
	int y5 = 60;

	int x6 = ScreenWidth / 2;

	int x7 = ScreenWidth / 1.1;
	int y7 = 995;

	int x8 = ScreenWidth / 10.4;
	int y8 = 980;

	int x9 = ScreenWidth / 2;
	int y9 = 20;

	int x10 = ScreenWidth / 1.85;

	int x11 = ScreenWidth / 2.17;
	int y11 = 10;

	int x12 = ScreenWidth / 2;
	int y12 = ScreenHeight / 4;

	int x13 = ScreenWidth / 10.4;
	int y13 = 980;

	int x14 = ScreenWidth / 10.4;
	int y14 = 980;

	int x15 = ScreenWidth / 2;
	int y15 = 43;

	int x16 = ScreenWidth / 2;
	int y16 = 20.5;

	const float flScale = 0.0f;
	int best_player = gHUD.m_Scoreboard.FindBestPlayer();
	int idx = IS_FIRSTPERSON_SPEC ? g_iUser2 : gEngfuncs.GetLocalPlayer()->index;

	int countHM = gHUD.m_Scoreboard.m_iTeamAlive_CT;
	int countZB = gHUD.m_Scoreboard.m_iTeamAlive_T;

	int scoreCT = gHUD.m_Scoreboard.m_iTeamScore_CT;
	int scoreT = gHUD.m_Scoreboard.m_iTeamScore_T;
	
	int scoreMax = gHUD.m_Scoreboard.m_iNumTeams;
	int roundNumber = scoreMax ? scoreMax : scoreT + scoreCT - 1;
	int roundmax = gHUD.m_Scoreboard.m_iTeamScore_Max;

	int roundNumber2 = gHUD.m_Scoreboard.m_iTeamScore_T + gHUD.m_Scoreboard.m_iTeamScore_CT;

	switch (gHUD.m_iModRunning)
	{

	 case MOD_ZB1:
	 case MOD_ZB3:
	 		
		gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

		newscoreboard->Bind();
		DrawUtils::Draw2DQuadScaled(x - 450 / 3.0, y - 4.6, x + 450 / 3.0, y + 78);

		iconround->Bind();
		DrawUtils::Draw2DQuadScaled(x9 - 39.7, y9 - 1.7, x9 + 39.7, y9 + 10.7);

		slash->Bind();
		DrawUtils::Draw2DQuadScaled(x15 - 4.0, y15 - 8.7, x15 + 4.0, y15 + 5.0);

		iconhm->Bind();
		DrawUtils::Draw2DQuadScaled(x10 - 34.7, y11 - 1.7, x10 + 34.7, y11 + 10.7);

		iconzb->Bind();
		DrawUtils::Draw2DQuadScaled(x11 - 34.7, y11 - 1.7, x11 + 34.7, y11 + 10.7);

		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
		DrawTexturedNumbersTopRightAligned(*countround, m_rcToprecord, roundmax, x16 + 19, y16 + 14, 1.0f);
		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
		DrawTexturedNumbersTopRightAligned(*countround, m_rcToprecord, roundNumber2, x16 - 24, y16 + 14, 1.0f);

		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
		DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, gHUD.m_Scoreboard.m_iTeamScore_CT, x4 + 70, y4 + 14, 1.0f);
		DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, gHUD.m_Scoreboard.m_iTeamScore_T, x3 - 87, y3 + 14, 1.0f);
		DrawTexturedNumbersTopRightAligned(*countplayer, m_rcToprecord2, countHM, x5 + 77, y5, 1.0f);
		DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, countZB, x6 - 87, y5, 1.0f);
		
	 break;
	 case MOD_NONE:

		gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

		newscoreboard->Bind();
		DrawUtils::Draw2DQuadScaled(x - 450 / 3.0, y - 4.6, x + 450 / 3.0, y + 78);

		iconround->Bind();
		DrawUtils::Draw2DQuadScaled(x9 - 39.7, y9 - 1.7, x9 + 39.7, y9 + 10.7);

		slash->Bind();
		DrawUtils::Draw2DQuadScaled(x15 - 4.0, y15 - 8.7, x15 + 4.0, y15 + 5.0);

		iconct->Bind();
		DrawUtils::Draw2DQuadScaled(x10 - 34.7, y11 - 1.7, x10 + 34.7, y11 + 10.7);

		icont->Bind();
		DrawUtils::Draw2DQuadScaled(x11 - 34.7, y11 - 1.7, x11 + 34.7, y11 + 10.7);

		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
		DrawTexturedNumbersTopRightAligned(*countround, m_rcToprecord, roundmax, x16 + 19, y16 + 14, 1.0f);
		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
		DrawTexturedNumbersTopRightAligned(*countround, m_rcToprecord, roundNumber2, x16 - 24, y16 + 14, 1.0f);

		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
		DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, gHUD.m_Scoreboard.m_iTeamScore_CT, x4 + 70, y4 + 14, 1.0f);
		DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, gHUD.m_Scoreboard.m_iTeamScore_T, x3 - 87, y3 + 14, 1.0f);
		DrawTexturedNumbersTopRightAligned(*countplayer, m_rcToprecord2, countHM, x5 + 77, y5, 1.0f);
		DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, countZB, x6 - 87, y5, 1.0f);

		break;
	}
	return 1;
}

int CHudZB3ScoreBoard::Draw2()
{
	int x = ScreenWidth / 2; // Движение по горизонтали
	int y = 15; // Движение по вертикали

	newscoreboard->Bind();
	DrawUtils::Draw2DQuadScaled(x - 450 / 3.0, y - 4.6, x + 450 / 3.0, y + 78);
	return 1;
}