#include "Menu.h"
#include <SoundManager.h>


Menu::Menu()
{
}

Menu::~Menu()
{
}

void Menu::Inicializar()
{
	menuGUI.inicializar();

	//GraphicEngine::i().getActiveCamera()->setInputReceiver(false);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	GraphicEngine::i().enableMouse(true);

	SoundManager::i().stopAllSounds();
	SoundManager::i().playSound("../media/MenuSong2.mp3",true);
	//SoundManager::i().playSound("../media/shoot.mp3", static_cast<Player*>(EntityManager::i().getEntity(PLAYER))->getRenderState()->getPosition());
}

void Menu::Clear()
{
	menuGUI.destroy();
}

void Menu::HandleEvent()
{

	if (Input::i().leftMouseDown()) {

		menuGUI.injectLeftMouseButton();

	}
	else if (Input::i().leftMouseUp()) {

		menuGUI.injectLeftMouseButtonUp();

	}


}

void Menu::Update(Time timeElapsed)
{
	menuGUI.update();
}

void Menu::Render(float interpolation, Time elapsedTime)
{

	float mouseX = (float)Input::i().getMouseX();
	float mouseY = (float)Input::i().getMouseY();


	//GUI
	menuGUI.injectMousePosition(mouseX, mouseY);

	GraphicEngine::i().renderAll();
}






