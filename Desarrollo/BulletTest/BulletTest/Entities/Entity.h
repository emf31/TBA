#pragma once

#include "irrlicht.h"
#include "../Otros/vec3.hpp"
#include "../Otros/Time.hpp"
#include "../RenderState.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


class Entity
{
public:
	Entity(ISceneNode *node);
	virtual ~Entity();

	void update(Time elapsedTime, btRigidBody *TObject, ISceneManager *irrScene, int player);
	void updateRender(float interpolation);
	Vec3<float> getRenderPosition();

	bool isMovingBackward;
	bool isMovingForward;
	bool isMovingLeft;
	bool isMovingRight;
private:
	

	ISceneNode *m_nodo;
	RenderState renderState;
};

