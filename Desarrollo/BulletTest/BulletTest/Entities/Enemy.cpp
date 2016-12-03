
#include "Enemy.h"
#include "../Motor/PhysicsEngine.h"
#include "../Motor/GraphicEngine.h"
#include "../Motor de Red/Estructuras.h"
#include "../Motor de Red/Cliente.h"
#include "../Entities/EntityManager.h"
#include <iostream>


Enemy::Enemy(const std::string& name, RakNet::RakNetGUID guid) : Entity(-1, NULL, name, guid)
{
	
	
}


Enemy::~Enemy()
{

}

void Enemy::inicializar()
{
	animation = new Animation();
	granada = new Granada();
}

void Enemy::update(Time elapsedTime)
{
	desencolaMovimiento();

	updateState();
	updateAnimation();

	isMoving = true;
	if (m_renderState.getPreviousPosition().getX() == m_renderState.getPosition().getX() &&
		m_renderState.getPreviousPosition().getY() == m_renderState.getPosition().getY() &&
		m_renderState.getPreviousPosition().getZ() == m_renderState.getPosition().getZ())
		isMoving = false;

}

void Enemy::handleInput()
{
}

void Enemy::cargarContenido()
{
	//Creas el nodo(grafico)
	m_nodo = GraphicEngine::i().createAnimatedNode(Vec3<float>(0, 100, 0), Vec3<float>(0.05f, 0.05f, 0.05f), "", "../media/ArmyPilot.b3d");
	m_nodo.get()->setTexture("../media/body01.png", 1);
	m_nodo.get()->setTexture("../media/head01.png", 0);
	m_nodo.get()->setTexture("../media/m4tex.png", 2);

	m_renderState.setPosition(Vec3<float>(0, 100, 0));

	animation->addAnimation("Default", 0, 0);
	animation->addAnimation("Run_Forwards", 1, 69);
	animation->addAnimation("Run_backwards", 70, 138);
	animation->addAnimation("Walk", 139, 183);
	animation->addAnimation("Jump", 184, 219);
	animation->addAnimation("Jump2", 184, 219);
	animation->addAnimation("Idle", 220, 472);
	animation->addAnimation("AimRunning", 473, 524);




	granada->cargarContenido();
	granada->inicializar();

	m_animState = quieto;

	radius = 1.2f;
	height = 7.3f;
	mass = 0.f;

	//Al mundo de fisicas del cliente a�adimos una representacion del objeto fisico pero no calcula fisicas
	//(servira para hacer los raycast)


	m_rigidBody = PhysicsEngine::i().createCapsuleRigidBody(this,height, radius, mass, DISABLE_SIMULATION);

	btBroadphaseProxy* proxy = m_rigidBody->getBroadphaseProxy();
	proxy->m_collisionFilterGroup = col::Collisions::Enemy;
	proxy->m_collisionFilterMask = col::enemyCollidesWith;

}

void Enemy::borrarContenido()
{

	GraphicEngine::i().removeNode(m_nodo);
}

//Teletransporta un enemigo a la posicion que le pasas
void Enemy::setPosition(Vec3<float> pos) {

	m_renderState.setPosition(pos);
	btTransform transform = m_rigidBody->getCenterOfMassTransform();
	transform.setOrigin(btVector3(pos.getX(), pos.getY(), pos.getZ()));
	m_rigidBody->setCenterOfMassTransform(transform);
	m_nodo.get()->setPosition(pos);

}


void Enemy::updateEnemigo(Vec3<float> pos) {
	m_renderState.updatePositions(pos);
	btTransform transform = m_rigidBody->getCenterOfMassTransform();
	transform.setOrigin(btVector3(pos.getX(), pos.getY() + (height / 2) + radius, pos.getZ()));
	m_rigidBody->setCenterOfMassTransform(transform);

}

