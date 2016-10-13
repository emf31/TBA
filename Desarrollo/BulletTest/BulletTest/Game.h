#pragma once

#include "Otros/Timer.h"
#include "Otros/vec3.hpp"
#include "irrlicht.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


class Game
{
public:
	Game();
	virtual ~Game();

	void run();

	void inicializarIrrlitch();
	void processEvents(); // Captura y procesa eventos
	void update(milliseconds elapsedTime);
	void render(float interpolation, milliseconds elapsedTime);

	ISceneNode* CreateBox(const Vec3<double> &TPosition, const Vec3<float> &TScale, float TMass);

	IrrlichtDevice *irrDevice;
	IVideoDriver *irrDriver;
	ISceneManager *irrScene;
	IGUIEnvironment *irrGUI;

private:
	static const duration<float> timePerFrame;
	float interpolation;

	
	ICameraSceneNode *Camera;
};



