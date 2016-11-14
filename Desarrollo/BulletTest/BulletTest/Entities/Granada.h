#pragma once
#include "irrlicht.h"
#include "Entity.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#define GRANADACARGADA 0
#define GRANADADISPARADA 1


class Granada :
	public Entity
{
public:
	Granada();
	~Granada();

	void setRigidBody(btRigidBody* rigidBody) { m_rigidBody = rigidBody; }

	void setPosition(Vec3<float> pos);
	int getEstado() { return estado;}
	void setEstado(int newEstado) { estado = newEstado; }
	void resetRigidBody();
	// Heredado v�a Entity
	virtual void inicializar() override;

	virtual void update(Time elapsedTime) override;

	virtual void handleInput() override;

	virtual void cargarContenido() override;

	virtual void borrarContenido() override;

	virtual void handleMessage(const Message& message) override;

	virtual std::string getClassName() { return "Granada"; }

	void shoot(const btVector3& posicionPlayer);


	btRigidBody* m_rigidBody;

	Clock clockRecargaGranada;

	float timeRecargaGranada = 3;

private:
	int estado=GRANADACARGADA;
};

