#include "ClippingZone.h"
#include <EntityManager.h>
#include <Player.h>

ClippingZone::ClippingZone(Vec3<float> center, Vec3<float> size,std::string name, int id) : m_name(name), id(id)
{
	Vec3<float> point1 = Vec3<float>(center.getX() - (size.getX() / 2), center.getY() - (size.getY() / 2), center.getZ() - (size.getZ() / 2));
	Vec3<float> point2 = Vec3<float>(center.getX() - (size.getX() / 2), center.getY() - (size.getY() / 2), center.getZ() + (size.getZ() / 2));
	Vec3<float> point3 = Vec3<float>(center.getX() - (size.getX() / 2), center.getY() + (size.getY() / 2), center.getZ() - (size.getZ() / 2));
	Vec3<float> point4 = Vec3<float>(center.getX() - (size.getX() / 2), center.getY() + (size.getY() / 2), center.getZ() + (size.getZ() / 2));
	Vec3<float> point5 = Vec3<float>(center.getX() + (size.getX() / 2), center.getY() - (size.getY() / 2), center.getZ() - (size.getZ() / 2));
	Vec3<float> point6 = Vec3<float>(center.getX() + (size.getX() / 2), center.getY() - (size.getY() / 2), center.getZ() + (size.getZ() / 2));
	Vec3<float> point7 = Vec3<float>(center.getX() + (size.getX() / 2), center.getY() + (size.getY() / 2), center.getZ() - (size.getZ() / 2));
	Vec3<float> point8 = Vec3<float>(center.getX() + (size.getX() / 2), center.getY() + (size.getY() / 2), center.getZ() + (size.getZ() / 2));
	//GraphicEngine::i().createNode(point1, Vec3<float>(1, 1, 1), "", "../media/box.obj");
	//GraphicEngine::i().createNode(point2, Vec3<float>(1, 1, 1), "", "../media/box.obj");
	//GraphicEngine::i().createNode(point3, Vec3<float>(1, 1, 1), "", "../media/box.obj");
	//GraphicEngine::i().createNode(point4, Vec3<float>(1, 1, 1), "", "../media/box.obj");
	//GraphicEngine::i().createNode(point5, Vec3<float>(1, 1, 1), "", "../media/box.obj");
	//GraphicEngine::i().createNode(point6, Vec3<float>(1, 1, 1), "", "../media/box.obj");
	//GraphicEngine::i().createNode(point7, Vec3<float>(1, 1, 1), "", "../media/box.obj");
	//GraphicEngine::i().createNode(point8, Vec3<float>(1, 1, 1), "", "../media/box.obj");

	m_points.push_back(point1);
	m_points.push_back(point2);
	m_points.push_back(point3);
	m_points.push_back(point4);
	m_points.push_back(point5);
	m_points.push_back(point6);
	m_points.push_back(point7);
	m_points.push_back(point8);

	nodo = new TNode(-1, SceneManager::i().getRootNode());
	nodo->setVisible(false);
}

void ClippingZone::addEntity(Entity * ent)
{
	m_entities.push_back(ent);
	nodo->addChild(ent->getNode()->getEntityNode()->getMiNodo());
	//ent->getNode()->add
}

std::vector<Vec3<float>> ClippingZone::getPoints()
{
	return m_points;
}

void ClippingZone::setVisible(bool visible)
{
	if (visible != m_visible) {
		m_visible = visible;
		nodo->setVisible(true);
		//putAllEntitiesInVisible(visible);
	}
}

bool ClippingZone::isPlayerinside()
{
	Vec3<float> playerPos = static_cast<Player*>(EntityManager::i().getEntity(PLAYER))->getRenderState()->getPosition();
	if (playerPos.getX() > m_points.at(0).getX() && playerPos.getY() > m_points.at(0).getY() && playerPos.getZ() > m_points.at(0).getZ()) {
		if (playerPos.getX() < m_points.at(7).getX() && playerPos.getY()  < m_points.at(7).getY() && playerPos.getZ() < m_points.at(7).getZ()) {
			setVisible(true);
			return true;
		}
	}
	return false;
}

void ClippingZone::putAllEntitiesInVisible(bool visible)
{
	for (std::vector<Entity*>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
		(*it)->getNode()->setVisible(visible);
	}
}
