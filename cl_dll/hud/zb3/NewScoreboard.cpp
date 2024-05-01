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
	if (!m_iCharacterBG_New_Bottom)
		m_iCharacterBG_New_Bottom = R_LoadTextureUnique("resource/hud/hud_character_bg_new_bottom");
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
	R_InitTexture(iconteamkill, "resource/hud/hud_text_icon_teamkill");
	R_InitTexture(iconhm, "resource/hud/zb3/hud_text_icon_hm_left");
	R_InitTexture(iconzb, "resource/hud/zb3/hud_text_icon_zb_right");
	R_InitTexture(iconct, "resource/hud/zb3/hud_text_icon_ct_left");
	R_InitTexture(icont, "resource/hud/zb3/hud_text_icon_tr_right");

	R_InitTexture(whitebig, "resource/hud/hud_sb_num_big_white");
	R_InitTexture(newscoreboarddm, "resource/hud/hud_scoreboard_bg_zombiegaint");
	R_InitTexture(iconkill, "resource/hud/hud_text_icon_kill_center");
	R_InitTexture(iconmy, "resource/hud/hud_text_icon_my");
	R_InitTexture(icononest, "resource/hud/hud_text_icon_1st");

	BuildNumberRC( m_rcSelfnumber, 18, 22);
	BuildNumberRC( m_rcTeamnumber, 18, 22);
	BuildNumberRC( m_rcToprecord, 11, 13);
	BuildNumberRC( m_rcToprecord2, 8, 11);
	BuildNumberRC(m_rcToprecord3, 18, 22);
	BuildNumberRC( m_rcroundmax, 11, 13);
	BuildNumberRC( m_rcroundnumber, 11, 13);

    return 1;
}

void CHudZB3ScoreBoard::InitHUDData(void)
{
	m_szLastModel = "";
}

