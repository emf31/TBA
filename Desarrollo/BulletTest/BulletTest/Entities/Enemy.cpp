
#include "Enemy.h"
#include "../Motor/PhysicsEngine.h"
#include "../Motor/GraphicEngine.h"
#include "../Motor de Red/Estructuras.h"
#include "../Motor de Red/Cliente.h"

Enemy::Enemy(const std::string& name, RakNet::RakNetGUID guid) : Entity(3, NULL, name, guid)
{
}


Enemy::~Enemy()
{
}

void Enemy::inicializar()
{
}

void Enemy::update(Time elapsedTime)
{

}

void Enemy::handleInput()
{
}

void Enemy::cargarContenido()
{
	//Creas el nodo(grafico)
	m_nodo = GraphicEngine::i().createNode(Vec3<float>(200, 500, 0), Vec3<float>(100.f, 100.f, 100.f), "../media/ice0.jpg");

	m_renderState.setPosition(Vec3<float>(200, 500, 0));

	//Al mundo de fisicas del cliente a�adimos una representacion del objeto fisico pero no calcula fisicas
	//(servira para hacer los raycast)

	m_rigidBody = PhysicsEngine::createBoxRigidBody(this, Vec3<float>(100.f, 100.f, 100.f),0.0f, DISABLE_SIMULATION);
}

void Enemy::borrarContenido()
{

	GraphicEngine::i().removeNode(m_nodo);
}

void Enemy::setPosition(Vec3<float> pos) {
	m_renderState.setPosition(pos);
	btTransform transform = m_rigidBody->getCenterOfMassTransform();
	transform.setOrigin(btVector3(pos.getX(), pos.getY(), pos.getZ()));
	m_rigidBody->setCenterOfMassTransform(transform);
	m_nodo->setPosition(pos);

}

void Enemy::updateEnemigo(Vec3<float> pos) {
	m_renderState.updatePositions(pos);
	btTransform transform = m_rigidBody->getCenterOfMassTransform();
	transform.setOrigin(btVector3(pos.getX(), pos.getY(), pos.getZ()));
	m_rigidBody->setCenterOfMassTransform(transform);
}

void Enemy::handleMessage(const Message & message)
{
	if (message.mensaje == "MOVE") {
		TPlayer* p = (TPlayer*)message.data;
		updateEnemigo(p->position);
	}else if (message.mensaje == "COLISION_BALA") {
		//TODO
		printf("le has dado a un enemigo\n");
		Cliente::i().enviarDisparo(m_guid);
	}
}