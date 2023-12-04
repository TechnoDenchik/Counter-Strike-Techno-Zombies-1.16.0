#include "hud.h"
#include "cl_util.h"
#include "draw_util.h"
#include "NewScoreboard.h"
#include "gamemode/mods_const.h"
#include "eventscripts.h"
//#include "cs_wpn/cs_weapons.cpp"

void CHudZB3ScoreBoard::BuildNumberRC(wrect_t(&rgrc)[10], int w, int h)
{
	int nw = 0;
	//first start num pos
	for (int i = 0; i < 10; i++)
	{
		rgrc[i].left = nw;
		rgrc[i].top = 0;
		rgrc[i].right = rgrc[i].left + w;
		rgrc[i].bottom = h;

		nw += w;
	}
}


unsigned CHudZB3ScoreBoard::math_log10(unsigned v, int iflags)
{
	if (v >= 10000)
		return 5;
	else if (iflags & (DHN_5DIGITS))
		return 5;
	else if (v >= 1000)
		return 4;
	else if (iflags & (DHN_4DIGITS))
		return 4;
	else if (v >= 100)
		return 3;
	else if (iflags & (DHN_3DIGITS))
		return 3;
	else if (v >= 10)
		return 2;
	else if (iflags & (DHN_2DIGITS))
		return 2;
	else
		return 1;


	return (v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7 :
		(v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4 :
		(v >= 1000) ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0;
}

unsigned CHudZB3ScoreBoard::math_log10_long(unsigned v, int iflags)
{
	if (v > 100000000)
		return 9;
	else if (iflags & (DHN_9DIGITS))
		return 9;
	else if (v > 10000000)
		return 8;
	else if (iflags & (DHN_8DIGITS))
		return 8;
	else if (v > 1000000)
		return 7;
	else if (iflags & (DHN_7DIGITS))
		return 7;
	else if (v > 100000)
		return 6;
	else if (iflags & (DHN_6DIGITS))
		return 6;
	else if (v >= 10000)
		return 5;
	else if (iflags & (DHN_5DIGITS))
		return 5;
	else if (v >= 1000)
		return 4;
	else if (iflags & (DHN_4DIGITS))
		return 4;
	else if (v >= 100)
		return 3;
	else if (iflags & (DHN_3DIGITS))
		return 3;
	else if (v >= 10)
		return 2;
	else if (iflags & (DHN_2DIGITS))
		return 2;
	else
		return 1;


	return (v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7 :
		(v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4 :
		(v >= 1000) ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0;
}

unsigned CHudZB3ScoreBoard::math_div(unsigned v, int iflags)
{
	int maxsize = 0;
	if (maxsize <= 0)
	{
		maxsize = 1;

		for (int num = 10; (v / num) > 0; num *= 10)
			maxsize++;
	}

	if (maxsize > 255)
		maxsize = 255;

	return maxsize;
}

int CHudZB3ScoreBoard::DrawTexturedNumbersTopRightAligned(const CTextureRef& tex, const wrect_t(&rect)[10], int iNumber, int x, int y, int iflags, int igaps, float scale, byte r, byte g, byte b, byte a, int Long)
{
	assert(iNumber >= 0);

	int iRight = x;

	int iTime = Long ? CHudZB3ScoreBoard::math_log10_long(iNumber, iflags) : CHudZB3ScoreBoard::math_log10(iNumber, iflags);

	if (iNumber >= 10000)
	{
		iTime = CHudZB3ScoreBoard::math_log10_long(iNumber, iflags);
	}
	else
	{
		iTime = CHudZB3ScoreBoard::math_log10(iNumber, iflags);
	}
	for (int i = 1; i <= iTime; i++)
	{
		int k = iNumber % 10;
		iNumber /= 10;

		CHudZB3ScoreBoard::DrawTexturePart(tex, rect[k], x, y, scale, r, g, b, a);
		x -= (rect[k].right - rect[k].left + igaps) * scale;
	}

	iRight += (rect[0].right - rect[0].left + igaps) * scale;	//caculate length should reduce one more times

	return iRight;
}

void CHudZB3ScoreBoard::DrawTexturePart(const CTextureRef& tex, const wrect_t& rect, int x1, int y1, float scale, byte r, byte g, byte b, byte a)
{
	float w = tex.w();
	float h = tex.h();

	int x2 = x1 + (rect.right - rect.left) * scale;
	int y2 = y1 + (rect.bottom - rect.top) * scale;

	tex.Draw2DQuadScaled(x1, y1, x2, y2, rect.left / w, rect.top / h, rect.right / w, rect.bottom / h, r, g, b, a);
}

int CHudZB3ScoreBoard::DrawTexturedNumbers(const CTextureRef& tex, const wrect_t(&rect)[10], int iNumber, int x, int y, int iflags, int igaps, float scale, byte r, byte g, byte b, byte a, int iLong)
{


	int n;


	if (iNumber >= 10000)
	{
		n = CHudZB3ScoreBoard::math_log10_long(iNumber, iflags);
	}
	else
	{
		n = CHudZB3ScoreBoard::math_log10(iNumber, iflags);
	}

	if ((iflags & DHN_CENTERALIGNED))
		x += (rect[0].right - rect[0].left + igaps) * (n - 2) * scale;	//0.5?
	else
		x += (rect[0].right - rect[0].left + igaps) * (n - 1) * scale;

	return CHudZB3ScoreBoard::DrawTexturedNumbersTopRightAligned(tex, rect, iNumber, x, y, iflags, igaps, scale, r, g, b, a, iLong);
}

int CHudZB3ScoreBoard::DrawTexturedNumbersLong(const CTextureRef& tex, const wrect_t(&rect)[10], int iNumber, int x, int y, int iflags, int igaps, float scale, byte r, byte g, byte b, byte a)
{

	int maxsize = 0;
	if (maxsize <= 0)
	{
		maxsize = 1;

		for (int num = 10; (iNumber / num) > 0; num *= 10)
			maxsize++;
	}

	if (maxsize > 255)
		maxsize = 255;
	bool bShouldDraw = false;



	for (int i = 0; i < maxsize; i++)
	{
		int div = 1;
		for (int j = 0; j < maxsize - i; j++)
			div *= 10;

		int iNum = (iNumber % div * 10) / div;

		if (iNum)
			bShouldDraw = true;

		if (!iNum && !bShouldDraw && i != maxsize - 1)
			continue;


		if ((iflags & DHN_CENTERALIGNED))
			x += (rect[0].right - rect[0].left + igaps) * (maxsize - 2) * scale;	//0.5?
		else
			x += (rect[0].right - rect[0].left + igaps) * (maxsize - 1) * scale;

		CHudZB3ScoreBoard::DrawTexturedNumbersTopRightAligned(tex, rect, iNumber, x, y, iflags, igaps, scale, r, g, b, a);
	}
	return x;
}


int CHudZB3ScoreBoard::VidInit(void)
{
	R_InitTexture(newscoreboard, "resource/hud/zb3/hud_scoreboard_bg");
    R_InitTexture(weaponboard, "resource/hud/zb3/weapon_list_new");
    R_InitTexture(healthboard, "resource/hud/zb3/hud_character_bg_bottom");
	R_InitTexture(healthboard2, "resource/hud/zb3/hud_character_bg_top");
    R_InitTexture(ammoboard,"resource/hud/zb3/hud_weapon_bg");
	R_InitTexture(textstring, "resource/hud/zb3/hud_string_bg");

	R_InitTexture(winhm, "resource/hud/zb3/hud_sb_num_big_blue");
	R_InitTexture(winzb, "resource/hud/zb3/hud_sb_num_big_red");
	R_InitTexture(countplayer, "resource/hud/zb3/hud_sb_num_center");
	R_InitTexture(iconround, "resource/hud/zb3/hud_text_icon_round");
	R_InitTexture(iconhm, "resource/hud/zb3/hud_text_icon_hm_left");
	R_InitTexture(iconzb, "resource/hud/zb3/hud_text_icon_zb_right");
	R_InitTexture(iconwinhm, "resource/hud/zb3/humanwin");
	R_InitTexture(iconwinzb, "resource/hud/zb3/zombiewin");
	R_InitTexture(ammocounts, "resource/hud/zb3/hud_character_num");
	R_InitTexture(iconclassmilitia, "resource/hud/zb3/militia");

	BuildHudNumberRect(m_rcSelfnumbers, m_rcSelfnumber, 18, 22, 1, 1);
	BuildHudNumberRect(m_rcTeamnumbers, m_rcTeamnumber, 18, 22, 1, 1);
	BuildHudNumberRect(m_rcAmmocounts, m_rcAmmocount, 18, 22, 1, 1);
	BuildHudNumberRect(m_rcToprecords, m_rcToprecord, 11, 13, 1, 1);
     return 0;
}

int CHudZB3ScoreBoard::Draw(float time)
{

	int x = ScreenWidth / 2; // Движение по горизонтали
	int y = 5; // Движение по вертикали

	int x2 = ScreenWidth / 2;
	int y2 = 8;

	int x3 = ScreenWidth / 2;
	int y3 = 7;

	int x4 = ScreenWidth / 2;
	int y4 = 7;

	int x6 = ScreenWidth / 2;
	int x5 = ScreenWidth / 2;
	int y5 = 57;

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

	const float flScale = 0.0f;
	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);

	newscoreboard->Bind();
	DrawUtils::Draw2DQuadScaled(x - 481 / 3.0, y - 4.5, x + 481 / 3.0, y + 77);

	iconround->Bind();
	DrawUtils::Draw2DQuadScaled(x9 - 39.7, y9 - 1.7, x9 + 39.7, y9 + 10.7);

	iconhm->Bind();
	DrawUtils::Draw2DQuadScaled(x10 - 34.7, y11 - 1.7, x10 + 34.7, y11 + 10.7);

	iconzb->Bind();
	DrawUtils::Draw2DQuadScaled(x11 - 34.7, y11 - 1.7, x11 + 34.7, y11 + 10.7);


	int best_player = gHUD.m_Scoreboard.FindBestPlayer();

	int countHM = gHUD.m_Scoreboard.m_iTeamAlive_CT;
	int countZB = gHUD.m_Scoreboard.m_iTeamAlive_T;

	int scoreCT = gHUD.m_Scoreboard.m_iTeamScore_CT;
	int scoreT = gHUD.m_Scoreboard.m_iTeamScore_T;
	int scoreMax = gHUD.m_Scoreboard.m_iNumTeams;
	int roundNumber = scoreMax ? scoreMax : scoreT + scoreCT + 1;
	int countammo = gHUD.m_Ammo.m_iAmmoLastCheck;

	DrawTexturedNumbersTopRightAligned(*winhm, m_rcTeamnumber, scoreCT, x4 + 70, y4 + 14, 1.20f);
	DrawTexturedNumbersTopRightAligned(*winzb, m_rcSelfnumber, scoreT, x3 - 65, y3 + 14, 1.20f);
	DrawTexturedNumbersTopRightAligned(*countplayer, m_rcToprecord, countHM, x5 + 77, y5, 0.60f);
	DrawTexturedNumbersTopRightAligned(*countplayer, m_rcToprecord, countZB, x6 - 77, y5, 0.60f);
	//DrawTexturedNumbersTopCenterAligned(*ammocounts, m_rcAmmocount, countammo, x6 - 77, y5);
	int idx = IS_FIRSTPERSON_SPEC ? g_iUser2 : gEngfuncs.GetLocalPlayer()->index;

	if (!g_PlayerExtraInfo[idx].zombie && !g_PlayerExtraInfo[idx].dead)
	{
		ammoboard->Bind();
		DrawUtils::Draw2DQuadScaled(x7 - 550 / 3.0, y7 + 4.5, x7 + 550 / 3.0, y7 + 77);

	}
	if (!g_PlayerExtraInfo[idx].dead)
	{
		healthboard->Bind();
		DrawUtils::Draw2DQuadScaled(x8 - 550 / 3.0, y8 + 5.5, x8 + 550 / 3.0, y8 + 95);

		healthboard2->Bind();
		DrawUtils::Draw2DQuadScaled(x8 - 550 / 3.0, y8 + 5.5, x8 + 550 / 3.0, y8 + 95);

	}
	//if (sPlayerModelFiles[11])
	//{
    //  iconclassmilitia->Bind();
    //   DrawUtils::Draw2DQuadScaled(x14 - 550 / 3.0, y14 + 5.5, x14 + 550 / 3.0, y14 + 95);
   // }
     return 1;
	return 1;
}

void CHudZB3ScoreBoard::HumanWin()
{
    //if(ROUND_CTS_WIN)
	{ 
	int x12 = ScreenWidth / 2;
	int y12 = ScreenHeight / 4;
	iconwinhm->Bind();
	DrawUtils::Draw2DQuadScaled(x12 - 373 / 2, y12, x12 + 373 / 2, y12 + 51);
	}

}

void CHudZB3ScoreBoard::ZombieWin()
{
	//if(ROUND_TERRORISTS_WIN)
	{ 
	int x12 = ScreenWidth / 2;
	int y12 = ScreenHeight / 4;
	iconwinzb->Bind();
	DrawUtils::Draw2DQuadScaled(x12 - 373 / 2, y12, x12 + 373 / 2, y12 + 51);
	
	}
	
}

void CHudZB3ScoreBoard::BuildHudNumberRect(int moe, wrect_t* prc, int w, int h, int xOffset, int yOffset)
{
	wrect_t rc = gHUD.GetSpriteRect(moe);
	int x = rc.left;
	int y = rc.top;

	for (int i = 0; i < 10; i++)
	{
		prc[i].left = x;
		prc[i].top = 0;
		prc[i].right = prc[i].left + w + xOffset;
		prc[i].bottom = h + yOffset;

		x += w;
		y += h;
	}
}

int CHudZB3ScoreBoard::DrawHudNumber(int moe, wrect_t* prc, int x, int y, int iFlags, int iNumber, int r, int g, int b)
{
	int iWidth = prc[0].right - prc[0].left;
	int k;
	wrect_t rc;

	if (iNumber >= 10000)
	{
		k = iNumber / 10000;
		SPR_Set(gHUD.GetSprite(moe), r, g, b);
		SPR_DrawAdditive(0, x, y, &prc[k]);
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS))
	{
		if (iFlags & DHN_DRAWZERO)
		{
			SPR_Set(gHUD.GetSprite(moe), r, g, b);
			SPR_DrawAdditive(0, x, y, &prc[0]);
		}

		x += iWidth;
	}

	if (iNumber >= 1000)
	{
		k = (iNumber % 10000) / 1000;
		SPR_Set(gHUD.GetSprite(moe), r, g, b);
		SPR_DrawAdditive(0, x, y, &prc[k]);
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS))
	{
		if (iFlags & DHN_DRAWZERO)
		{
			SPR_Set(gHUD.GetSprite(moe), r, g, b);
			SPR_DrawAdditive(0, x, y, &prc[0]);
		}

		x += iWidth;
	}

	if (iNumber >= 100)
	{
		k = (iNumber % 1000) / 100;
		SPR_Set(gHUD.GetSprite(moe), r, g, b);
		SPR_DrawAdditive(0, x, y, &prc[k]);
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS))
	{
		if (iFlags & DHN_DRAWZERO)
		{
			SPR_Set(gHUD.GetSprite(moe), r, g, b);
			SPR_DrawAdditive(0, x, y, &prc[0]);
		}

		x += iWidth;
	}

	if (iNumber >= 10)
	{
		k = (iNumber % 100) / 10;
		rc = prc[k];
		SPR_Set(gHUD.GetSprite(moe), r, g, b);
		SPR_DrawAdditive(0, x, y, &rc);
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS))
	{
		if (iFlags & DHN_DRAWZERO)
		{
			SPR_Set(gHUD.GetSprite(moe), r, g, b);
			SPR_DrawAdditive(0, x, y, &prc[0]);
		}

		x += iWidth;
	}

	k = iNumber % 10;
	SPR_Set(gHUD.GetSprite(moe), r, g, b);
	SPR_DrawAdditive(0, x, y, &prc[k]);
	x += iWidth;

	return x;
}

