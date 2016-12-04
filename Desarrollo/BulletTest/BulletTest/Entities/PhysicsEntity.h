#pragma once
#include "Entity.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "../Motor/BasicSceneNode.h"
#include "../Motor/PhysicsEngine.h"
class PhysicsEntity : public Entity
{
public:
	PhysicsEntity(std::shared_ptr<BasicSceneNode> nodo, const std::string& name);
	~PhysicsEntity();

	void setRigidBody(btRigidBody* rigidBody) { 
		m_rigidBody = rigidBody;
	}
	void setCollisionGroup(const int &group) {
		btBroadphaseProxy* proxy = m_rigidBody->getBroadphaseProxy();
		proxy->m_collisionFilterGroup = group;
	}
	void setCollisionMask(const int &mask) {
		btBroadphaseProxy* proxy = m_rigidBody->getBroadphaseProxy();
		proxy->m_collisionFilterMask = mask;

	}
	btRigidBody* getRigidBody() { return m_rigidBody; }

	void rotate(Vec3<float> rot);

	// Heredado v�a Entity
	virtual void inicializar() override;

	virtual void update(Time elapsedTime) override;

	virtual void handleInput() override;

	virtual void cargarContenido() override;

	virtual void borrarContenido() override;

	virtual void handleMessage(const Message & message) override;

	virtual std::string getClassName() { return "PhysicsEntity"; }


	void setPosition(Vec3<float> pos) {

		m_renderState.setPosition(pos);
		btTransform transform = m_rigidBody->getCenterOfMassTransform();
		transform.setOrigin(btVector3(pos.getX(), pos.getY(), pos.getZ()));
		m_rigidBody->setCenterOfMassTransform(transform);
		m_nodo->setPosition(pos);
		std::cout << m_nodo->getPosition().getX()<<'\n';

	}

private:
	btRigidBody* m_rigidBody;
};

