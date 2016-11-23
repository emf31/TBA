#include "RocketLauncherDrop.h"
#include "../../Motor de Red/Cliente.h"


RocketLauncherDrop::RocketLauncherDrop(std::shared_ptr<SceneNode> nodo, const std::string& name) : WeaponDrop(nodo, name)
{
}


RocketLauncherDrop::~RocketLauncherDrop()
{
}

void RocketLauncherDrop::inicializar()
{
}

void RocketLauncherDrop::update(Time elapsedTime)
{
	if (estado == USADO) {
		if (clockRecargaLife.getElapsedTime().asSeconds() >= timeRecargaLife) {
			estado = DISPONIBLE;
			m_ghostObject = PhysicsEngine::i().createBoxGhostObject(this, Vec3<float>(5.f, 5.f, 5.f));
			m_nodo->setVisible(true);
		}
	}
}

void RocketLauncherDrop::handleInput()
{
}

void RocketLauncherDrop::cargarContenido()
{


}

void RocketLauncherDrop::borrarContenido()
{
}

void RocketLauncherDrop::handleMessage(const Message & message)
{
	if (message.mensaje == "COLLISION") {
		if (static_cast<Entity*>(message.data)->getClassName() == "Player") {

			if (estado == DISPONIBLE) {
				PhysicsEngine::i().m_world->removeCollisionObject(m_ghostObject);
				estado = USADO;
				clockRecargaLife.restart();

				static_cast<Player*>(message.data)->setWeapon(LANZACOHETES);
				m_nodo->setVisible(false);
				if (Cliente::i().isConected())
					Cliente::i().armaCogida(m_id);

			}

		}
	}
}

