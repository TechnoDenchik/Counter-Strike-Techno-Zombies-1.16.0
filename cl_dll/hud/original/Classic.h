#pragma once

class CHudClassic : public CHudBase
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
	bool m_bCanDraw;
	CHudMsgFunc(CLSMsg);

public:
	class CHudClassic_impl_t* pimpl = nullptr;
};