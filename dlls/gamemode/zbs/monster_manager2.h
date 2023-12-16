#ifndef MONSTER_MANAGER2_H
#define MONSTER_MANAGER2_H
#ifdef _WIN32
#pragma once
#endif

#include <memory>
#include <functional>

class CMonsterManager2
{
private:
	CMonsterManager2(); // singleton creator
	friend inline CMonsterManager2& MonsterManager2()
	{
		static CMonsterManager2 x;
		return x;
	}

public:
	~CMonsterManager2();

private:
	friend class CMonster2;
	void OnEntityAdd(CMonster2* ent);
	void OnEntityRemove(CMonster2* ent);

public:
	size_t EntityCount();
	void EntityForEach(std::function<void(CMonster2*)> functor);

public: // max monster num settings
	void SetMaxNumOfEntity(size_t what);
	size_t GetMaxNumOfEntity();
	void SetAutoGcRatio(float what);

private:
	class impl_t;
	std::unique_ptr<impl_t> pimpl;

};

inline CMonsterManager2& MonsterManager2();

#endif
