#pragma once
#include <Lobby2Client_Steam.h> // If Lobby2Client_Steam.h is included before SocketLayer.h, then it will use the steam send functions
#include <Lobby2Message_Steam.h>
#include <FullyConnectedMesh2.h>
#include <steam_api.h>
#include <RakNetTypes.h>
#include <iostream>


class SteamResults : public RakNet::Lobby2Callbacks {
public:


	
	virtual void MessageResult(RakNet::Notification_Console_MemberJoinedRoom *message);

		virtual void MessageResult(RakNet::Console_CreateRoom *message);

		virtual void MessageResult(RakNet::Console_SearchRooms *message);

		virtual void ExecuteDefaultResult(RakNet::Lobby2Message *message);
		
		virtual void MessageResult(RakNet::Notification_Console_MemberLeftRoom *message);

private:

	uint64_t lastRoom;
};