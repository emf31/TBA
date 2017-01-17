#include "Map.h"
#include <GraphEdge.h>
#include <EntityManager.h>
#include <Util.h>




void Map::addNodosToCells()
{
	
	for (std::vector<NavGraphNode>::iterator it = grafo->m_nodes.begin(); it != grafo->m_nodes.end(); ++it)
	{
		cellSpace->addNodoACelda(*it);
	}
}

Map::Map() : grafo(NULL), cellSpace(NULL)
{
}



void Map::inicializar()
{


	//dimensiones del mapa 647x475
	//se crea la division del espacio en celdas
	cellSpace = new CellSpacePartition(647.f, 475.f, 10, 5);

	//Lee el mapa
	map.readMap("../media/laberinto.json");


	//GRAFO
	
	//True, grafo dirigido
	grafo = new SparseGraph(true);
	grafo->readGraph("../media/laberinto.json");

	/*
	grafo->addNode(nodo1);

	NavGraphNode nodo2(grafo->getNextFreeNodeIndex(), Vec2f(0, 10));
	grafo->addNode(nodo2);

	NavGraphNode nodo3(grafo->getNextFreeNodeIndex(), Vec2f(20, 60));
	grafo->addNode(nodo3);

	NavGraphNode nodo4(grafo->getNextFreeNodeIndex(), Vec2f(30, 60));
	grafo->addNode(nodo4);

	NavGraphNode nodo5(grafo->getNextFreeNodeIndex(), Vec2f(150, 200));
	grafo->addNode(nodo5);

	NavGraphNode nodo6(grafo->getNextFreeNodeIndex(), Vec2f(100, 100));
	grafo->addNode(nodo6);

	
	NavGraphNode nodo7(grafo->getNextFreeNodeIndex(), Vec2f(10, 100));
	grafo->addNode(nodo7);
	
	NavGraphNode nodo8(grafo->getNextFreeNodeIndex(), Vec2f(20, 90));
	grafo->addNode(nodo8);
	NavGraphNode nodo9(grafo->getNextFreeNodeIndex(), Vec2f(250, 300));
	grafo->addNode(nodo9);
	NavGraphNode nodo10(grafo->getNextFreeNodeIndex(), Vec2f(253, 290));
	grafo->addNode(nodo10);
	NavGraphNode nodo11(grafo->getNextFreeNodeIndex(), Vec2f(250, 305));
	grafo->addNode(nodo11);
	NavGraphNode nodo12(grafo->getNextFreeNodeIndex(), Vec2f(250, 295));
	grafo->addNode(nodo12);
	

	GraphEdge edge1(nodo1.Index(), nodo5.Index(), 2.9);
	GraphEdge edge2(nodo1.Index(), nodo6.Index(), 1.f);

	GraphEdge edge3(nodo5.Index(), nodo6.Index(), 3.f);
	GraphEdge edge4(nodo5.Index(), nodo2.Index(), 1.9);

	GraphEdge edge5(nodo6.Index(), nodo4.Index(), 1.1);
	GraphEdge edge6(nodo2.Index(), nodo3.Index(), 3.1);
	GraphEdge edge7(nodo4.Index(), nodo3.Index(), 3.7);

	GraphEdge edge8(nodo3.Index(), nodo5.Index(), 0.8);

	grafo->addEdge(edge1);
	grafo->addEdge(edge2);
	grafo->addEdge(edge3);
	grafo->addEdge(edge4);
	grafo->addEdge(edge5);
	grafo->addEdge(edge6);
	grafo->addEdge(edge7);
	grafo->addEdge(edge8);*/

	//grafo->printGrafo();


	

	//se meten los nodos a las celdas correspondientes
	std::cout<<""<<std::endl;
	std::cout << "" << std::endl;
	std::cout << "" << std::endl;
	std::cout << "A�ADIMOS LOS NODOS A LAS CELDAS" << std::endl;
	addNodosToCells();
	std::cout << "" << std::endl;
	std::cout << "" << std::endl;
	std::cout << "" << std::endl;
	cellSpace->mostrarContenido();




	//AStarSearch astar(grafo, 0 , 2);
	//std::list<int> camino = astar.GetPathToTarget();


}