void Enemy::handleMessage(const Message & message)
{
	if (message.mensaje == "COLISION_BALA") {
		//TODO
		printf("le has dado a un enemigo\n");
		Cliente::i().enviarDisparo(m_guid);
	}
	else if (message.mensaje == "LAZARGRANADA") {

		TGranada* tGranada = static_cast<TGranada*>(message.data);


		granada->serverShoot(*tGranada);
	} else if (message.mensaje == "MOVIMIENTO") {

		TPlayer* tPlayer = static_cast<TPlayer*>(message.data);

		encolaMovimiento(*tPlayer);

		delete tPlayer;

	}
	else if (message.mensaje == "ARMAUP") {
		//TODO poner el codigo de cambiar el modelo del arma hacia arriba

	}
	else if (message.mensaje == "ARMADOWN") {
		//TODO poner el codigo de cambiar el modelo del arma hacia abajo

	}
}

//pila posiciones
void Enemy::encolaMovimiento(TPlayer pos)
{
	//m.lock();
	// A�adir a la cola
	TMovimiento mov;
	mov.position = pos.position;
	mov.rotation = pos.rotation;

	m_positions.push(mov);
	m_renderState.setVelocity(pos.velocidad);

	//m.unlock();
}

void Enemy::desencolaMovimiento()
{	
	//m.lock();
	//std::cout << "Numero Paquetes: " << m_positions.size() << std::endl;
	
	if (m_positions.size() > 3) {
		TMovimiento mov;
		while (!m_positions.empty()) {
			mov = m_positions.front();
			//lo borramos de la cola
			m_positions.pop();
			//llamamos al update con la nueva posicion
		}
		updateEnemigo(mov.position);
		m_renderState.updateRotations(mov.rotation);
	}

	else if (m_positions.size() > 0) {
		TMovimiento mov;
		mov = m_positions.front();
		//lo borramos de la cola
		m_positions.pop();
		//llamamos al update con la nueva posicion
		updateEnemigo(mov.position);
		m_renderState.updateRotations(mov.rotation);
	}else {
		updateEnemigo(m_renderState.getPosition() + m_renderState.getVelocity() * (1.f / 15.f));
		
	}
	//m.unlock();

}
/////////

//pila rotaciones
/*
void Enemy::encolaRot(TPlayer rot)
{
	//m.lock();
	// A�adir a la cola
	m_rotations.push(rot.position);

	//m.unlock();
}

void Enemy::desEncolaRot()
{
	//m.lock();

	if (m_rotations.size() > 3) {
		Vec3<float> new_rot;
		while (!m_rotations.empty()) {
			new_rot = m_rotations.front();
			//lo borramos de la cola
			m_rotations.pop();
			//llamamos al update con la nueva posicion
		}
		
		m_renderState.updateRotations(new_rot);
	}

	else if (m_positions.size() > 0) {
		Vec3<float> new_rot;
		new_rot = m_rotations.front();
		//lo borramos de la cola
		m_rotations.pop();
		//llamamos al update con la nueva posicion
		m_renderState.updateRotations(new_rot);
	}
	//m.unlock();

}*/
/////////

void Enemy::updateAnimation()
{
	switch (m_animState)
	{
	case quieto:
		if (animation->getActualAnimation() != "Idle") {
			m_nodo.get()->setAnimation(animation->getAnimationStart("Idle"), animation->getAnimationEnd("Idle"));
		}
		break;

	case andando:
		if (animation->getActualAnimation() != "Walk") {
			m_nodo.get()->setAnimation(animation->getAnimationStart("Walk"), animation->getAnimationEnd("Walk"));
		}
		break;

	case saltando:
		if (animation->getActualAnimation() != "Jump") {
			m_nodo.get()->setAnimation(animation->getAnimationStart("Jump"), animation->getAnimationEnd("Jump"));
		}
		break;
	case saltando2:
		if (animation->getActualAnimation() != "Jump2") {
			m_nodo.get()->setAnimation(animation->getAnimationStart("Jump2"), animation->getAnimationEnd("Jump2"));
		}
		break;

	}
}

void Enemy::updateState()
{
	/*if (!p_controller->onGround() && p_controller->numJumps == 0) {
		m_playerState = saltando;
	}
	else if (!p_controller->onGround() && p_controller->numJumps == 1) {
		m_playerState = saltando2;
	}
	else*/ if (isMoving) {
		m_animState = andando;
	}
	else {
		m_animState = quieto;
	}
}
