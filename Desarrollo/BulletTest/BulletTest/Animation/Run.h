#pragma once

#include "AnimationCharacter.h"

class Run :
	public AnimationCharacter
{
public:

	static Run& i() {
		static Run singleton;
		return singleton;
	}	


	virtual void Enter(Character* pEnemy);
	virtual void Exit(Character* pEnemy);
	virtual void Execute(Character* pCharacter);

	virtual std::string getAnimationName() { return "Run"; }


	~Run();
};

