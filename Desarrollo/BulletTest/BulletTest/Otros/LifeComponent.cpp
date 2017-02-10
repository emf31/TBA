#include "LifeComponent.h"
#include <Player.h>
#include <Cliente.h>
#include <Map.h>

LifeComponent::LifeComponent(Entity * owner) 
	: m_pOwner(owner), m_isDying(false), m_vida(100)
{
}

LifeComponent::~LifeComponent()
{
}
void LifeComponent::restaVida(float cantidad, RakNet::RakNetGUID guid)
{
	m_vida -= cantidad;
	if (m_vida <= 0 && m_isDying == false) {

		m_isDying = true;
		relojMuerte.restart();

		if (Cliente::i().isConected()) {

			TPlayer nuevoplayer;
			nuevoplayer.position = m_pOwner->getRenderState()->getPosition();
			nuevoplayer.guid = m_pOwner->getGuid();
			nuevoplayer.name = m_pOwner->getName();


			Cliente::i().dispatchMessage(nuevoplayer, MUERTE);


			TKill kill;
			kill.guidKill = guid;
			kill.guidDeath = m_pOwner->getGuid();

			Cliente::i().dispatchMessage(kill, ACTUALIZA_TABLA);
	
		}

	}
	
}

void LifeComponent::update()
{
	//Una vez termine la animacion de muerte, volvemos a movernos
	if (m_isDying && relojMuerte.getElapsedTime().asSeconds() > 3) {
		m_isDying = false;
		m_vida = 100;

		m_pOwner->setPosition(Map::i().searchSpawnPoint());
	}

}

void LifeComponent::resetVida()
{
	m_vida = 100;
}

float LifeComponent::getVida()
{
	return m_vida;
}

void LifeComponent::sumarVida()
{
	m_vida += 40;
	if (m_vida > 100) {
		m_vida = 100;
	}
}

bool LifeComponent::isDying()
{
	return m_isDying;
}