void Map::borrarContenido() {
	delete grafo;
	delete cellSpace;
}
bool Map::isPathObstructed(Vec2f posIni, Vec2f posFinal, float radio)
{
	//TODO hacer este metodo
	return false;
}

void Map::ConvertirNodosAPosiciones(std::list<int> CaminoDeNodos, std::list<Vec2f> camino)
{
	//Iteramos la lista de node index y obtenemos el nodo del grafo para meter la pos en el camino
	for (std::list<int>::const_iterator it = CaminoDeNodos.begin(); it != CaminoDeNodos.end(); ++it) {
		camino.push_back(grafo->getNode(*it).getPosition());
		
	}
}

//Busca un punto de respawn seguro en el mapa
Vec3<float> Map::searchSpawnPoint()
{
	float radio = 100;
	float fDistance = 0;
	int spawn = 0;

	std::vector<Vec3<float>> m_spawns = map.getSpawnPoints();

	if (m_spawns.size() == 1) {
		return m_spawns.at(0);
	}

	std::list<Entity*> enemies = EntityManager::i().getEnemies();

	std::vector<Vec3<float>> auxSpawns;

	std::list<Entity*>::iterator it;
	std::vector<Vec3<float>>::iterator it2;

	for (it2 = m_spawns.begin(); it2 != m_spawns.end(); ++it2) {
		bool valid = true;
		for (it = enemies.begin(); it != enemies.end(); ++it) {
			Vec3<float> vector = (*it)->getRenderState()->getPosition() - (*it2);
			fDistance = vector.Magnitude();

			if (fDistance < 100) {
				valid = false;
				break;
			}

		}
		if (valid) {
			auxSpawns.push_back(*it2);
		}

	}

	//Si hay mas de 1 elegimos uno aleatorio
	if (auxSpawns.size() > 1) {
		spawn = Randi(0, auxSpawns.size() - 1);
	}

	//Si no esta vacio es que hemos encontrado uno
	if (!auxSpawns.empty()) {
		//p_controller->reset(PhysicsEngine::i().m_world);
		//setPosition(auxSpawns.at(spawn));
		return auxSpawns.at(spawn);
	}
	else {
		//p_controller->reset(PhysicsEngine::i().m_world);
		//setPosition(m_spawns.at(Randi(0, m_spawns.size() - 1)));
		return m_spawns.at(Randi(0, m_spawns.size() - 1));
	}
}

void Map::CalcularNodosCercanos(Vec2f& pos, std::list<NavGraphNode>& nodosCercanos)
{
	
	std::vector<int> celdasVecinas;
	int indCelda = cellSpace->PositionToIndex(pos);
	cellSpace->CalculaNodoEnCelda(indCelda, nodosCercanos);
	
	if (nodosCercanos.size() == 0) {
		cellSpace->CalculaNodosEnCeldasVecinas(indCelda, nodosCercanos, celdasVecinas);
	}
	if(nodosCercanos.size() == 0) {
		//nos guardamos en celdasVecinas las celdas visitadas para luego calcular las celdas vecina de cada una de las visitadas, es decir,
		//la primera vez visitamos las 8 vecinas de indCelda, pero si no encontramos ningun nodo, la segunda vez tendriamos que visitar las
		//8 vecinas de cada una de las celdas que visitamos antes, para ello recorremos la lista
		for (int i = 0; i < celdasVecinas.size(); i++) {
			
		
			cellSpace->CalculaNodosEnCeldasVecinas(celdasVecinas[i], nodosCercanos, celdasVecinas);
			
		}
		
	}

	
}
