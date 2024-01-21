/*
zb2.h - CSMoE Client HUD : Zombie Mod 2
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

#pragma once

class CHudZB2 : public CHudBase
{
public:
	int Init() override;
	int VidInit() override;
	int Draw(float time) override;
	void Think() override;
	void Reset() override;
	void InitHUDData() override;		// called every time a server is connected to
	void Shutdown() override;
	void ClearSelector();
	void SetSelectorDrawTime(float flTime, float flDisplayTime);
	void SetSelectorIcon(int slot, const char* name);
	void SetSelectorIconBan(int slot);
	void SetSelectorIconLevel(int slot, int level);
	bool Selector(int i);
	bool SelectorCanDraw();
public:
	bool ActivateSkill(int iSlot);
	bool m_bCanDraw;
	CHudMsgFunc(ZB2Msg);
    //CHudMsgFunc(ZB2Win);

public:
	class CHudZB2_impl_t *pimpl = nullptr;
};