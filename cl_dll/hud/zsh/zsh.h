#include "hud.h"
#include "followicon.h"
#include "cl_util.h"
#include "draw_util.h"
#include "triangleapi.h"

#include "parsemsg.h"

class CHudZSH : public CHudBase
{
public:
	int Init() override;
	int VidInit() override;
	int Draw(float time) override;
	void Think() override;
	void Reset() override;
	void InitHUDData() override;		// called every time a server is connected to
	void Shutdown() override;

public:
	bool ActivateSkill(int iSlot);
	CHudMsgFunc(ZSHMsg);

public:
	class CHudZSH_impl_t* pimpl = nullptr;
};
