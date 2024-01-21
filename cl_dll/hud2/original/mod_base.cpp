/*
zb2.cpp - CSMoE Client HUD : Zombie Mod 2
Copyright (C) 2019 Moemod Yanase

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"
#include "parsemsg.h"
#include "original/mod_base.h"
#include "winhud2.h"
#include "hud_sub_impl.h"
#include "gamemode/zb2/zb2_const.h"
#include <vector>

class CHudOriginal_impl_t : public THudSubDispatcher<CHudOrigWins>
{
public:
	std::vector<CHudRetina::MagicNumber> m_RetinaIndexes;
};

DECLARE_MESSAGE(m_ORIG, ORIGWin)

int CHudOriginal::MsgFunc_ORIGWin(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);
	auto type = static_cast<ZB2MessageType>(buf.ReadByte());
	switch (type)
	{
	case ORIG_WINCT:
	{
		pimpl->get<CHudOrigWins>().winsct();
		break;
	}
	case ORIG_WINT:
	{
		pimpl->get<CHudOrigWins>().winst();
		break;
	}
	}

	return 1;
}

int CHudOriginal::Init()
{
	pimpl = new CHudOriginal_impl_t;

	gHUD.AddHudElem(this);

	HOOK_MESSAGE(ORIGWin);

	return 1;
}

int CHudOriginal::VidInit()
{
	pimpl->for_each(&IBaseHudSub::VidInit);
	return 1;
}

int CHudOriginal::Draw(float time)
{
	pimpl->for_each(&IBaseHudSub::Draw, time);
	return 1;
}

void CHudOriginal::Think()
{
	pimpl->for_each(&IBaseHudSub::Think);
}

void CHudOriginal::Reset()
{
	pimpl->for_each(&IBaseHudSub::Reset);
}

void CHudOriginal::InitHUDData()
{
	pimpl->for_each(&IBaseHudSub::InitHUDData);
}

void CHudOriginal::Shutdown()
{
	delete pimpl;
	pimpl = nullptr;
}