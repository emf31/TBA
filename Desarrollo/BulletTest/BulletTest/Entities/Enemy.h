#pragma once
#include "Entity.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicscommon.h>


#include "../Motor de Red/Estructuras.h"
#include "../Handlers/Message.h"
#include "../Motor/Animation.h"

#include <queue>
#include <mutex>

#include "../Motor/BasicSceneNode.h"
#include "../Entities/Granada.h"

#include "../Otros/SafeQueue.h"

class Enemy : public Entity
{
public:
	Enemy(const std::string& name, RakNet::RakNetGUID guid = RakNet::UNASSIGNED_RAKNET_GUID);

	~Enemy();

	// Heredado v�a Entity
	virtual void inicializar() override;
	virtual void update(Time elapsedTime) override;
	virtual void handleInput() override;
	virtual void cargarContenido() override;
	virtual void borrarContenido() override;

	void setPosition(Vec3<float> pos);
	void updateEnemigo(Vec3<float> pos);

	void updateAnimation();
	void updateState();

	virtual void handleMessage(const Message & message) override;

	void encolaPos(TPlayer pos);
	void desEncolaPos();
	void encolaRot(TPlayer rot);
	void desEncolaRot();
	//esto es un semaforo para cuando accedamos de diferentes hilos no intenten escribir y leer a la vez
	std::mutex m;


private:

	float radius;
	float height;
	float mass;
	Animation* animation;
	Granada* granada;

	enum AnimState { quieto, andando, corriendo, saltando, saltando2 } m_animState;
	bool isMoving;

	btRigidBody* m_rigidBody;
	SafeQueue<Vec3<float>> m_positions;
	std::queue<Vec3<float>> m_rotations;

	virtual std::string getClassName() { return "Enemy"; }
	
};

