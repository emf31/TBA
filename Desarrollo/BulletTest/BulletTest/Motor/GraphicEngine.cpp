
#include "GraphicEngine.h"
#include "../MastEventReceiver.hpp"


GraphicEngine::GraphicEngine()
{
}

SceneNode* GraphicEngine::createNode(const Vec3<float>& TPosition, const Vec3<float>& TScale, const io::path & texture, const io::path & mesh)
{
	ISceneNode *Node;
	if(mesh!="")
		Node = irrScene->addMeshSceneNode(irrScene->getMesh(mesh));
	else
		Node = irrScene->addCubeSceneNode(1.0f);

	Node->setScale(vector3df(TScale.getX(), TScale.getY(), TScale.getZ()));
	Node->setPosition(vector3df(TPosition.getX(), TPosition.getY(), TPosition.getZ()));
	//Asi no le afectan las luces
	Node->setMaterialFlag(EMF_LIGHTING, false);

	m_camera = 0;

	//Si es diferente de "" asignamos una textura al nodo
	if (texture != "") {
		Node->setMaterialTexture(0,irrDriver->getTexture(texture));
	}
	//Le pasamos irrDriver para que se encargue el de asignar la textura
	return new SceneNode(Node, irrDriver);
}


void GraphicEngine::createCamera(Vec3<float> position, Vec3<float> target)
{
	//camara tipo fps
	ICameraSceneNode* cam = irrScene->addCameraSceneNodeFPS();
	cam->setPosition(vector3df(position.getX(), position.getY(), position.getZ()));
	cam->setTarget(vector3df(target.getX(), target.getY(), target.getZ()));
	cam->setInputReceiverEnabled(true);

	//Creamos el objeto camara y la metemos en unordermap de cameras y si es la primera se setea como activa
	Camera* myCamera=new Camera(cam);
	cameras[m_camera] = myCamera;

	if (m_camera == 0)
		active_camera = myCamera;

	m_camera++;
}

void GraphicEngine::setCameraEntity(Entity * entity)
{
	active_camera->asignarEntity(entity);
}

void GraphicEngine::updateCamera()
{
	active_camera->update();
}


Camera * GraphicEngine::getActiveCamera()
{
	return active_camera;
}

void GraphicEngine::setActiveCamera(int ID)
{
	active_camera = cameras[ID];
}

void GraphicEngine::renderAll()
{
	irrDriver->beginScene(true, true, SColor(255, 100, 101, 140));
	irrScene->drawAll();
	irrDriver->endScene();
}

void GraphicEngine::inicializar()
{
	// Initialize irrlicht
	irrDevice = createDevice(video::EDT_OPENGL, dimension2d<u32>(800, 600), 32, false, false, false, &MastEventReceiver::i());
	irrDevice->setWindowCaption(L"Test");

	irrGUI = irrDevice->getGUIEnvironment();
	irrScene = irrDevice->getSceneManager();
	irrDriver = irrDevice->getVideoDriver();

	irrDevice->getCursorControl()->setVisible(0);
}

bool GraphicEngine::isRuning()
{
	return irrDevice->run();
}

bool GraphicEngine::isWindowActive()
{
	return irrDevice->isWindowActive();
}

bool GraphicEngine::apagar()
{
	return irrDevice->drop();
}
