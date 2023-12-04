#ifndef MOD_BASE_H
#define MOD_BASE_H
#ifdef _WIN32
#pragma once
#endif


#include "mods.h"
#include "ruleof350.h"
#include "monsters.h"
#include <memory>

class CBaseEntity; // cbase.h
class CBasePlayer; // player.h
typedef struct entvars_s entvars_t; // progdefs.h
	
class CMod_ZombieShelter_coop : public CHalfLifeMultiplay, ruleof350::unique
{
public:
	CMod_ZombieShelter_coop();
	base_shelter();


private:

protected:


};