int CHudZB3ScoreBoard::Draw(float time)
{
	int x = ScreenWidth / 2; // Движение по вертикали
	int y = 5; // Движение по горизонтали

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

	int x17 = ScreenWidth / 2.03;
	int y17 = ScreenHeight / 50;

	const float flScale = 0.0f;
	int best_player = gHUD.m_Scoreboard.FindBestPlayer();
	int idx = gEngfuncs.GetLocalPlayer()->index;

	int countHM = gHUD.m_Scoreboard.m_iTeamAlive_CT;
	int countZB = gHUD.m_Scoreboard.m_iTeamAlive_T;

	int scoreCT = gHUD.m_Scoreboard.m_iTeamScore_CT;
	int scoreT = gHUD.m_Scoreboard.m_iTeamScore_T;

	int scoreMax = gHUD.m_Scoreboard.m_iNumTeams;
	int roundNumber = scoreMax ? scoreMax : scoreT + scoreCT - 1;
	
	int roundNumber2 = gHUD.m_Scoreboard.m_iTeamScore_T + gHUD.m_Scoreboard.m_iTeamScore_CT;
	int roundmax = gHUD.m_Scoreboard.m_iTeamScore_Max;

	int iX = 0;
	int iY = ScreenHeight - 5;

	int iW = m_iCharacterBG_New_Bottom->w();
	int iH = m_iCharacterBG_New_Bottom->h();

	m_iCharacterBG_New_Bottom->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);

	iW = m_iCharacterBG_New_Bottom->w();
	iH = m_iCharacterBG_New_Bottom->h();

	iX = ScreenWidth;
	iY = ScreenHeight - 5;

	if (!g_PlayerInfoList[idx].model) 
	{
		return 1;
	}

	if (m_szLastModel != g_PlayerInfoList[idx].model)
	{
		m_szLastModel = g_PlayerInfoList[idx].model;
		if (m_iCharacter)
			m_iCharacter = nullptr;

		std::string tmp = "resource/hud/portrait/hud_" + m_szLastModel;
		m_iCharacter = R_LoadTextureUnique(tmp.c_str());
	}

	if (m_iCharacter) 
	{
		iW = m_iCharacter->w();
		iH = m_iCharacter->h();
		iX = 0;
		iY = ScreenHeight - 44;

		m_iCharacter->Draw2DQuadScaled(iX, iY - iH, iX + iW, iY - iH + iH);
	}

	switch (gHUD.m_iModRunning)
	{

	 case MOD_ZB1:
	 case MOD_ZB2:
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

		if (gHUD.m_Scoreboard.m_iTeamScore_CT < 10)
		{
			DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, gHUD.m_Scoreboard.m_iTeamScore_CT, x4 + 70, y4 + 14, 1.0f);
		}
		else if (gHUD.m_Scoreboard.m_iTeamScore_CT < 100)
		{
			DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, gHUD.m_Scoreboard.m_iTeamScore_CT, x4 + 75, y4 + 14, 1.0f);
		}
		else
		{
			DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, gHUD.m_Scoreboard.m_iTeamScore_CT, x4 + 87, y4 + 14, 1.0f);
		}

		if (gHUD.m_Scoreboard.m_iTeamScore_T < 10)
		{
			DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, gHUD.m_Scoreboard.m_iTeamScore_T, x3 - 88, y3 + 14, 1.0f);
		}
		else if (gHUD.m_Scoreboard.m_iTeamScore_T < 100)
		{
			DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, gHUD.m_Scoreboard.m_iTeamScore_T, x3 - 77, y3 + 14, 1.0f);
		}
		else
		{
			DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, gHUD.m_Scoreboard.m_iTeamScore_T, x3 - 78, y3 + 14, 1.0f);
		}

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

		if (gHUD.m_Scoreboard.m_iTeamScore_CT < 10)
		{
			DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, gHUD.m_Scoreboard.m_iTeamScore_CT, x4 + 70, y4 + 14, 1.0f);
		}
		else if (gHUD.m_Scoreboard.m_iTeamScore_CT < 100)
		{
			DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, gHUD.m_Scoreboard.m_iTeamScore_CT, x4 + 75, y4 + 14, 1.0f);
		}
		else
		{
			DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, gHUD.m_Scoreboard.m_iTeamScore_CT, x4 + 87, y4 + 14, 1.0f);
		}

		if (gHUD.m_Scoreboard.m_iTeamScore_T < 10)
		{
			DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, gHUD.m_Scoreboard.m_iTeamScore_T, x3 - 88, y3 + 14, 1.0f);
		}
		else if (gHUD.m_Scoreboard.m_iTeamScore_T < 100)
		{
			DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, gHUD.m_Scoreboard.m_iTeamScore_T, x3 - 77, y3 + 14, 1.0f);
		}
		else
		{
			DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, gHUD.m_Scoreboard.m_iTeamScore_T, x3 - 78, y3 + 14, 1.0f);
		}

		DrawTexturedNumbersTopRightAligned(*countplayer, m_rcToprecord2, countHM, x5 + 77, y5, 1.0f);
		DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, countZB, x6 - 87, y5, 1.0f);

		break;
	 case MOD_DM: 
		 
		 scoreCT = g_PlayerExtraInfo[gEngfuncs.GetLocalPlayer()->index].frags;
		 scoreT = best_player ? g_PlayerExtraInfo[best_player].frags : 0;

		 roundNumber = scoreMax ? scoreMax : 0;

		 gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
		 gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

		 newscoreboarddm->Bind();
		 DrawUtils::Draw2DQuadScaled(x - 450 / 3.0, y - 4.6, x + 450 / 3.0, y + 78);

		 iconkill->Bind();
		 DrawUtils::Draw2DQuadScaled(x9 - 39.7, y9 - 1.7, x9 + 39.7, y9 + 10.7);

		 iconmy->Bind();
		 DrawUtils::Draw2DQuadScaled(x10 - 34.7, y11 - 1.7, x10 + 34.7, y11 + 10.7);

		 icononest->Bind();
		 DrawUtils::Draw2DQuadScaled(x11 - 34.7, y11 - 1.7, x11 + 34.7, y11 + 10.7);

		 gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
		 DrawTexturedNumbersTopRightAligned(*countround, m_rcToprecord, roundmax, x17 + 19, y17 + 14, 1.0f);

		 gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

		 if (scoreCT < 10)
		 {
			 DrawTexturedNumbersTopRightAligned(*whitebig, m_rcToprecord3, scoreCT, x4 + 70, y4 + 14, 1.0f);
		 }
		 else if (scoreCT < 100)
		 {
			 DrawTexturedNumbersTopRightAligned(*whitebig, m_rcToprecord3, scoreCT, x4 + 75, y4 + 14, 1.0f);
		 }
		 else
		 {
			 DrawTexturedNumbersTopRightAligned(*whitebig, m_rcToprecord3, scoreCT, x4 + 87, y4 + 14, 1.0f);
		 }

		 if (scoreT < 10)
		 {
			 DrawTexturedNumbersTopRightAligned(*whitebig, m_rcToprecord3, scoreT, x3 - 88, y3 + 14, 1.0f);
		 }
		 else if (scoreT < 100)
		 {
			 DrawTexturedNumbersTopRightAligned(*whitebig, m_rcToprecord3, scoreT, x3 - 77, y3 + 14, 1.0f);
		 }
		 else
		 {
			 DrawTexturedNumbersTopRightAligned(*whitebig, m_rcToprecord3, scoreT, x3 - 78, y3 + 14, 1.0f);
		 }

		 break;
	 case MOD_TDM:

		 gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
		 gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

		 newscoreboard->Bind();
		 DrawUtils::Draw2DQuadScaled(x - 450 / 3.0, y - 4.6, x + 450 / 3.0, y + 78);

		 iconteamkill->Bind();
		 DrawUtils::Draw2DQuadScaled(x9 - 39.7, y9 - 1.7, x9 + 39.7, y9 + 10.7);

		 iconct->Bind();
		 DrawUtils::Draw2DQuadScaled(x10 - 34.7, y11 - 1.7, x10 + 34.7, y11 + 10.7);

		 icont->Bind();
		 DrawUtils::Draw2DQuadScaled(x11 - 34.7, y11 - 1.7, x11 + 34.7, y11 + 10.7);

		 gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
		 DrawTexturedNumbersTopRightAligned(*countround, m_rcToprecord, roundmax, x17 + 19, y17 + 14, 1.0f);

		 gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
		 if (gHUD.m_Scoreboard.m_iTeamScore_CT < 10)
		 {
			 DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, gHUD.m_Scoreboard.m_iTeamScore_CT, x4 + 70, y4 + 14, 1.0f);
		 }
		 else if (gHUD.m_Scoreboard.m_iTeamScore_CT < 100)
		 {
			 DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, gHUD.m_Scoreboard.m_iTeamScore_CT, x4 + 75, y4 + 14, 1.0f);
		 }
		 else
		 {
			 DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, gHUD.m_Scoreboard.m_iTeamScore_CT, x4 + 87, y4 + 14, 1.0f);
		 }

		 if (gHUD.m_Scoreboard.m_iTeamScore_T < 10)
		 {
			 DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, gHUD.m_Scoreboard.m_iTeamScore_T, x3 - 88, y3 + 14, 1.0f);
		 }
		 else if (gHUD.m_Scoreboard.m_iTeamScore_T < 100)
		 {
			 DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, gHUD.m_Scoreboard.m_iTeamScore_T, x3 - 77, y3 + 14, 1.0f);
		 }
		 else
		 {
			 DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, gHUD.m_Scoreboard.m_iTeamScore_T, x3 - 78, y3 + 14, 1.0f);
		 }
		
		 DrawTexturedNumbersTopRightAligned(*countplayer, m_rcToprecord2, countHM, x5 + 77, y5, 1.0f);
		 DrawTexturedNumbersTopRightAligned(*countplayer2, m_rcToprecord2, countZB, x6 - 87, y5, 1.0f);

		 break;
	}
	return 1;
}