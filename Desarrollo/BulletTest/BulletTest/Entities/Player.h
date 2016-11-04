#pragma once
#include "Entity.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicscommon.h>
#include "Rocket.h"
#include <vector>
#include "DynamicCharacterController.h"


class Player : public Entity
{
public:
	Player();
	~Player();

	void jump();

	void shoot();
	void shootRocket();


	void move_up();
	void move_down();
	void move_right();
	void move_left();



private:
	
	btRigidBody* m_rigidBody;

	Rocket* rocket;
	Clock clockRecargaRocket;
	float timeRecargaRocket = 3;


	bool isJumping;
	bool isMoving;

	float m_speedFactor;
	float giro;

	Vec3<float> vectorPrev;
	Vec3<float> vectorNew;

	Vec3<float> speedFinal;


	// Heredado v�a Entity
	virtual void inicializar() override;
	virtual void update(Time elapsedTime) override;
	virtual void handleInput() override;
	virtual void cargarContenido() override;
	virtual void borrarContenido() override;
	virtual void handleMessage(const Message& message) override;

	//Player controller
	DynamicCharacterController* p_controller;

	float m_acceleration_walk;
	float m_acceleration_run;
	float m_deceleration_walk;
	float m_deceleration_run;
	float m_maxSpeed_walk;
	float m_maxSpeed_run;
};

