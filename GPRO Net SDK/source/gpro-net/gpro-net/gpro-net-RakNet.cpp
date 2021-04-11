/*
   Copyright 2021 Daniel S. Buckstein

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	GPRO Net SDK: Networking framework.
	By Daniel S. Buckstein

	gpro-net-RakNet.cpp
	Source for RakNet common management.
*/

#include "gpro-net/gpro-net/gpro-net-RakNet.hpp"


namespace gproNet
{
	cRakNetManager::cRakNetManager()
		: peer(RakNet::RakPeerInterface::GetInstance())
	{
	}

	cRakNetManager::~cRakNetManager()
	{
		RakNet::RakPeerInterface::DestroyInstance(peer);
	}

	bool cRakNetManager::ProcessMessage(RakNet::BitStream& bitstream, RakNet::SystemAddress const sender, RakNet::Time const dtSendToReceive, RakNet::MessageID const msgID)
	{
		// process messages that can be handled the same way for all types of peers
		//switch (msgID)
		//{
		//}
		return false;
	}

	RakNet::BitStream& cRakNetManager::WriteTimestamp(RakNet::BitStream& bitstream)
	{
		bitstream.Write((RakNet::MessageID)ID_TIMESTAMP);
		bitstream.Write((RakNet::Time)RakNet::GetTime());
		return bitstream;
	}

	RakNet::BitStream& cRakNetManager::ReadTimestamp(RakNet::BitStream& bitstream, RakNet::Time& dtSendToReceive_out, RakNet::MessageID& msgID_out)
	{
		RakNet::Time tSend = 0, tReceive = 0;
		if (msgID_out == ID_TIMESTAMP)
		{
			tReceive = RakNet::GetTime();
			bitstream.Read(tSend);
			bitstream.Read(msgID_out);
			dtSendToReceive_out = (tReceive - tSend);
		}
		return bitstream;
	}

	RakNet::BitStream& cRakNetManager::WriteTest(RakNet::BitStream& bitstream, char const message[])
	{
		WriteTimestamp(bitstream);
		bitstream.Write((RakNet::MessageID)ID_GPRO_MESSAGE_COMMON_BEGIN);
		bitstream.Write(message);
		return bitstream;
	}

	RakNet::BitStream& cRakNetManager::ReadTest(RakNet::BitStream& bitstream)
	{
		RakNet::RakString rs;
		bitstream.Read(rs);
		printf("%s\n", rs.C_String());
		return bitstream;
	}

	int cRakNetManager::MessageLoop()
	{
		int count = 0;
		RakNet::Packet* packet = 0;
		RakNet::MessageID msgID = 0;
		RakNet::Time dtSendToReceive = 0;

		while (packet = peer->Receive())
		{
			RakNet::BitStream bitstream(packet->data, packet->length, false);
			bitstream.Read(msgID);

			// process timestamp
			ReadTimestamp(bitstream, dtSendToReceive, msgID);

			// process content
			if (ProcessMessage(bitstream, packet->systemAddress, dtSendToReceive, msgID))
				++count;

			// done with packet
			peer->DeallocatePacket(packet);
		}

		// done
		return count;
	}
}


/* INITIAL INFORMATION FROM QUIZ TO BE EDITED*/

// Description of spatial pose.
struct sSpatialPose
{
	float scale[3];     // non-uniform scale
	float rotate[3];    // orientation as Euler angles
	float translate[3]; // translation

	// read from stream
	RakNet::BitStream& Read(RakNet::BitStream& bitstream)
	{
		/*
		ALT CODE
			send information to decompress function
			send out decompressed data
		*/
		bitstream.Read(scale[0]);
		bitstream.Read(scale[1]);
		bitstream.Read(scale[2]);
		bitstream.Read(rotate[0]);
		bitstream.Read(rotate[1]);
		bitstream.Read(rotate[2]);
		bitstream.Read(translate[0]);
		bitstream.Read(translate[1]);
		bitstream.Read(translate[2]);
		return bitstream;
	}

	// write to stream
	RakNet::BitStream& Write(RakNet::BitStream& bitstream) const
	{
		/*
		ALT CODE
			send information into compressions function
			send compressed data to the bitstream
		*/

		// Example line of call
		bitstream.Write(CompressData(scale[0]));

		bitstream.Write(scale[1]);
		bitstream.Write(scale[2]);
		bitstream.Write(rotate[0]);
		bitstream.Write(rotate[1]);
		bitstream.Write(rotate[2]);
		bitstream.Write(translate[0]);
		bitstream.Write(translate[1]);
		bitstream.Write(translate[2]);
		return bitstream;
	}
};


/* QUICK CODE UNDER THE ASSUMPTION THAT THESE FLOATS WORK FROM ABOUT -1 TO 1 IN USAGE */

// uniform compression size for formula on how to compress and decompress data
int compressionSize = ...;

// compress into ints for a smaller data type than floats
int CompressData(float data)
{
	return (int)(data * compressionSize);
}

float DecompressData(int data)
{
	return (float) data / compressionSize;
}

// This above method can be done with smaller data types like chars
// Smaller data types means less precision though and more data lost in compression


/* ANOTHER OPTION */

// if this spatial data being taken in is based off of player inputs, we can track those instead

struct Inputs {
	bool w;
	bool a;
	bool s;
	bool d;
	bool space;
	bool shoot;
};

// Go on with usual processing, but instead, server and clients take in other inputs and process them as movement on their screens
// Bools would waaaay lower data that's being sent back and forth
// This could put a lot more processing on each player and the server than just simply sending data