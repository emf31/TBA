#pragma once
#include <list>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicscommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "../Entities/Entity.h"
#include "../Otros/Time.hpp"

#define BIT(x) (1<<(x))
namespace col {
	enum Collisions {
		COL_NOTHING = 0, //<Collide with nothing
		Granada = BIT(0), //<Collide with ships
		Suelo = BIT(1), //<Collide with walls
		Object = BIT(2), //<Collide with powerups
		Rocket = BIT(3)
	};
}



class PhysicsEngine
{
public:
	

	static PhysicsEngine& i() {
		static PhysicsEngine singleton;
		return singleton;
	}

	btDynamicsWorld* m_world;

	//Inicializa el motor de fisicas
	void inicializar();

	//Updatea las fisicas
	void update(Time elapsedTime);


	void createBoxDynamicCharacter(btRigidBody* rigid);


	//creamos y registramos un rigidbody cuadrado - asumimos que la posicion esta puesta
	btRigidBody* createBoxRigidBody(Entity* entity, const Vec3<float> &scale, float masa, int body_state = ACTIVE_TAG);

	btRigidBody* createCapsuleRigidBody(Entity* entity, float height, float radius, float masa, int body_state = ACTIVE_TAG);

	//creamos y registramos una esfera - asumimos que la posicion esta puesta
	btRigidBody * createSphereRigidBody(Entity * entity, float radius, float mass, int body_state = ACTIVE_TAG);

	btGhostObject* createBoxGhostObject(Entity * entity, const Vec3<float>& scale);

	btGhostObject* createSphereShape(Entity* entity, float radio);


	//borra un rigidbody de la simulacion
	bool removeRigidBody(btRigidBody* body);
	bool removeGhostObject(btGhostObject * body);

	//aqui se borran todos los punteros
	void apagar();

private:
	btDefaultCollisionConfiguration* m_config;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_broadphase;
	btSequentialImpulseConstraintSolver* m_solver;
	btGhostPairCallback* m_pGhostPairCallBack;

	std::list<btRigidBody*> m_rigidBodies;

	static const Time tickPhysics;

	//Constructor privado para que no se pueda llamar
	PhysicsEngine() { }
};

