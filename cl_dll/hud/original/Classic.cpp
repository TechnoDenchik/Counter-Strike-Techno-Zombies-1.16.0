#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "parsemsg.h"

#include "Classic.h"
#include "hud_sub_impl.h"
#include "player/player_const.h"
#include "gamemode/zb2/zb2_const.h"
#include "Wins.h"

#include <vector>

class CHudClassic_impl_t : public THudSubDispatcher<CHudWinOriginal>
{
public:
};

DECLARE_MESSAGE(m_CLS, CLSMsg)

int CHudClassic::MsgFunc_CLSMsg(const char* pszName, int iSize, void* pbuf)
{
	BufferReader buf(pszName, pbuf, iSize);

	auto type = static_cast<ZB2MessageType>(buf.ReadByte());
	switch (type)
	{
	case ORIG_WINCT:
	{
		pimpl->get<CHudWinOriginal>().winct();
		break;
	}
	case ORIG_WINT:
	{
		pimpl->get<CHudWinOriginal>().wint();
		break;
	}
	}
	return 1;
}

int CHudClassic::Init()
{
	pimpl = new CHudClassic_impl_t;

	gHUD.AddHudElem(this);

	HOOK_MESSAGE(CLSMsg);

	return 1;
}

int CHudClassic::VidInit()
{
	pimpl->for_each(&IBaseHudSub::VidInit);

	return 1;
}

int CHudClassic::Draw(float time)
{
	pimpl->for_each(&IBaseHudSub::Draw, time);
	return 1;
}

void CHudClassic::Think()
{
	pimpl->for_each(&IBaseHudSub::Think);
}

void CHudClassic::Reset()
{
	pimpl->for_each(&IBaseHudSub::Reset);
}

void CHudClassic::InitHUDData()
{
	pimpl->for_each(&IBaseHudSub::InitHUDData);
}

void CHudClassic::Shutdown()
{
	delete pimpl;
	pimpl = nullptr;
}