/* =================================================================================== *
	  * =================== TechnoSoftware & MoeMod Developing =================== *
 * =================================================================================== */

#include "hud.h"
#include "cl_util.h"
#include "cl_entity.h"
#include "triangleapi.h"
#include "draw_util.h"
#include "com_model.h"
#include "calcscreen.h"

int CHudHeadName::Init(void)
{
	gHUD.AddHudElem(this);

	m_iFlags |= HUD_DRAW;

	return 1;
}

int CHudHeadName::VidInit(void)
{
	if (!ourforces)
		ourforces = R_LoadTextureShared("resource/zb3/ourforces");
	return 1;
}

bool CHudHeadName::CheckForPlayer(cl_entity_s *pEnt)
{
	if (pEnt && pEnt->model && pEnt->model->name[0] && pEnt->player)
		return true;

	return false;
}

int CHudHeadName::Draw(float flTime)
{
	if ((gHUD.m_iHideHUDDisplay & HIDEHUD_ALL) || g_iUser1)
		return 1;

	for (int i = 1; i < 33; i++)
	{
		if (g_PlayerExtraInfo[i].dead)
			continue;

		if (g_PlayerExtraInfo[i].teamnumber != g_PlayerExtraInfo[gHUD.m_Scoreboard.m_iPlayerNum].teamnumber)
			continue;

		if (i != gHUD.m_Scoreboard.m_iPlayerNum)
		{
			if(gHUD.cl_headname->value == 1)
			{ 
				cl_entity_t *ent = gEngfuncs.GetEntityByIndex(i);

				if (!CheckForPlayer(ent))
					continue;

				model_t *model = ent->model;
				vec3_t origin = ent->origin;

				if (model)
					origin.z += max(model->maxs.z, 35.0);

				float screen[2]{ -1,-1 };
				if (!CalcScreen(origin, screen))
					continue;

				int textlen = DrawUtils::HudStringLen(g_PlayerInfoList[i].name);

				DrawUtils::DrawHudString(screen[0] - textlen * 0.5f, screen[1], gHUD.m_scrinfo.iWidth, g_PlayerInfoList[i].name, 150, 150, 150);
			}

			if (gHUD.cl_headname->value == 2)
			{
				cl_entity_t* ent = gEngfuncs.GetEntityByIndex(i);

				if (!CheckForPlayer(ent))
					continue;

				vec3_t origin = ent->origin;
				origin.z += 33.0;

				float xyScreen[2];
				if (CalcScreen(origin, xyScreen))
				{
					gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
					gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
					ourforces->Bind();
					DrawUtils::Draw2DQuadScaled(xyScreen[0] - 13, xyScreen[1] - 8, xyScreen[0] + 13, xyScreen[1] + 8);
				}
			}
		}
	}
	return 1;
}