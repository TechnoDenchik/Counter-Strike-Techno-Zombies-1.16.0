#pragma once

class CHudZSH : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float time);
	void Think(void);
	void Reset(void);
	void InitHUDData(void);		// called every time a server is connected to
	void Shutdown(void);
	CHudMsgFunc(ZSHMsg);
	//class CHudZSH_impl_t* pimpl = nullptr;
	
protected:
	class impl_t;
	impl_t* pimpl;
};

//CHudZSH zsh_pve;