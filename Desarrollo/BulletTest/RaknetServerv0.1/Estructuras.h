#pragma once
#include "../../BulletTest/Otros/vec3.hpp"
#include <RakPeerInterface.h>
#include <iostream>
#include <MessageIdentifiers.h>

typedef struct {
	Vec3<float> position;
	RakNet::RakNetGUID guid;
	std::string name;
}TPlayer;

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	MENSAJE_POSICION = ID_USER_PACKET_ENUM + 2,
	MENSAJE_NOMBRE = ID_USER_PACKET_ENUM + 3,
	NUEVO_PLAYER = ID_USER_PACKET_ENUM + 4,
	GUID_NUESTRO = ID_USER_PACKET_ENUM + 5,
	MOVIMIENTO = ID_USER_PACKET_ENUM + 6,
	LISTA_CLIENTES = ID_USER_PACKET_ENUM + 7,
	NUEVO_CLIENTE = ID_USER_PACKET_ENUM + 8,
	ELIMINAR_CLIENTE = ID_USER_PACKET_ENUM + 9,
	ACTUALIZA_CLIENTE = ID_USER_PACKET_ENUM + 10
};
