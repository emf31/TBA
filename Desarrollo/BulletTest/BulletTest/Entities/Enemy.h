#pragma once
#include <Entity.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicscommon.h>


#include <Estructuras.h>
#include <Message.h>
#include <Animation.h>

#include <queue>

#include <BasicSceneNode.h>
#include <Granada.h>

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
	virtual void setPosition(const Vec3<float> &pos) override;
	virtual std::string getClassName() { return "Enemy"; }

	void updateEnemigo(Vec3<float> pos);

	void updateAnimation();
	void updateState();

	virtual void handleMessage(const Message & message) override;

	virtual bool handleTrigger(TriggerRecordStruct* Trigger) override;



	void encolaMovimiento(TMovimiento& mov);
	void desencolaMovimiento();

	void lanzarGranada(TGranada g);

	bool isDying() { return m_isDying; }
	void setIsDying(bool die) { m_isDying = die; }

	float getRadio(){
		return radius;
	}

	void setVisibilidadBilboardSync();
	

private:


	float radius;
	float height;
	float mass;
	Animation* animation;
	Granada* granada;

	enum AnimState { quieto, andando, corriendo, saltando, saltando2 } m_animState;
	bool isMoving;

	bool m_isDying;

	Clock relojMuerte;
	
	


	btRigidBody* m_rigidBody;
	std::queue<TMovimiento> m_positions;
	

	
	
};

