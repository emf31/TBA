#pragma once
#include <unordered_map>
#include "../Otros/Time.hpp"
#include "Entity.h"
#include <set>
#include <list>
#include "../Motor de Red/Estructuras.h"



static const int PLAYER = 1000;

class EntityManager
{
public:
	static EntityManager& i() {
		static EntityManager singleton;
		return singleton;
	}

	int numClientes() {
		return m_jugadores.size();
	}
	
	void mostrarClientes();
	void muestraPosClientes();

	void inicializarEntityManager() {
		m_nextID = 0; m_entities = std::unordered_map<int, Entity*>(); m_jugadores = std::unordered_map<unsigned long, Entity*>();
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

	void cleanDeleteQueue();

	void cambiaTabla(std::unordered_map <unsigned long, TFilaTabla> tabla) {
		m_tabla = tabla;
	}
	void muestraTabla();

	Entity* getEntity(int id);
	Entity* getRaknetEntity(RakNet::RakNetGUID guid);
	list<Entity*> getCharacters();

	

private:
	EntityManager(EntityManager const&);
	EntityManager() { 
	}

	int m_nextID;
	
	std::unordered_map<int, Entity*> m_entities;
	std::unordered_map<unsigned long, Entity*> m_jugadores;
	std::unordered_map <unsigned long, TFilaTabla> m_tabla;
	
	std::set<Entity*> delete_set;

};