int CHudZB3ScoreBoard::GetHudNumberWidth(int moe, wrect_t* prc, int iFlags, int iNumber)
{
	int iWidth = prc[0].right - prc[0].left;
	int k;
	wrect_t rc;
	int x = 0;

	if (iNumber >= 10000)
	{
		k = iNumber / 10000;
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS))
		x += iWidth;

	if (iNumber >= 1000)
	{
		k = (iNumber % 10000) / 1000;
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS))
		x += iWidth;

	if (iNumber >= 100)
	{
		k = (iNumber % 1000) / 100;
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS))
		x += iWidth;

	if (iNumber >= 10)
	{
		k = (iNumber % 100) / 10;
		rc = prc[k];
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS))
		x += iWidth;

	k = iNumber % 10;
	x += iWidth;

	return x;
}


/*
CHudZB3ScoreBoard::CHudZB3ScoreBoard()
{
	BuildNumberRC(m_iNum_BigBlueC, 18, 22);
	BuildNumberRC(m_iNum_BigRedC, 18, 22);
	BuildNumberRC(m_iNum_CenterC, 11, 13);
	BuildNumberRC(m_iNum_SmallBlueC, 8, 11);
	BuildNumberRC(m_iNum_SmallRedC, 8, 11);
	BuildNumberRC(m_iNum_BigWhiteC, 18, 22);
}

CHudZB3ScoreBoard::~CHudZB3ScoreBoard()
{

}

int CHudZB3ScoreBoard::VidInit(void)
{
	m_iOriginalBG = gHUD.GetSpriteIndex("SBOriginalBG");
	m_iTeamDeathBG = gHUD.GetSpriteIndex("SBTeamDeathBG");
	m_iUnitehBG = gHUD.GetSpriteIndex("SBUnitehBG");
	m_iNum_L = gHUD.GetSpriteIndex("SBNum_L");
	m_iNum_S = gHUD.GetSpriteIndex("SBNum_S");
	m_iNum_csgo = gHUD.GetSpriteIndex("csgo_number");
	m_iText_CT = gHUD.GetSpriteIndex("SBText_CT");
	m_iText_T = gHUD.GetSpriteIndex("SBText_T");
	m_iText_TR = gHUD.GetSpriteIndex("SBText_TR");
	m_iText_HM = gHUD.GetSpriteIndex("SBText_HM");
	m_iText_ZB = gHUD.GetSpriteIndex("SBText_ZB");
	m_iText_1st = gHUD.GetSpriteIndex("SBText_1st");
	m_iText_Kill = gHUD.GetSpriteIndex("SBText_Kill");
	m_iText_Round = gHUD.GetSpriteIndex("SBText_Round");
	R_InitTexture(m_pTexture_Board, "resource/hud/csgo/board");
	BuildHudNumberRect(m_iNum_csgo, m_rcNumber_csgo, 21, 30, 1, 1);
	BuildHudNumberRect(m_iNum_L, m_rcNumber_Large, 13, 13, 1, 1);
	BuildHudNumberRect(m_iNum_S, m_rcNumber_Small, 10, 10, 1, 1);

	if (!m_iNum_BigBlue)
		m_iNum_BigBlue = R_LoadTextureShared("resource/hud/hud_sb_num_big_blue");

	if (!m_iNum_BigRed)
		m_iNum_BigRed = R_LoadTextureShared("resource/hud/hud_sb_num_big_red");

	if (!m_iNum_BigWhite)
		m_iNum_BigWhite = R_LoadTextureShared("resource/hud/hud_sb_num_big_white");

	if (!m_iNum_Center)
		m_iNum_Center = R_LoadTextureShared("resource/hud/hud_sb_num_center");

	if (!m_iNum_SmallBlue)
		m_iNum_SmallBlue = R_LoadTextureShared("resource/hud/hud_sb_num_small_blue");

	if (!m_iNum_SmallRed)
		m_iNum_SmallRed = R_LoadTextureShared("resource/hud/hud_sb_num_small_red");

	if (!m_iScoreboardBG)
		m_iScoreboardBG = R_LoadTextureUnique("resource/hud/hud_scoreboard_bg");

	if (!m_iScoreboardBG_GD)
		m_iScoreboardBG_GD = R_LoadTextureUnique("resource/hud/hud_scoreboard_bg_gundeath");

	if (!m_iScoreboardBG_HMS)
		m_iScoreboardBG_HMS = R_LoadTextureUnique("resource/hud/hud_scoreboard_bg_humanscenario");

	if (!m_iScoreboardBG_TIME)
		m_iScoreboardBG_TIME = R_LoadTextureUnique("resource/hud/hud_scoreboard_bg_time");

	if (!m_iScoreboardBG_ZB3)
		m_iScoreboardBG_ZB3 = R_LoadTextureUnique("resource/hud/hud_scoreboard_bg_zb3");

	if (!m_iScoreboardBG_ZBF)
		m_iScoreboardBG_ZBF = R_LoadTextureUnique("resource/hud/hud_scoreboard_bg_zfile_pve");

	if (!m_iScoreboardBG_ZBG)
		m_iScoreboardBG_ZBG = R_LoadTextureUnique("resource/hud/hud_scoreboard_bg_zombie-gaint");

	if (!m_iScoreboardBG_ZSHT01)
		m_iScoreboardBG_ZSHT01 = R_LoadTextureUnique("resource/hud/hud_scoreboard_bg_zsht_pvp01");

	if (!m_iScoreboardBG_ZSHT02)
		m_iScoreboardBG_ZSHT02 = R_LoadTextureUnique("resource/hud/hud_scoreboard_bg_zsht_pvp02");

	if (!m_iScoreboardBG_ZTD01)
		m_iScoreboardBG_ZTD01 = R_LoadTextureUnique("resource/hud/hud_scoreboard_bg_ztd01");

	if (!m_iScoreboardBG_ZSHT02)
		m_iScoreboardBG_ZSHT02 = R_LoadTextureUnique("resource/hud/hud_scoreboard_bg_ztd02");

	if (!m_iIcon_CT_Left)
		m_iIcon_CT_Left = R_LoadTextureUnique("resource/hud/hud_text_icon_ct_left");

	if (!m_iIcon_CT_Right)
		m_iIcon_CT_Right = R_LoadTextureUnique("resource/hud/hud_text_icon_ct_right");

	if (!m_iIcon_TR_Left)
		m_iIcon_TR_Left = R_LoadTextureUnique("resource/hud/hud_text_icon_tr_left");

	if (!m_iIcon_TR_Right)
		m_iIcon_TR_Right = R_LoadTextureUnique("resource/hud/hud_text_icon_tr_right");

	if (!m_iIcon_Round)
		m_iIcon_Round = R_LoadTextureUnique("resource/hud/hud_text_icon_round");

	if (!m_iSlash_Center)
		m_iSlash_Center = R_LoadTextureUnique("resource/hud/hud_sb_num_center_slash");

	if (!m_iIcon_Win_Center)
		m_iIcon_Win_Center = R_LoadTextureUnique("resource/hud/hud_text_icon_win_center");

	if (!m_iIcon_ZB_Left)
		m_iIcon_ZB_Left = R_LoadTextureUnique("resource/hud/hud_text_icon_zb_left");

	if (!m_iIcon_ZB_Right)
		m_iIcon_ZB_Right = R_LoadTextureUnique("resource/hud/hud_text_icon_zb_right");

	if (!m_iIcon_HM_Left)
		m_iIcon_HM_Left = R_LoadTextureUnique("resource/hud/hud_text_icon_hm_left");

	if (!m_iIcon_HM_Right)
		m_iIcon_HM_Right = R_LoadTextureUnique("resource/hud/hud_text_icon_hm_right");

	if (!m_iIcon_1st)
		m_iIcon_1st = R_LoadTextureUnique("resource/hud/hud_text_icon_1st");

	if (!m_iIcon_My)
		m_iIcon_My = R_LoadTextureUnique("resource/hud/hud_text_icon_my");

	if (!m_iIcon_KillC)
		m_iIcon_KillC = R_LoadTextureUnique("resource/hud/hud_text_icon_kill_center");

	if (!m_iIcon_TeamKill)
		m_iIcon_TeamKill = R_LoadTextureUnique("resource/hud/hud_text_icon_team-kill");

	return 1;
}


void CHudZB3ScoreBoard::BuildNumberRC(wrect_t(&rgrc)[10], int w, int h)
{
	int nw = 0;
	//first start num pos
	for (int i = 0; i < 10; i++)
	{
		rgrc[i].left = nw;
		rgrc[i].top = 0;
		rgrc[i].right = rgrc[i].left + w;
		rgrc[i].bottom = h;

		nw += w;
	}
}


unsigned CHudZB3ScoreBoard::math_log10(unsigned v, int iflags)
{
	if (v >= 10000)
		return 5;
	else if (iflags & (DHN_5DIGITS))
		return 5;
	else if (v >= 1000)
		return 4;
	else if (iflags & (DHN_4DIGITS))
		return 4;
	else if (v >= 100)
		return 3;
	else if (iflags & (DHN_3DIGITS))
		return 3;
	else if (v >= 10)
		return 2;
	else if (iflags & (DHN_2DIGITS))
		return 2;
	else
		return 1;


	return (v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7 :
		(v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4 :
		(v >= 1000) ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0;
}

unsigned CHudZB3ScoreBoard::math_log10_long(unsigned v, int iflags)
{
	if (v > 100000000)
		return 9;
	else if (iflags & (DHN_9DIGITS))
		return 9;
	else if (v > 10000000)
		return 8;
	else if (iflags & (DHN_8DIGITS))
		return 8;
	else if (v > 1000000)
		return 7;
	else if (iflags & (DHN_7DIGITS))
		return 7;
	else if (v > 100000)
		return 6;
	else if (iflags & (DHN_6DIGITS))
		return 6;
	else if (v >= 10000)
		return 5;
	else if (iflags & (DHN_5DIGITS))
		return 5;
	else if (v >= 1000)
		return 4;
	else if (iflags & (DHN_4DIGITS))
		return 4;
	else if (v >= 100)
		return 3;
	else if (iflags & (DHN_3DIGITS))
		return 3;
	else if (v >= 10)
		return 2;
	else if (iflags & (DHN_2DIGITS))
		return 2;
	else
		return 1;


	return (v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7 :
		(v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4 :
		(v >= 1000) ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0;
}

unsigned CHudZB3ScoreBoard::math_div(unsigned v, int iflags)
{
	int maxsize = 0;
	if (maxsize <= 0)
	{
		maxsize = 1;

		for (int num = 10; (v / num) > 0; num *= 10)
			maxsize++;
	}

	if (maxsize > 255)
		maxsize = 255;

	return maxsize;
}

int CHudZB3ScoreBoard::DrawTexturedNumbersTopRightAligned(const CTextureRef& tex, const wrect_t(&rect)[10], int iNumber, int x, int y, int iflags, int igaps, float scale, byte r, byte g, byte b, byte a, int Long)
{
	assert(iNumber >= 0);

	int iRight = x;

	int iTime = Long ? CHudZB3ScoreBoard::math_log10_long(iNumber, iflags) : CHudZB3ScoreBoard::math_log10(iNumber, iflags);

	if (iNumber >= 10000)
	{
		iTime = CHudZB3ScoreBoard::math_log10_long(iNumber, iflags);
	}
	else
	{
		iTime = CHudZB3ScoreBoard::math_log10(iNumber, iflags);
	}
	for (int i = 1; i <= iTime; i++)
	{
		int k = iNumber % 10;
		iNumber /= 10;

		CHudZB3ScoreBoard::DrawTexturePart(tex, rect[k], x, y, scale, r, g, b, a);
		x -= (rect[k].right - rect[k].left + igaps) * scale;
	}

	iRight += (rect[0].right - rect[0].left + igaps) * scale;	//caculate length should reduce one more times

	return iRight;
}

void CHudZB3ScoreBoard::DrawTexturePart(const CTextureRef& tex, const wrect_t& rect, int x1, int y1, float scale, byte r, byte g, byte b, byte a)
{
	float w = tex.w();
	float h = tex.h();

	int x2 = x1 + (rect.right - rect.left) * scale;
	int y2 = y1 + (rect.bottom - rect.top) * scale;

	tex.Draw2DQuadScaled(x1, y1, x2, y2, rect.left / w, rect.top / h, rect.right / w, rect.bottom / h, r, g, b, a);
}

int CHudZB3ScoreBoard::DrawTexturedNumbers(const CTextureRef& tex, const wrect_t(&rect)[10], int iNumber, int x, int y, int iflags, int igaps, float scale, byte r, byte g, byte b, byte a, int iLong)
{


	int n;


	if (iNumber >= 10000)
	{
		n = CHudZB3ScoreBoard::math_log10_long(iNumber, iflags);
	}
	else
	{
		n = CHudZB3ScoreBoard::math_log10(iNumber, iflags);
	}

	if ((iflags & DHN_CENTERALIGNED))
		x += (rect[0].right - rect[0].left + igaps) * (n - 2) * scale;	//0.5?
	else
		x += (rect[0].right - rect[0].left + igaps) * (n - 1) * scale;

	return CHudZB3ScoreBoard::DrawTexturedNumbersTopRightAligned(tex, rect, iNumber, x, y, iflags, igaps, scale, r, g, b, a, iLong);
}

int CHudZB3ScoreBoard::DrawTexturedNumbersLong(const CTextureRef& tex, const wrect_t(&rect)[10], int iNumber, int x, int y, int iflags, int igaps, float scale, byte r, byte g, byte b, byte a)
{

	int maxsize = 0;
	if (maxsize <= 0)
	{
		maxsize = 1;

		for (int num = 10; (iNumber / num) > 0; num *= 10)
			maxsize++;
	}

	if (maxsize > 255)
		maxsize = 255;
	bool bShouldDraw = false;



	for (int i = 0; i < maxsize; i++)
	{
		int div = 1;
		for (int j = 0; j < maxsize - i; j++)
			div *= 10;

		int iNum = (iNumber % div * 10) / div;

		if (iNum)
			bShouldDraw = true;

		if (!iNum && !bShouldDraw && i != maxsize - 1)
			continue;


		if ((iflags & DHN_CENTERALIGNED))
			x += (rect[0].right - rect[0].left + igaps) * (maxsize - 2) * scale;	//0.5?
		else
			x += (rect[0].right - rect[0].left + igaps) * (maxsize - 1) * scale;

		CHudZB3ScoreBoard::DrawTexturedNumbersTopRightAligned(tex, rect, iNumber, x, y, iflags, igaps, scale, r, g, b, a);
	}
	return x;
}

int CHudZB3ScoreBoard::Draw(float flTime)
{
	if (g_iUser1 || gHUD.m_iModRunning == MOD_ZB4)
		return 1;

	int idx = gEngfuncs.GetLocalPlayer()->index;

	if (g_PlayerExtraInfo[idx].dead == true)
		return 1;

	if (m_iBGIndex == -1)
		return 1;

	HSPRITE bgSprite = gHUD.GetSprite(m_iBGIndex);
	wrect_t bgRect = gHUD.GetSpriteRect(m_iBGIndex);
	int bgHeight = (bgRect.bottom - bgRect.top);
	int bgWidth = (bgRect.right - bgRect.left);
	int bgY = 2;
	int bgX = (ScreenWidth - bgWidth) / 2;

	int aliveCT = gHUD.m_Scoreboard.m_iTeamAlive_CT;
	int aliveT = gHUD.m_Scoreboard.m_iTeamAlive_T;
	int textWidth_CTAlive = GetHudNumberWidth(m_iNum_S, m_rcNumber_Small, DHN_2DIGITS | DHN_DRAWZERO, aliveCT);
	int textWidth_TAlive = GetHudNumberWidth(m_iNum_S, m_rcNumber_Small, DHN_2DIGITS | DHN_DRAWZERO, aliveT);
	int scoreCT = gHUD.m_Scoreboard.m_iTeamScore_CT;
	int scoreT = gHUD.m_Scoreboard.m_iTeamScore_T;
	int scoreMax = gHUD.m_Scoreboard.m_iTeamScore_Max;
	int roundNumber = scoreMax ? scoreMax : scoreT + scoreCT + 1;

	if (gHUD.m_hudstyle->value == 2)
	{
		DrawNewHud(flTime);
		return 1;
	}

	if (gHUD.m_hudstyle->value == 1 && gHUD.m_iModRunning == MOD_NONE)
	{
		int x = ScreenWidth / 2;
		bgY = 5;
		int y = 5;
		const float flScale = 0.0f;

		m_pTexture_Board->Draw2DQuadScaled(x - 240 / 2, y, x + 240 / 2, y + 70);

		if (scoreCT >= 100)
		{
			int textWidth_CT = GetHudNumberWidth(m_iNum_L, m_rcNumber_Large, DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, scoreCT);

			if (textWidth_CT > 0)

				DrawHudNumber(m_iNum_L, m_rcNumber_Large, (ScreenWidth) / 2 - 39, bgY + 50, DHN_3DIGITS | DHN_DRAWZERO, scoreCT, 100, 134, 142);
		}
		else if (scoreCT >= 10)
		{
			int textWidth_CT = GetHudNumberWidth(m_iNum_L, m_rcNumber_Large, DHN_2DIGITS | DHN_DRAWZERO, scoreCT);

			if (textWidth_CT > 0)

				DrawHudNumber(m_iNum_L, m_rcNumber_Large, (ScreenWidth) / 2 - 39, bgY + 50, DHN_2DIGITS | DHN_DRAWZERO, scoreCT, 100, 134, 142);
		}
		else
		{
			int textWidth_CT = GetHudNumberWidth(m_iNum_L, m_rcNumber_Large, DHN_DRAWZERO, scoreCT);

			if (textWidth_CT > 0)

				DrawHudNumber(m_iNum_L, m_rcNumber_Large, (ScreenWidth) / 2 - 33, bgY + 50, DHN_DRAWZERO, scoreCT, 100, 134, 142);
		}

		if (scoreT >= 100)
		{
			int textWidth_T = GetHudNumberWidth(m_iNum_L, m_rcNumber_Large, DHN_3DIGITS | DHN_DRAWZERO, scoreT);

			if (textWidth_T > 0)
				DrawHudNumber(m_iNum_L, m_rcNumber_Large, ((ScreenWidth) / 2) + 39 - (textWidth_T / 2), bgY + 50, DHN_3DIGITS | DHN_DRAWZERO, scoreT, 172, 154, 111);
		}
		else if (scoreT >= 10)
		{
			int textWidth_T = GetHudNumberWidth(m_iNum_L, m_rcNumber_Large, DHN_2DIGITS | DHN_DRAWZERO, scoreT);

			if (textWidth_T > 0)
				DrawHudNumber(m_iNum_L, m_rcNumber_Large, ((ScreenWidth) / 2) + 39 - textWidth_T, bgY + 50, DHN_2DIGITS | DHN_DRAWZERO, scoreT, 172, 154, 111);
		}
		else
		{
			int textWidth_T = GetHudNumberWidth(m_iNum_L, m_rcNumber_Large, DHN_DRAWZERO, scoreT);

			if (textWidth_T > 0)
				DrawHudNumber(m_iNum_L, m_rcNumber_Large, ((ScreenWidth) / 2) + 39 - textWidth_T, bgY + 50, DHN_DRAWZERO, scoreT, 172, 154, 111);
		}

		textWidth_CTAlive = GetHudNumberWidth(m_iNum_csgo, m_rcNumber_csgo, DHN_2DIGITS | DHN_DRAWZERO, aliveCT);
		textWidth_TAlive = GetHudNumberWidth(m_iNum_csgo, m_rcNumber_csgo, DHN_2DIGITS | DHN_DRAWZERO, aliveT);
		if (textWidth_TAlive > 0)
		{
			if (aliveCT >= 10)
				DrawHudNumber(m_iNum_csgo, m_rcNumber_csgo, (ScreenWidth) / 2 - 111, bgY + 20, DHN_2DIGITS | DHN_DRAWZERO, aliveCT, 128 * 255, 128 * 255, 128 * 255);
			else
				DrawHudNumber(m_iNum_csgo, m_rcNumber_csgo, (ScreenWidth) / 2 - 100, bgY + 20, DHN_DRAWZERO, aliveCT, 128 * 255, 128 * 255, 128 * 255);
		}

		if (textWidth_CTAlive > 0)
		{
			if (aliveT >= 10)
				DrawHudNumber(m_iNum_csgo, m_rcNumber_csgo, (ScreenWidth) / 2 + 111 - textWidth_TAlive, bgY + 20, DHN_2DIGITS | DHN_DRAWZERO, aliveT, 128 * 255, 128 * 255, 128 * 255);
			else
			{
				DrawHudNumber(m_iNum_csgo, m_rcNumber_csgo, (ScreenWidth) / 2 + 100 - 19, bgY + 20, DHN_DRAWZERO, aliveT, 128 * 255, 128 * 255, 128 * 255);
			}
		}

		return 1;
	}

	if (bgSprite)
	{
		SPR_Set(bgSprite, 255, 255, 255);
		SPR_DrawHoles(0, bgX, bgY, &bgRect);
	}

	HSPRITE textSprite = gHUD.GetSprite(m_iTextIndex);

	if (textSprite)
	{
		wrect_t textRect = gHUD.GetSpriteRect(m_iTextIndex);

		SPR_Set(textSprite, 128, 128, 128);
		SPR_DrawAdditive(0, (ScreenWidth - (textRect.right - textRect.left)) / 2, bgY + 29, &textRect);
	}

	HSPRITE textSprite_T = gHUD.GetSprite(m_iTTextIndex);

	if (textSprite_T)
	{
		wrect_t textRect = gHUD.GetSpriteRect(m_iTTextIndex);

		SPR_Set(textSprite_T, 128, 128, 128);
		SPR_DrawAdditive(0, (ScreenWidth) / 2 - 50, bgY + 11, &textRect);
	}

	HSPRITE textSprite_CT = gHUD.GetSprite(m_iCTTextIndex);

	if (textSprite_CT)
	{
		wrect_t textRect = gHUD.GetSpriteRect(m_iCTTextIndex);

		SPR_Set(textSprite_CT, 128, 128, 128);
		SPR_DrawAdditive(0, (ScreenWidth) / 2 + 27, bgY + 11, &textRect);
	}

	//Caculate HudNumber

	if (gHUD.m_iModRunning == MOD_DM)
	{
		int best_player = gHUD.m_Scoreboard.FindBestPlayer();
		scoreCT = g_PlayerExtraInfo[gEngfuncs.GetLocalPlayer()->index].frags;
		scoreT = best_player ? g_PlayerExtraInfo[best_player].frags : 0;

		roundNumber = scoreMax ? scoreMax : 0;
	}

	if (roundNumber >= 1000)
	{
		int textWidth = GetHudNumberWidth(m_iNum_S, m_rcNumber_Small, DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, roundNumber);

		if (textWidth > 0)
			DrawHudNumber(m_iNum_S, m_rcNumber_Small, (ScreenWidth - textWidth) / 2, bgY + 10, DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, roundNumber, 128, 128, 128);
	}
	else if (roundNumber >= 1000)
	{
		int textWidth = GetHudNumberWidth(m_iNum_L, m_rcNumber_Large, DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, roundNumber);

		if (textWidth > 0)
			DrawHudNumber(m_iNum_L, m_rcNumber_Large, (ScreenWidth - textWidth) / 2, bgY + 10, DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, roundNumber, 128, 128, 128);
	}
	else
	{
		int textWidth = GetHudNumberWidth(m_iNum_L, m_rcNumber_Large, DHN_2DIGITS | DHN_DRAWZERO, roundNumber);

		if (textWidth > 0)
			DrawHudNumber(m_iNum_L, m_rcNumber_Large, (ScreenWidth - textWidth) / 2, bgY + 10, DHN_2DIGITS | DHN_DRAWZERO, roundNumber, 128, 128, 128);
	}

	if (scoreT >= 1000)
	{
		int textWidth_T = GetHudNumberWidth(m_iNum_S, m_rcNumber_Small, DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, scoreT);

		if (textWidth_T > 0)
			DrawHudNumber(m_iNum_S, m_rcNumber_Small, (ScreenWidth) / 2 - 90, bgY + 11, DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, scoreT, 128, 128, 128);
	}
	else if (scoreT >= 100)
	{
		int textWidth_T = GetHudNumberWidth(m_iNum_L, m_rcNumber_Large, DHN_3DIGITS | DHN_DRAWZERO, scoreT);

		if (textWidth_T > 0)
			DrawHudNumber(m_iNum_L, m_rcNumber_Large, (ScreenWidth) / 2 - 89, bgY + 10, DHN_3DIGITS | DHN_DRAWZERO, scoreT, 128, 128, 128);
	}
	else
	{
		int textWidth_T = GetHudNumberWidth(m_iNum_L, m_rcNumber_Large, DHN_2DIGITS | DHN_DRAWZERO, scoreT);

		if (textWidth_T > 0)
			DrawHudNumber(m_iNum_L, m_rcNumber_Large, (ScreenWidth) / 2 - 89, bgY + 10, DHN_2DIGITS | DHN_DRAWZERO, scoreT, 128, 128, 128);
	}

	if (scoreCT >= 1000)
	{
		int textWidth_CT = GetHudNumberWidth(m_iNum_S, m_rcNumber_Small, DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, scoreCT);

		if (textWidth_CT > 0)
			DrawHudNumber(m_iNum_S, m_rcNumber_Small, ((ScreenWidth) / 2) + 71 - (textWidth_CT / 2), bgY + 11, DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, scoreCT, 128, 128, 128);
	}
	else if (scoreCT >= 100)
	{
		int textWidth_CT = GetHudNumberWidth(m_iNum_L, m_rcNumber_Large, DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, scoreCT);

		if (textWidth_CT > 0)
			DrawHudNumber(m_iNum_L, m_rcNumber_Large, ((ScreenWidth) / 2) + 70 - (textWidth_CT / 2), bgY + 10, DHN_3DIGITS | DHN_DRAWZERO, scoreCT, 128, 128, 128);
	}
	else
	{
		int textWidth_CT = GetHudNumberWidth(m_iNum_L, m_rcNumber_Large, DHN_2DIGITS | DHN_DRAWZERO, scoreCT);

		if (textWidth_CT > 0)
			DrawHudNumber(m_iNum_L, m_rcNumber_Large, ((ScreenWidth) / 2) + 73 - (textWidth_CT / 2), bgY + 10, DHN_2DIGITS | DHN_DRAWZERO, scoreCT, 128, 128, 128);
	}

	if (m_iBGIndex != m_iTeamDeathBG)
	{
		if (textWidth_TAlive > 0)
			DrawHudNumber(m_iNum_S, m_rcNumber_Small, (ScreenWidth) / 2 - 69, bgY + 30, DHN_2DIGITS | DHN_DRAWZERO, aliveT, 128, 128, 128);

		if (textWidth_CTAlive > 0)
			DrawHudNumber(m_iNum_S, m_rcNumber_Small, (ScreenWidth) / 2 + 47, bgY + 30, DHN_2DIGITS | DHN_DRAWZERO, aliveCT, 128, 128, 128);
	}

	return 1;
}

int CHudZB3ScoreBoard::DrawNewHud(float flTime)
{
	int iX = 0, iY = 0;
	int iW, iH;
	if (gHUD.m_iModRunning == MOD_NONE)
	{
		iW = m_iScoreboardBG->w();
		iH = m_iScoreboardBG->h();

		iX = (ScreenWidth - iW) / 2;

		m_iScoreboardBG->Draw2DQuadScaled(iX, iY, iX + iW, iY + iH);

		iY += 10;
		iX += 105;
		iW = m_iIcon_TR_Right->w();
		iH = m_iIcon_TR_Right->h();

		m_iIcon_TR_Right->Draw2DQuadScaled(iX - 5 - iW, iY, iX - 5 - iW + iW, iY + iH);

		iX = ScreenWidth / 2 + (ScreenWidth / 2 - (iX - 5 - iW + iW));
		iW = m_iIcon_CT_Left->w();
		iH = m_iIcon_CT_Left->h();

		m_iIcon_CT_Left->Draw2DQuadScaled(iX, iY, iX + iW, iY + iH);

		iW = m_iIcon_Win_Center->w();
		iY += iH;
		iH = m_iIcon_Win_Center->h();
		iX = (ScreenWidth - iW) / 2;
		m_iIcon_Win_Center->Draw2DQuadScaled(iX, iY, iX + iW, iY + iH);

		iY += iH + 2;
		iX = ScreenWidth / 2 + 2;
		int roundNumber = gHUD.m_Scoreboard.m_iTeamScore_T + gHUD.m_Scoreboard.m_iTeamScore_CT + 1;
		DrawTexturedNumbers(*m_iNum_Center, m_iNum_CenterC, roundNumber, iX, iY, DHN_2DIGITS | DHN_CENTERALIGNED, 4);

		iX = ScreenWidth / 2 - 93;
		iY = m_iIcon_TR_Right->h() + 15;

		int iRight;
		iRight = DrawTexturedNumbers(*m_iNum_BigRed, m_iNum_BigRedC, gHUD.m_Scoreboard.m_iTeamScore_T, iX, iY, DHN_2DIGITS, 3); // ok

		iX = ScreenWidth / 2 + 93 - (iRight - iX) + 3;
		DrawTexturedNumbers(*m_iNum_BigBlue, m_iNum_BigBlueC, gHUD.m_Scoreboard.m_iTeamScore_CT, iX, iY, DHN_2DIGITS, 3); // ok

		iX = ScreenWidth / 2 - 75;
		iY = 60;
		iRight = DrawTexturedNumbers(*m_iNum_SmallRed, m_iNum_SmallRedC, gHUD.m_Scoreboard.m_iTeamAlive_T, iX, iY, DHN_2DIGITS, 1);


		iX = ScreenWidth / 2 + 75 - (iRight - iX) + 1;
		DrawTexturedNumbers(*m_iNum_SmallBlue, m_iNum_SmallBlueC, gHUD.m_Scoreboard.m_iTeamAlive_CT, iX, iY, DHN_2DIGITS, 1);
	}
	else if (gHUD.m_iModRunning == MOD_ZB1 || gHUD.m_iModRunning == MOD_ZB2 || gHUD.m_iModRunning == MOD_ZB3)
	{
		iW = m_iScoreboardBG_GD->w();
		iH = m_iScoreboardBG_GD->h();

		iX = (ScreenWidth - iW) / 2;

		m_iScoreboardBG_GD->Draw2DQuadScaled(iX, iY, iX + iW, iY + iH);

		iY += 10;
		iX += 105;
		iW = m_iIcon_ZB_Right->w();
		iH = m_iIcon_ZB_Right->h();

		m_iIcon_ZB_Right->Draw2DQuadScaled(iX - 5 - iW, iY, iX - 5 - iW + iW, iY + iH);

		iX = ScreenWidth / 2 + (ScreenWidth / 2 - (iX - 5 - iW + iW));
		iW = m_iIcon_HM_Left->w();
		iH = m_iIcon_HM_Left->h();

		m_iIcon_HM_Left->Draw2DQuadScaled(iX, iY, iX + iW, iY + iH);


		iW = m_iIcon_Round->w();
		iY += iH;
		iH = m_iIcon_Round->h();
		iX = (ScreenWidth - iW) / 2;
		m_iIcon_Round->Draw2DQuadScaled(iX, iY, iX + iW, iY + iH);

		iY += iH + 2;

		iW = m_iSlash_Center->w();
		iH = m_iSlash_Center->h();
		iX = (ScreenWidth - iW) / 2;
		m_iSlash_Center->Draw2DQuadScaled(iX, iY, iX + iW, iY + iH);

		iX = ScreenWidth / 2 - 32;
		int roundmax = gHUD.m_Scoreboard.m_iTeamScore_Max;
		int roundNumber = gHUD.m_Scoreboard.m_iTeamScore_T + gHUD.m_Scoreboard.m_iTeamScore_CT;
		DrawTexturedNumbers(*m_iNum_Center, m_iNum_CenterC, roundNumber, iX, iY, DHN_2DIGITS, 2);
		iX = ScreenWidth / 2 + 8;
		DrawTexturedNumbers(*m_iNum_Center, m_iNum_CenterC, roundmax, iX, iY, DHN_2DIGITS, 2);

		iX = ScreenWidth / 2 - 93;
		iY = m_iIcon_TR_Right->h() + 15;

		int iRight;
		iRight = DrawTexturedNumbers(*m_iNum_BigRed, m_iNum_BigRedC, gHUD.m_Scoreboard.m_iTeamScore_T, iX, iY, DHN_2DIGITS, 3); // ok

		iX = ScreenWidth / 2 + 93 - (iRight - iX) + 3;
		DrawTexturedNumbers(*m_iNum_BigBlue, m_iNum_BigBlueC, gHUD.m_Scoreboard.m_iTeamScore_CT, iX, iY, DHN_2DIGITS, 3); // ok

		iX = ScreenWidth / 2 - 75;
		iY = 60;
		iRight = DrawTexturedNumbers(*m_iNum_SmallRed, m_iNum_SmallRedC, gHUD.m_Scoreboard.m_iTeamAlive_T, iX, iY, DHN_2DIGITS, 1);


		iX = ScreenWidth / 2 + 75 - (iRight - iX) + 1;
		DrawTexturedNumbers(*m_iNum_SmallBlue, m_iNum_SmallBlueC, gHUD.m_Scoreboard.m_iTeamAlive_CT, iX, iY, DHN_2DIGITS, 1);


	}
	else if (gHUD.m_iModRunning == MOD_DM)
	{
		iW = m_iScoreboardBG->w();
		iH = m_iScoreboardBG->h();

		iX = (ScreenWidth - iW) / 2;

		m_iScoreboardBG_ZBG->Draw2DQuadScaled(iX, iY, iX + iW, iY + iH);

		iY += 10;
		iX += 105;
		iW = m_iIcon_1st->w();
		iH = m_iIcon_1st->h();

		m_iIcon_1st->Draw2DQuadScaled(iX - 5 - iW, iY, iX - 5 - iW + iW, iY + iH);

		iX = ScreenWidth / 2 + (ScreenWidth / 2 - (iX - 5 - iW + iW));
		iW = m_iIcon_My->w();
		iH = m_iIcon_My->h();

		m_iIcon_My->Draw2DQuadScaled(iX, iY, iX + iW, iY + iH);

		iW = m_iIcon_KillC->w();
		iY += iH;
		iH = m_iIcon_KillC->h();
		iX = (ScreenWidth - iW) / 2;
		m_iIcon_KillC->Draw2DQuadScaled(iX, iY, iX + iW, iY + iH);

		iY += iH + 2;
		iX = ScreenWidth / 2 + 2;
		int iSBTopCenter = gHUD.m_Scoreboard.m_iTeamScore_Max ? gHUD.m_Scoreboard.m_iTeamScore_Max : 0;
		if (iSBTopCenter > 100)
			DrawTexturedNumbers(*m_iNum_Center, m_iNum_CenterC, iSBTopCenter, iX - 19, iY, 0, 1);
		else
			DrawTexturedNumbers(*m_iNum_Center, m_iNum_CenterC, iSBTopCenter, iX, iY, DHN_2DIGITS | DHN_CENTERALIGNED, 1);

		iX = ScreenWidth / 2 - 93;
		iY = m_iIcon_My->h() + 15;

		int best_player = gHUD.m_Scoreboard.FindBestPlayer();
		int iSBTopRight = g_PlayerExtraInfo[gEngfuncs.GetLocalPlayer()->index].frags;
		int iSBTopLeft = best_player ? g_PlayerExtraInfo[best_player].frags : 0;

		int iRight;
		iRight = DrawTexturedNumbers(*m_iNum_BigWhite, m_iNum_BigWhiteC, iSBTopLeft, iX, iY, DHN_2DIGITS, 3); // ok

		iX = ScreenWidth / 2 + 93 - (iRight - iX) + 3;
		DrawTexturedNumbers(*m_iNum_BigWhite, m_iNum_BigWhiteC, iSBTopRight, iX, iY, DHN_2DIGITS, 3); // ok
	}
	else if (gHUD.m_iModRunning == MOD_TDM)
	{
		iW = m_iScoreboardBG->w();
		iH = m_iScoreboardBG->h();

		iX = (ScreenWidth - iW) / 2;

		m_iScoreboardBG->Draw2DQuadScaled(iX, iY, iX + iW, iY + iH);

		iY += 10;
		iX += 105;
		iW = m_iIcon_TR_Right->w();
		iH = m_iIcon_TR_Right->h();

		m_iIcon_TR_Right->Draw2DQuadScaled(iX - 5 - iW, iY, iX - 5 - iW + iW, iY + iH);

		iX = ScreenWidth / 2 + (ScreenWidth / 2 - (iX - 5 - iW + iW));
		iW = m_iIcon_CT_Left->w();
		iH = m_iIcon_CT_Left->h();

		m_iIcon_CT_Left->Draw2DQuadScaled(iX, iY, iX + iW, iY + iH);

		iW = m_iIcon_TeamKill->w();
		iY += iH;
		iH = m_iIcon_TeamKill->h();
		iX = (ScreenWidth - iW) / 2;
		m_iIcon_TeamKill->Draw2DQuadScaled(iX, iY, iX + iW, iY + iH);

		iY += iH + 2;
		iX = ScreenWidth / 2 + 2;
		int iSBTopCenter = gHUD.m_Scoreboard.m_iTeamScore_Max ? gHUD.m_Scoreboard.m_iTeamScore_Max : 0;
		if (iSBTopCenter > 100)
			DrawTexturedNumbers(*m_iNum_Center, m_iNum_CenterC, iSBTopCenter, iX - 19, iY, 0, 1);
		else
			DrawTexturedNumbers(*m_iNum_Center, m_iNum_CenterC, iSBTopCenter, iX, iY, DHN_2DIGITS | DHN_CENTERALIGNED, 1);

		int iSBTopLeft = gHUD.m_Scoreboard.m_iTeamScore_T;
		iX = ScreenWidth / 2 - (iSBTopLeft > 100 ? 106 : 93);
		iY = m_iIcon_TR_Right->h() + 15;
		if (iSBTopLeft > 100)
			DrawTexturedNumbers(*m_iNum_BigRed, m_iNum_BigRedC, iSBTopLeft, iX, iY, 0, 3); // ok
		else
			DrawTexturedNumbers(*m_iNum_BigRed, m_iNum_BigRedC, iSBTopLeft, iX, iY, DHN_2DIGITS, 3); // ok


		int iSBTopRight = gHUD.m_Scoreboard.m_iTeamScore_CT;
		int iLength = iSBTopRight > 100 ? 93 : 42;
		iX = ScreenWidth / 2 + (iSBTopRight > 100 ? 106 : 93) - iLength + 3;	//- length + gap

		if (iSBTopRight > 100)
			DrawTexturedNumbers(*m_iNum_BigBlue, m_iNum_BigBlueC, iSBTopRight, iX, iY, 0, 3); // ok
		else
			DrawTexturedNumbers(*m_iNum_BigBlue, m_iNum_BigBlueC, iSBTopRight, iX, iY, DHN_2DIGITS, 3); // ok


		//survival player
		iX = ScreenWidth / 2 - 75;
		iY = 60;
		int iRight = DrawTexturedNumbers(*m_iNum_SmallRed, m_iNum_SmallRedC, gHUD.m_Scoreboard.m_iTeamAlive_T, iX, iY, DHN_2DIGITS, 1);


		iX = ScreenWidth / 2 + 75 - (iRight - iX) + 1;
		DrawTexturedNumbers(*m_iNum_SmallBlue, m_iNum_SmallBlueC, gHUD.m_Scoreboard.m_iTeamAlive_CT, iX, iY, DHN_2DIGITS, 1);
	}
	return 1;
}

void CHudZB3ScoreBoard::Reset(void)
{
	m_iBGIndex = m_iOriginalBG;
	m_iTextIndex = m_iText_Round;
	m_iTTextIndex = m_iText_TR;
	m_iCTTextIndex = m_iText_CT;

	switch (gHUD.m_iModRunning)
	{
	case MOD_NONE:
	{
		// nothing to change
		break;
	}
	case MOD_TDM:
	{
		m_iBGIndex = m_iTeamDeathBG;
		m_iTextIndex = m_iText_Kill;
		break;
	}
	case MOD_DM:
	{
		m_iBGIndex = m_iTeamDeathBG;
		m_iTextIndex = m_iText_Kill;
		m_iTTextIndex = m_iText_1st;
		m_iCTTextIndex = m_iText_Kill;
		break;
	}
	case MOD_ZB3:
	case MOD_ZB2:
	case MOD_ZB1:
	{
		m_iTTextIndex = m_iText_ZB;
		m_iCTTextIndex = m_iText_HM;
		break;
	}
	case MOD_ZBS:
	{
		// already drawn in zbs/zbs_scoreboard.cpp
		m_iBGIndex = -1;
		break;
	}
	default:
	{
		// shut clang warnings
		break;
	}
	}
}


void CHudZB3ScoreBoard::BuildHudNumberRect(int moe, wrect_t* prc, int w, int h, int xOffset, int yOffset)
{
	wrect_t rc = gHUD.GetSpriteRect(moe);
	int x = rc.left;
	int y = rc.top;

	for (int i = 0; i < 10; i++)
	{
		prc[i].left = x;
		prc[i].top = 0;
		prc[i].right = prc[i].left + w + xOffset;
		prc[i].bottom = h + yOffset;

		x += w;
		y += h;
	}
}

int CHudZB3ScoreBoard::DrawHudNumber(int moe, wrect_t* prc, int x, int y, int iFlags, int iNumber, int r, int g, int b)
{
	int iWidth = prc[0].right - prc[0].left;
	int k;
	wrect_t rc;

	if (iNumber >= 10000)
	{
		k = iNumber / 10000;
		SPR_Set(gHUD.GetSprite(moe), r, g, b);
		SPR_DrawAdditive(0, x, y, &prc[k]);
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS))
	{
		if (iFlags & DHN_DRAWZERO)
		{
			SPR_Set(gHUD.GetSprite(moe), r, g, b);
			SPR_DrawAdditive(0, x, y, &prc[0]);
		}

		x += iWidth;
	}

	if (iNumber >= 1000)
	{
		k = (iNumber % 10000) / 1000;
		SPR_Set(gHUD.GetSprite(moe), r, g, b);
		SPR_DrawAdditive(0, x, y, &prc[k]);
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS))
	{
		if (iFlags & DHN_DRAWZERO)
		{
			SPR_Set(gHUD.GetSprite(moe), r, g, b);
			SPR_DrawAdditive(0, x, y, &prc[0]);
		}

		x += iWidth;
	}

	if (iNumber >= 100)
	{
		k = (iNumber % 1000) / 100;
		SPR_Set(gHUD.GetSprite(moe), r, g, b);
		SPR_DrawAdditive(0, x, y, &prc[k]);
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS))
	{
		if (iFlags & DHN_DRAWZERO)
		{
			SPR_Set(gHUD.GetSprite(moe), r, g, b);
			SPR_DrawAdditive(0, x, y, &prc[0]);
		}

		x += iWidth;
	}

	if (iNumber >= 10)
	{
		k = (iNumber % 100) / 10;
		rc = prc[k];
		SPR_Set(gHUD.GetSprite(moe), r, g, b);
		SPR_DrawAdditive(0, x, y, &rc);
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS))
	{
		if (iFlags & DHN_DRAWZERO)
		{
			SPR_Set(gHUD.GetSprite(moe), r, g, b);
			SPR_DrawAdditive(0, x, y, &prc[0]);
		}

		x += iWidth;
	}

	k = iNumber % 10;
	SPR_Set(gHUD.GetSprite(moe), r, g, b);
	SPR_DrawAdditive(0, x, y, &prc[k]);
	x += iWidth;

	return x;
}

int CHudZB3ScoreBoard::GetHudNumberWidth(int moe, wrect_t* prc, int iFlags, int iNumber)
{
	int iWidth = prc[0].right - prc[0].left;
	int k;
	wrect_t rc;
	int x = 0;

	if (iNumber >= 10000)
	{
		k = iNumber / 10000;
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS))
		x += iWidth;

	if (iNumber >= 1000)
	{
		k = (iNumber % 10000) / 1000;
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS))
		x += iWidth;

	if (iNumber >= 100)
	{
		k = (iNumber % 1000) / 100;
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS))
		x += iWidth;

	if (iNumber >= 10)
	{
		k = (iNumber % 100) / 10;
		rc = prc[k];
		x += iWidth;
	}
	else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS))
		x += iWidth;

	k = iNumber % 10;
	x += iWidth;

	return x;
}*/