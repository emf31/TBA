#pragma once
#include <unordered_map>
#include "../Otros/Time.hpp"
#include "Entity.h"

class EntityManager
{
public:
	static EntityManager& i() {
		static EntityManager singleton;
		return singleton;
	}


	
	

	//Inicializa todas las entities
	void inicializar();
	//Updatea todas las entities
	void update(Time elapsedTime);
	//Updatea la posicion de render de todas las entities
	void updateRender(float interpolation);

	void handleInput();
	//Carga el contenido de todas las entities
	void cargarContenido();
	//Borra el contenido de todas las entities
	void borrarContenido();
	//Borra el entity manager(borramos el mapa y los punteros a entities)
	void apagar();

	//Registra una entity en el mapa
	void registerEntity(Entity* entity);
	//Borra una entity del mapa
	void removeEntity(Entity* entity);
	Entity* getEntity(int id);

private:
	EntityManager(EntityManager const&);
	EntityManager() { m_nextID = 0; m_entities = std::unordered_map<int, Entity*>(); }

	int m_nextID;
	std::unordered_map<int, Entity*> m_entities;

};

