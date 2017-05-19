#include "Death.h"
#include <Run.h>
#include <Character.h>
void Death::Enter(Character * pEnemy)
{
	pEnemy->getNode()->setCurrentAnimation("muerte");
	
}

void Death::Exit(Character * pEnemy)
{
	
}

void Death::Execute(Character * pCharacter)
{
	//Muero y pongo animacion de correr
	if (!pCharacter->getLifeComponent()->isDying()) {
		pCharacter->getAnimationMachine()->ChangeState(&Run::i());
	}
}

Death::~Death()
{
}
