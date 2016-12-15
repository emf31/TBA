#include "Asalto.h"
#include "../../Motor de Red/Cliente.h"
#include "../../Motor de Red/Estructuras.h"
#include "../../Otros/Util.h"

Asalto::Asalto() : Weapon()
{
	

}


Asalto::~Asalto()
{

}

void Asalto::inicializar()
{
	damage = 15;
	capacidadAmmo = 30;
	disparos = 0;
	cadencia = milliseconds(50);
	recarga = milliseconds(1000);
	numCargadores = numCargadoresAsalto;
	SIZE_OF_WORLD = btVector3(1500, 1500, 1500);
	FUERZA = btVector3(60.f, 60.f, 60.f);
}

void Asalto::update(Time elapsedTime)
{
	if (equipada) {
		Vec3<float> player_pos = EntityManager::i().getEntity(PLAYER)->getRenderState()->getPosition();
		Vec3<float> player_rot = GraphicEngine::i().getActiveCamera()->getRotation();
		m_renderState.updatePositions(Vec3<float>(player_pos.getX(), player_pos.getY() + 5.5f, player_pos.getZ()));
		m_renderState.updateRotations(player_rot);

		if (estadoWeapon == DESCARGADA) {
			if (numCargadores > 0) {
				if (relojrecarga.getElapsedTime() >= recarga) {
					estadoWeapon = CARGADA;
					disparos = 0;
					numCargadores--;
				}
			}
			else {
				relojrecarga.restart();
			}
		}
	
	}

}

void Asalto::handleInput()
{
}

void Asalto::cargarContenido()
{
	Vec3<float> player_pos = EntityManager::i().getEntity(PLAYER)->getRenderState()->getPosition();
	m_nodo = GraphicEngine::i().createAnimatedNode(Vec3<float>(player_pos.getX(), player_pos.getY(), player_pos.getZ()), Vec3<float>(0.003f, 0.003f, 0.003f), "", "../media/arma/asalto.obj");
	m_nodo->setVisible(false);
	m_nodo->setTexture("../media/ice0.jpg", 0);
}

void Asalto::borrarContenido()
{
}

void Asalto::handleMessage(const Message & message)
{

}

bool Asalto::handleTrigger(TriggerRecordStruct * Trigger)
{
	return false;
}

void Asalto::shoot()
{

	if (disparos < capacidadAmmo) {


		if (relojCadencia.getElapsedTime().asMilliseconds() > cadencia.asMilliseconds()) {
			//aumentamos en uno el numero de disparos, para reducir la municion
			disparos++;

			// posicion de la camara
			btVector3 start = bt(GraphicEngine::i().getActiveCamera()->getPosition()); 

			//a�adimos un poco de desvio en el arma
			start += btVector3(Randf(-1.f, 1.f), Randf(-1.f, 1.f), Randf(-1.f, 1.f)) / 10.f;

			btVector3 target = bt(GraphicEngine::i().getActiveCamera()->getTarget());
			btVector3 direccion = target - bt(GraphicEngine::i().getActiveCamera()->getPosition());
			direccion.normalize();

			btVector3 end = start + (direccion*SIZE_OF_WORLD);

			btCollisionWorld::ClosestRayResultCallback ray(start, end);

			PhysicsEngine::i().m_world->rayTest(start, end, ray);

			btVector3 posicionImpacto;


			if (ray.hasHit())//si ray ha golpeado algo entro
			{
				//Veo la entity que colisiona
				Entity* ent = static_cast<Entity*>(ray.m_collisionObject->getUserPointer());
				if (ent != EntityManager::i().getEntity(PLAYER))
				{
					if (ent->getClassName() == "Enemy") {
						Message msg(ent, "COLISION_BALA", NULL);
						MessageHandler::i().sendMessage(msg);
					}
					//Para mover objetos del mapa
					posicionImpacto = ray.m_hitPointWorld;

					if (ent->getClassName() == "PhysicsEntity") {
						btRigidBody::upcast(ray.m_collisionObject)->activate(true);
						btRigidBody::upcast(ray.m_collisionObject)->applyImpulse(direccion*FUERZA, posicionImpacto);
					}
				}
			}

			GunBullet* bala = new GunBullet(cons(start), cons(direccion), cons(posicionImpacto), GraphicEngine::i().getActiveCamera()->getRotation());
			bala->cargarContenido();

			if (Cliente::i().isConected()) {
				//enviamos el disparo de la bala al servidor para que el resto de clientes puedan dibujarla
				Cliente::i().dispararBala(cons(start), cons(direccion), cons(posicionImpacto), GraphicEngine::i().getActiveCamera()->getRotation());
			}

			relojCadencia.restart();

	}


	}
	if (disparos == capacidadAmmo && estadoWeapon == CARGADA) {
		if (numCargadores > 0) {
			relojrecarga.restart();
		}
		estadoWeapon = DESCARGADA;
	}
}