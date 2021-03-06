#pragma once

#include <vec3.hpp>
#include <Entity.h>
#include <EntityManager.h>
#include <Time.hpp>
#include <Clock.hpp>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>

class RocketBulletEnemy : public Entity
{
public:
	RocketBulletEnemy(Vec3<float> position, Vec3<float> direction, Vec3<float> rotation);
	~RocketBulletEnemy();

private:
	float radioExplosion;
	Vec3<float> m_direction;
	Vec3<float> m_position;
	Vec3<float> m_rotation;
	float m_velocity;
	Time m_lifetime;
	Clock timelifeclock;

	btRigidBody* m_rigidBody;
	int estado = DISPONIBLE;

	// Heredado v�a Entity
	virtual void inicializar() override;
	virtual void update(Time elapsedTime) override;
	virtual void handleInput() override;
	virtual void cargarContenido() override;
	virtual void borrarContenido() override;
	virtual void handleMessage(const Message & message) override;
	virtual bool handleTrigger(TriggerRecordStruct* Trigger) override;
	virtual std::string getClassName() override;
	virtual void setPosition(const Vec3<float> &pos) override;

	float explosion(Vec3<float>posExplosion, Vec3<float>posCharacter, float radio);


};