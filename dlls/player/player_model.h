#ifndef PLAYER_MODEL_H
#define PLAYER_MODEL_H
#ifdef _WIN32
#pragma once
#endif

#define	SHOW_DAMAGE (1 << 0)
#define	SHOW_SPEED (1 << 1)
#define	SHOW_ACCSHOOT_FIRST (1 << 2)
#define	SHOW_TRACE_LOW_HEALTH (1 << 3)
#define	SHOW_LAST (1 << 4)


const char *Client_ApperanceToModel(int iApperance);

void PlayerModel_Precache();
void PlayerModel_ForceUnmodified(const Vector &vMin, const Vector &vMax);

int PlayerModel_GetNumSkins();
ModelName PlayerModel_GetRandomSkin();
const char* PlayerModel_GetApperance(int iApperance, TeamName iTeam);
bool PlayerModel_IsFemale(int iApperance, TeamName iTeam);

class CPlayerClassManager
{
public:
	struct ClassData
	{
		int ClassID;
		const char* model_name;
		TeamName team;
		bool isFemale;
		int m_iBitsShowState;
		int HandTexid[6];
	};
	void PlayerModel_Precache();
	const char* PlayerClass_GetModelName(int iApperance);
	bool PlayerClass_IsFemale(int iApperance);
	void Client_ApperanceToModel(char* buffer, int iApperance);
	int Client_ModelToApperance(const char* modelname);
	int PlayerClass_GetNumClass();
	int PlayerClass_GetNumCT();
	int PlayerClass_GetNumTR();
	ModelName PlayerClass_GetRandomClass();

	void SetPlayerClass(int index, char* name);

	inline ClassData& GetPlayerClass(int idx)
	{
		if (idx > 33 || idx < 0)
			return m_NullClass;

		if (!m_PlayerClass[idx])
			return m_NullClass;
		return *m_PlayerClass[idx];
	}

	ClassData* m_PlayerClass[33];
	ClassData m_NullClass;
};

CPlayerClassManager& PlayerClassManager();



#endif