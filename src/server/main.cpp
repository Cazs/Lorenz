#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <string.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <typeinfo>
#include <cmath>
#include "lib/sha1.h"
#include <regex>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8082"
#define WEBSOCKET_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define BUF_SIZE 2048
#define BUF_SMALL 100
#define MAX_WIDTH 1366 // 680
#define MAX_HEIGHT 720 // 480

enum ErrorCode
{
	ERROR_NONE,
	ERROR_400_BAD_REQUEST,
	ERROR_404_NOT_FOUND,
	ERROR_MAX,
};

#pragma pack(push, 1)

struct _websocket_header
{
	unsigned char opcode : 4;

	unsigned char rsv3 : 1;
	unsigned char rsv2 : 1;
	unsigned char rsv1 : 1;
	unsigned char fin : 1;

	unsigned char len : 7;
	unsigned char mask : 1;
};

struct _extended_16
{
	unsigned char value[2];
};

struct _extended_64
{
	unsigned char value[8];
};

struct _mask_key
{
	unsigned char value[4];
};

struct _client_pos_
{
	unsigned int x = 0;
	unsigned int y = 0;
};

#pragma pack(pop)

struct WSockClientBullet
{
	std::string bulletKey; // char*
	std::string direction; // char*
	// int id = -1;
	double x = 0;
	double y = 0;
};

struct WSockClient
{
	SOCKET socket;
	char *socketKey;
	int id = -1;
	double x = 300; // TODO: spawn point
	double y = 300; // TODO: spawn point
	double hp = 100;
	double xp = 0;
	int hitCount = 0;
	int killCount = 0;
	std::map<std::string, WSockClientBullet> bullets; // = nullptr;
};

/*struct WorldObject
{
	int id = -1;
	int x = 300; // TODO: spawn point
	int y = 300; // TODO: spawn point
	int width = 100;
	int height = 0;
	int type = -1;
};*/

struct WorldObject
{
	std::string id = "-1";
	std::string x = "-1";
	std::string y = "-1";
	std::string length = "-1";
	std::string width = "-1";
	std::string height = "-1";
	std::string type = "-1";
};

unsigned WINAPI RequestHandler(void *arg);
void LogHandling(int type, std::string message, bool show);

std::map<std::string, WSockClient> clientsMap = {}; // nullptr; // {}; // new std::map<std::string, WSockClient>(); // {0};
std::map<std::string, WorldObject> worldObjects = {}; // nullptr; // {}; // new std::map<std::string, WSockClient>(); // {0};

// WorldObject[] worldObjects = [];
// WorldObject* worldObjects = {};
// std::string* strWorldObjects = {};
std::string strWorldObjects;

// const WorldObject[] getWorldMap() {
// WorldObject* getWorldMap() {
// std::string getWorldMap() {
void setWorldMap() {
	std::ostringstream ssWorldObjects;

	// Create a text string, which is used to output the text file
	std::string myText = "";

	// Read from the text file
	std::ifstream worldMapFile("map1.log");

	// Use a while loop together with the getline() function to read the file line by line
	std::string delimiter = ",";

	for (std::string line; std::getline(worldMapFile, line, ';');) 
	{
		std::cout << line << std::endl;

		if(line.length() > 6) {
			WorldObject worldObject;

			size_t pos = line.find(delimiter);
			std::string token = line.substr(0, pos);
			std::string objectId = (std::string) std::regex_replace(token, std::regex("\n"), "");
			objectId = std::regex_replace(objectId, std::regex("\r"), "");
			objectId = std::regex_replace(objectId, std::regex("\t"), "");
			objectId = std::regex_replace(objectId, std::regex(" "), "");
			worldObject.id = objectId;
			std::cout << "objectId: [" << worldObject.id << "]" << std::endl;
			
			line.erase(0, pos + delimiter.length());
			pos = line.find(delimiter);
			token = line.substr(0, pos);
			std::string objectX = std::regex_replace(token, std::regex("\n"), "");
			objectX = std::regex_replace(objectX, std::regex("\r"), "");
			objectX = std::regex_replace(objectX, std::regex("\t"), "");
			objectX = std::regex_replace(objectX, std::regex(" "), "");
			worldObject.x = objectX;
			std::cout << "objectX: [" << worldObject.x << "]" << std::endl;
			
			line.erase(0, pos + delimiter.length());
			pos = line.find(delimiter);
			token = line.substr(0, pos);
			std::string objectY = std::regex_replace(token, std::regex("\n"), "");
			objectY = std::regex_replace(objectY, std::regex("\r"), "");
			objectY = std::regex_replace(objectY, std::regex("\t"), "");
			objectY = std::regex_replace(objectY, std::regex(" "), "");
			worldObject.y = objectY;
			std::cout << "objectY: [" << worldObject.y << "]" << std::endl;
			
			line.erase(0, pos + delimiter.length());
			pos = line.find(delimiter);
			token = line.substr(0, pos);
			std::string objectLength = std::regex_replace(token, std::regex("\n"), "");
			objectLength = std::regex_replace(objectLength, std::regex("\r"), "");
			objectLength = std::regex_replace(objectLength, std::regex("\t"), "");
			objectLength = std::regex_replace(objectLength, std::regex(" "), "");
			worldObject.length = objectLength;
			std::cout << "objectLength: [" << worldObject.length << "]" << std::endl;
			
			line.erase(0, pos + delimiter.length());
			pos = line.find(delimiter);
			token = line.substr(0, pos);
			std::string objectWidth = std::regex_replace(token, std::regex("\n"), "");
			objectWidth = std::regex_replace(objectWidth, std::regex("\r"), "");
			objectWidth = std::regex_replace(objectWidth, std::regex("\t"), "");
			objectWidth = std::regex_replace(objectWidth, std::regex(" "), "");
			worldObject.width = objectWidth;
			std::cout << "objectWidth: [" << worldObject.width << "]" << std::endl;
			
			line.erase(0, pos + delimiter.length());
			pos = line.find(delimiter);
			token = line.substr(0, pos);
			std::string objectHeight = std::regex_replace(token, std::regex("\n"), "");
			objectHeight = std::regex_replace(objectHeight, std::regex("\r"), "");
			objectHeight = std::regex_replace(objectHeight, std::regex("\t"), "");
			objectHeight = std::regex_replace(objectHeight, std::regex(" "), "");
			worldObject.height = objectHeight;
			std::cout << "objectHeight: [" << worldObject.height << "]" << std::endl;
			
			line.erase(0, pos + delimiter.length());
			pos = line.find(";");
			token = line.substr(0, pos);
			std::string objectType = std::regex_replace(token, std::regex("\n"), "");
			objectType = std::regex_replace(objectType, std::regex("\r"), "");
			objectType = std::regex_replace(objectType, std::regex("\t"), "");
			objectType = std::regex_replace(objectType, std::regex(" "), "");
			worldObject.type = objectType;
			std::cout << "objectType: [" << worldObject.type << "]" << std::endl;

			worldObjects.insert_or_assign(objectId, worldObject);
			
			ssWorldObjects
				<< worldObject.id
				<< ','
				<< worldObject.x
				<< ','
				<< worldObject.y
				<< ','
				<< worldObject.length
				<< ','
				<< worldObject.width
				<< ','
				<< worldObject.height
				<< ','
				<< worldObject.type
				<< ';';
		}
	}

	// Close the file
	// worldMapFile.close();

	// return worldObjects;
	std::cout << "World Objects Count: " << worldObjects.size() << std::endl;
	strWorldObjects = ssWorldObjects.str();
	// return ssWorldObjects.str();
}

// const char* getClientKey(SOCKET ClientSocket, int clientId, char *pKey, char *sendbuf)
const char* getClientKey(SOCKET ClientSocket, char *pKey, char *sendbuf)
{
	// parse just the key part
	pKey = strchr(pKey, ' ') + 1;
	char *pEnd = strchr(pKey, '\r');
	*pEnd = 0;

	LogHandling(0, "    Parsing Client Headers...\n      Client Socket Key: [" + ((std::string) pKey) + ((std::string) "]\n"), true);

	WSockClient wsockClient = clientsMap[pKey]; // || {};
	// wsockClient.socket = ClientSocket; // (SOCKET*) ClientSocket;
	// wsockClient.socketKey = pKey;
	// wsockClient.bullets = {};

	// Initialize random number generator.
	// srand(time(0));

	// wsockClient.id = clientId; // (rand() % 10) + 1;
	/*int cId = (rand() % 10) + 1;
	while(clientsMap[cId].id >= 0) {
		cId = (rand() % 10) + 1;
	}*/
	// wsockClient.id = cId; // (rand() % 10) + 1; // TODO: ID collisions?
	if(wsockClient.id <= 0) {
		wsockClient = {};
		wsockClient.socket = ClientSocket; // (SOCKET*) ClientSocket;
		wsockClient.socketKey = pKey;
		wsockClient.bullets = {};

		srand(time(0));
		// int tempId = (rand() % 10) + 1; // TODO: 10 IDs?
		int tempId = (rand() % 10) + clientsMap.size() + 1; // TODO: 10 IDs?
		// while(tempId ) {}
		wsockClient.id = tempId; // TODO: ID collisions?
	}

	LogHandling(0, "      Set Client Socket ID: [" + std::to_string(wsockClient.id) + "]", true);

	// clientsMap.insert(std::make_pair(pKey, wsockClient));
	clientsMap.insert_or_assign(pKey, wsockClient);
	LogHandling(0, "      Total Connected Clients: [" + std::to_string(clientsMap.size()) + "]\n", true);

	char key[256];
	_snprintf_s(key, _countof(key), "%s%s", pKey, WEBSOCKET_KEY);

	unsigned char result[20];
	const unsigned char *pSha1Key = sha1(key);

	// endian swap each of the 5 ints
	for (int i = 0; i < 5; i++)
	{
		for (int c = 0; c < 4; c++)
			result[i * 4 + c] = pSha1Key[i * 4 + (4 - c - 1)];
	}

	pKey = base64_encode(result, 20);
	LogHandling(0, "      Server Socket Key: [" + std::string(pKey) + ((std::string) "]\n"), true);

	std::ostringstream ss;
	ss << "HTTP/1.1 101 Switching Protocols\r\n"
	   << "Upgrade: websocket\r\n"
	   << "Connection: Upgrade\r\n"
	   << "Sec-WebSocket-Accept: " << pKey // << "\r\n"
	//    << "Client_ID: " << std::to_string(wsockClient.id)// << "\r\n"
	   << "\r\n\r\n";

	std::string bld = ss.str();
	const char *pTemplateResponse = bld.c_str();

	return strdup(pTemplateResponse);

	// std::map<const char *, WSockClient> clientSocketMap = {}; // std::string
	// clientSocketMap.insert_or_assign(strdup(pTemplateResponse), wsockClient);
	
	/*std::map<int, const char*> clientSocketMap = {}; // std::string
	clientSocketMap.insert_or_assign(wsockClient.id, strdup(pTemplateResponse));
	return clientSocketMap;*/
}

void sendClientResponse(SOCKET ClientSocket, size_t sendbuf_size, char *sendbuf, int iSendResult)
{
	LogHandling(0, "    Attempting To Send Response...", true),
	LogHandling(0, "      Has a valid connection? [" + ((std::string) (&ClientSocket == nullptr ? "No]" : "Yes]")), true);

	if (sendbuf_size > 0 && &ClientSocket != nullptr)
	{
		// iSendResult = send((SOCKET) &ClientSocket, sendbuf, (int)sendbuf_size, 0);
		iSendResult = send(ClientSocket, sendbuf, (int) sendbuf_size, 0);

		if (iSendResult == SOCKET_ERROR)
		{	
			LogHandling(2, "       Send failed with error: " + std::to_string(WSAGetLastError()) + ((std::string) "\n"), true);
			// exit(1);
		}
		else
		{
			LogHandling(0, "      Successfully sent [" + std::to_string(iSendResult) + "] chars\n", true);
		}
	}
	else
	{
		LogHandling(1, "      Could NOT successfully send response due to the message content being empty...\n\n", true);
	}
}

// std::string round_up(double value, int decimal_places) {
std::string round_up(double value, double precision = 1.0) {
    // const double multiplier = pow(10.0, decimal_places);
    // return std::to_string(ceil(value * multiplier) / multiplier);
    return std::to_string(std::round(value / precision) * precision);
}

std::string handleBulletMoveToCommand(SOCKET ClientSocket, const char *bulletMoveTo)
{
	std::ostringstream ssAbmt;
	double xOffset = 0;
	double yOffset = 0;
	double xDiff = 0;
	double yDiff = 0;
	double offset = 15; // 10; // 5 // TODO: actor size?  account for bullet speed

	if (bulletMoveTo)
	{
		LogHandling(0, "    Handle [BulletMoveTo] Command...", true);

		bulletMoveTo = strchr(bulletMoveTo, ' ') + 1;
		WSockClient tempCl = {};
		std::string senderKey;

		for (auto &[key, value] : clientsMap)
		{
			if (value.socket == ClientSocket)
			{
				senderKey = key;
				tempCl = value;
				break;
			}
		}

		std::string s(bulletMoveTo);
		std::string delimiter = ",";
		size_t pos = 0;
		std::string token;

		// id
		pos = s.find(delimiter);
		token = s.substr(0, pos);
		std::string bulletId = token; // strdup(token); // (std::string) token.c_str();

		// dir
		s.erase(0, pos + delimiter.length());
		pos = s.find(delimiter);
		token = s.substr(0, pos);
		std::string direction = token; // strdup(token); // (std::string) token.c_str();
		
		// x
		s.erase(0, pos + delimiter.length());
		pos = s.find(delimiter);
		token = s.substr(0, pos);
		std::string xPos = token; // strdup(token); // (std::string) token.c_str();

		// y
		s.erase(0, pos + delimiter.length());
		pos = s.find(";");
		token = s.substr(0, pos);
		std::string yPos = token; // s.substr(0, 3); // strdup(token); // (std::string) token.c_str();

		// std::cout << "bullet move to s remainder: [" << s << "]" << std::endl;

		WSockClientBullet wsockClientBullet = {}; // clientsMap[tempCl.socketKey].bullets[bulletId] ||
		wsockClientBullet.bulletKey = bulletId;
		wsockClientBullet.direction = direction;
		wsockClientBullet.x = std::stod(xPos);
		wsockClientBullet.y = std::stod(yPos);

		// if(wsockClientBullet.x > MAX_WIDTH) {}

		LogHandling(0, "      (bulletMoveTo) Insert or Assign Bullet: [id: " + bulletId + ", source client: " + std::to_string(tempCl.id) + ", direction: " + direction + ", x: " + xPos + ", y: " + yPos + "]", true);
		clientsMap[tempCl.socketKey].bullets.insert_or_assign(bulletId, wsockClientBullet);

		// ssAbmt << "ACK_BULLET_MOVETO:" << bulletId << "," << std::to_string(tempCl.id) << "," << direction << "," << std::to_string(xPos) << "," << std::to_string(yPos) << "|";
		ssAbmt << "ACK_BULLET_MOVETO:" << bulletId << "," << std::to_string(tempCl.id) << "," << direction << "," << xPos << "," << yPos << "|";

		LogHandling(0, "      (bulletMoveTo) Bullet Count: [" + std::to_string(clientsMap[tempCl.socketKey].bullets.size()) + "]", true);

		WSockClient tempCl2 = {};
		// check collisions
		for (auto &[cKey, cValue] : clientsMap)
		{
			tempCl2 = cValue; // .socket;
			// if (cValue.socket != wsockClientBullet) {
			if (tempCl2.socketKey != tempCl.socketKey) {
				// tempCl2 = cValue; // .socket;

				// xDiff = abs(tempCl2.x % wsockClientBullet.x);
				// yDiff = abs(tempCl2.y % wsockClientBullet.y);

				// xDiff = abs(tempCl2.x - wsockClientBullet.x);
				// yDiff = abs(tempCl2.y - wsockClientBullet.y);
				
				xDiff = abs(abs(tempCl2.x) - abs(wsockClientBullet.x));
				yDiff = abs(abs(tempCl2.y) - abs(wsockClientBullet.y));

				// xDiff = abs(abs(clientsMap[tempCl2.socketKey].x) - abs(clientsMap[tempCl.socketKey].x));
				// yDiff = abs(abs(clientsMap[tempCl2.socketKey].y) - abs(clientsMap[tempCl.socketKey].y));

				LogHandling(1, "      (bulletMoveTo) xDiff: [" + std::to_string(xDiff) + "]", true);
				LogHandling(1, "      (bulletMoveTo) yDiff: [" + std::to_string(yDiff) + "]", true);
				
				if((xDiff <= offset) && (yDiff <= offset) || (wsockClientBullet.x <= 0 || wsockClientBullet.x >= MAX_WIDTH) || (wsockClientBullet.y <= 0 || wsockClientBullet.y >= MAX_HEIGHT)) {
					// update src stats
					clientsMap[tempCl.socketKey].hitCount += 1; // TODO: hit count decrease multiplier?
					// update dest stats
					clientsMap[cKey].hp -= 10; // TODO: HP decrease multiplier?

					// reset player if 0 HP
					if(clientsMap[cKey].hp <= 0) {
						clientsMap[tempCl.socketKey].killCount += 1;
						clientsMap[tempCl.socketKey].xp += tempCl.hitCount * (clientsMap[tempCl.socketKey].killCount + 0.3); // TODO: multiplier
						clientsMap[cKey].hp = 100; // TODO: ENum for max HP?
						// TODO: HP+ or buffs?
					}

					ssAbmt
						<< std::to_string(tempCl.id) << "+" 
						<< round_up(clientsMap[tempCl.socketKey].hp, 2.0) << "+"
						<< round_up(clientsMap[tempCl.socketKey].xp, 2.0) << "+"
						<< std::to_string(clientsMap[tempCl.socketKey].hitCount) << "+"
						<< std::to_string(clientsMap[tempCl.socketKey].killCount) 
						<< "@"
						<< std::to_string(clientsMap[cKey].id) << "+"
						<< round_up(clientsMap[cKey].hp, 2.0) << "+"
						<< round_up(clientsMap[cKey].xp, 2.0) << "+"
						<< std::to_string(clientsMap[cKey].hitCount) << "+"
						<< std::to_string(clientsMap[cKey].killCount) 
						<< ",";

					LogHandling(1, "      (bulletMoveTo) xDiff: [" + std::to_string(xDiff) + "]", true);
					LogHandling(1, "      (bulletMoveTo) yDiff: [" + std::to_string(yDiff) + "]", true);
					LogHandling(1, "      (bulletMoveTo) Bullet impacted: [" + std::to_string(tempCl2.id) + "]\n", true);
					// TODO: remove impacted bullets
					LogHandling(1, "      (bulletMoveTo) Bullet Count Before: [" + std::to_string(clientsMap[tempCl.socketKey].bullets.size()) + "]", true);
					clientsMap[tempCl.socketKey].bullets.erase(bulletId);
					LogHandling(1, "      (bulletMoveTo) Bullet Count After: [" + std::to_string(clientsMap[tempCl.socketKey].bullets.size()) + "]", true);
				}
			}
		}
	}

	// std::string bldAbmt = ssAbmt.str();
	// return ssAbmt ? strdup(((std::string)ssAbmt.str()).c_str()) : nullptr;
	// return ssAbmt ? strdup(((std::string) ssAbmt.str())) : nullptr;
	return ssAbmt ? ssAbmt.str() : nullptr;
	// return ssAbmt ? strdup(((std::string) ssAbmt.str()).c_str()) : "";
}

std::string handleClientInitCommand(SOCKET ClientSocket, const char *clientInit)
{
	std::ostringstream ssAbmt;

	if (clientInit)
	{
		LogHandling(1, "\n    Handle [ClientInit] Command...", true);
		// clientInit = strchr(clientInit, ' ') + 1;
		WSockClient tempCl = {};

		// MAP::const_iterator pos = worldObjects.find("1");
		// std::map<std::string, WorldObject> pos = worldObjects.find("1");

		// std::map<std::string, WorldObject>::iterator pos = worldObjects.find("1");
		// std::cout << "pos: " << pos << std::endl;

		/*if (pos == worldObjects.end()) {
			//handle the error
			LogHandling(3, "World Object not found.", true);
		}*/

		for (auto &[key, value] : clientsMap)
		{
			if (value.socket == ClientSocket)
			{
				LogHandling(1, "\n      [ClientInit] found client: " + key, true);
				std::cout << "World Objects Count2: " << worldObjects.size() << std::endl;

				// std::string value = pos->second;

				tempCl = value;
				ssAbmt
					<< "ACK_CLIENT_INIT:"
					<< clientsMap[tempCl.socketKey].id
					<< ','
					<< clientsMap[tempCl.socketKey].x
					<< ','
					<< clientsMap[tempCl.socketKey].y
					<< '|'
					<< strWorldObjects;
					// << pos->id;
					/* << worldObjects[1].x
					// << worldObjects["1"].x
					// << std::to_string(worldObjects["1"].x)
					<< ','
					<< worldObjects[1].y
					// << worldObjects["1"].y
					// << std::to_string(worldObjects["1"].y)
					<< ','
					<< worldObjects[1].width
					// << worldObjects["1"].width
					// << std::to_string(worldObjects["1"].width)
					<< ','
					<< worldObjects[1].height
					// << worldObjects["1"].height
					// << std::to_string(worldObjects["1"].height)
					<< ','
					<< worldObjects[1].type
					// << worldObjects["1"].type
					// << std::to_string(worldObjects["1"].type)
					<< ';';*/
					// << std::to_string(worldObjects[1].id);
					// << strWorldObjects;
					 //  << "," << clientsMap[tempCl.socketKey].x << "," << clientsMap[tempCl.socketKey].y;

				/*for (auto &[wkey, wvalue] : worldObjects) {
					std::cout << "wkey: " << wkey << std::endl;
					std::cout << "wvalue x: " << wvalue.x << std::endl;
					std::cout << "wvalue y: " << wvalue.y << std::endl;
					ssAbmt << worldObjects[wkey].id << ',';
					// ssAbmt << std::to_string(&std::to_integer(worldObjects[wkey].id)) << ',';
					// ssAbmt << (int) worldObjects[wkey].id << ',';
					// ssAbmt << (std::string) wvalue.id << ",";
					// ssAbmt << worldObjects[wkey].id << ","
					// ssAbmt << worldObjects[wvalue.id].id << ","
				}*/

				return ssAbmt.str();
				// return strdup(((std::string)ssAbmt.str()).c_str());
				// return ssAbmt.str();
			}
		}
	}

	// return ssAbmt ? strdup(((std::string)ssAbmt.str()).c_str()) : nullptr;
	return nullptr;
}

std::string handleClientMoveToCommand(SOCKET ClientSocket, const char *moveTo)
{
	std::ostringstream ssAbmt;

	if (moveTo)
	{
		LogHandling(0, "\n    Handle [ClientMoveTo] Command...", true);
		moveTo = strchr(moveTo, ' ') + 1;
		WSockClient tempCl = {};

		for (auto &[key, value] : clientsMap)
		{
			if (value.socket == ClientSocket)
			{
				tempCl = value;
				break;
			}
		}

		std::string s(moveTo);
		std::string delimiter = ",";
		size_t pos = 0;
		std::string token;

		pos = s.find(delimiter);
		token = s.substr(0, pos);
		// const std::string xPos = token;
		std::string xPos = token; // strdup(token); // (std::string) token.c_str();
		
		/*s.erase(0, pos + delimiter.length());
		// const std::string yPos = s;
		pos = s.find(delimiter);
		token = s.substr(0, pos);
		std::string yPos = token; // strdup(token); // (std::string) token.c_str();*/

		s.erase(0, pos + delimiter.length());
		pos = s.find(";");
		token = s.substr(0, pos);
		std::string yPos = token;

		// if ((std::stod(xPos) >= 0 && std::stod(yPos) >= 0) && (std::stod(xPos) < MAX_WIDTH && std::stod(yPos) < MAX_HEIGHT))
		if ((std::stod(xPos) >= 0 && std::stod(yPos) >= 0))
		{
			clientsMap[tempCl.socketKey].x = std::stod(xPos);
			clientsMap[tempCl.socketKey].y = std::stod(yPos);
			ssAbmt << "ACK_CLIENT_MOVETO:" << clientsMap[tempCl.socketKey].id << "," << xPos << "," << yPos << "#";

			// WSockClient tempCl = value;
			// ssAbmt << value.id << "," << value.x << "," << value.y << "#";
			/*for (auto &[index, bullet] : clientsMap[tempCl.socketKey].bullets)
			{
				WSockClientBullet wsockClientBullet = bullet;
				LogHandling(1, "        Bullet Coords [id: " + wsockClientBullet.bulletKey + ", direction: " + wsockClientBullet.direction + ", x: " +  std::to_string(wsockClientBullet.x) + ", y: " + std::to_string(wsockClientBullet.y) + "]", true);
				ssAbmt << wsockClientBullet.bulletKey << "," << wsockClientBullet.direction << "," << wsockClientBullet.x << "," << wsockClientBullet.y << ","; // << "";
			}*/
		}
		else
		{
			ssAbmt << "NACK_CLIENT_MOVETO:" << clientsMap[tempCl.socketKey].id << "," << xPos << "," << yPos;
		}
	}

	// return ssAbmt ? strdup(((std::string)ssAbmt.str()).c_str()) : nullptr;
	// return ssAbmt ? strdup(((std::string) ssAbmt.str())) : nullptr;
	return ssAbmt ? ssAbmt.str() : nullptr;
}

void handleClientCommand(SOCKET ClientSocket, char recvbuf[DEFAULT_BUFLEN], char *sendbuf, size_t sendbuf_size, int iSendResult)
{
	LogHandling(0, "\n  Handle Client Command...\n", true);
	// char* text = (char*) "ACK_CMD";
	std::string text = (std::string) "ACK_CMD";

	_websocket_header *h = (_websocket_header *) recvbuf;
	_mask_key *mask_key;
	unsigned long long length;

	if (h->len < 126)
	{
		length = h->len;
		mask_key = (_mask_key *)(recvbuf + sizeof(_websocket_header));
	}
	else if (h->len == 126)
	{
		_extended_16 *extended = (_extended_16 *)(recvbuf + sizeof(_websocket_header));
		length = (extended->value[0] << 8) | extended->value[1];
		mask_key = (_mask_key *)(recvbuf + sizeof(_websocket_header) + sizeof(_extended_16));
	}
	else
	{
		_extended_64 *extended = (_extended_64 *)(recvbuf + sizeof(_websocket_header));
		length = (((unsigned long long)extended->value[0]) << 56) | (((unsigned long long)extended->value[1]) << 48) | (((unsigned long long)extended->value[2]) << 40) |
				 (((unsigned long long)extended->value[3]) << 32) | (((unsigned long long)extended->value[4]) << 24) | (((unsigned long long)extended->value[5]) << 16) |
				 (((unsigned long long)extended->value[6]) << 8) | (((unsigned long long)extended->value[7]) << 0);
		mask_key = (_mask_key *)(recvbuf + sizeof(_websocket_header) + sizeof(_extended_64));
	}

	char *client_msg = ((char *)mask_key) + sizeof(_mask_key);

	if (h->mask)
	{
		for (int i = 0; i < length; i++)
			client_msg[i] = client_msg[i] ^ mask_key->value[i % 4];
	}

	const char *clientInit = strstr(client_msg, "ClientInit");
	const char *clientMoveTo = strstr(client_msg, "ClientMoveTo:");
	const char *bulletMoveTo = strstr(client_msg, "BulletMoveTo:");
	const char *getClients = strstr(client_msg, "GetClients:");

	if (clientInit)
	{
		// text = (char*) "ACK_CLIENT_INIT";
		const std::string msg = handleClientInitCommand(ClientSocket, clientInit);
		if (msg.size() > 0)
		{
			text = (std::string) msg;
		}
		else
		{
			text = (std::string) "NACK_CLIENT_INIT";
		}
	}
	
	if (clientMoveTo)
	{
		// text = (char*) "ACK_CLIENT_CMD";
		const std::string msg = handleClientMoveToCommand(ClientSocket, clientMoveTo);
		if (msg.size() > 0)
		{
			text = (std::string) msg;
		}
		else
		{
			text = (std::string) "NACK_CLIENT_MOVETO";
		}
	}

	if (bulletMoveTo)
	{
		const std::string msg = handleBulletMoveToCommand(ClientSocket, bulletMoveTo);
		if (msg.size() > 0)
		{
			text = (std::string)msg;
		}
		else
		{
			text = (std::string) "NACK_BULLET_MOVETO";
		}
	}

	char *pData;
	h = (_websocket_header *)sendbuf;
	*h = _websocket_header{};
	h->opcode = 0x1; // 0x1 = text, 0x2 = blob
	h->fin = 1;

	// bullet logic
	// if(text == nil || strlen(text.c_str()) <= 0) {
	/*if (text.size() <= 0) {
		std::ostringstream ss;

		// iterate using C++17 facilities
		for (auto &[key, value] : clientsMap) {
			if (clientsMap[value.socketKey].socketKey) {
				if (value.socket != ClientSocket) {
					const WSockClient client = clientsMap[value.socketKey];
					ss << client.id << " ," << client.x << " ," << client.y << ":[";

					for (auto &[k, v] : client.bullets) {
						if (v.x >= 0 && v.y >= 0) {
							ss << k << "," << v.x << "," << v.y << ";";
						}
					}

					ss << "]|";
				}
			}
		}*

		// std::string bld = ss.str();
		// text = (char*) bld.c_str();
		text = ss.str();*
	}*/

	LogHandling(0, "    Building Client Response...", true);
	LogHandling(0,"      response text: [" + text + "]", true);

	unsigned long long msg_length = text.size();
	sendbuf_size = sizeof(_websocket_header);

	if (msg_length <= 125) {
		LogHandling(0, "      Handle [msg_length <= 125]", true);
		pData = sendbuf + sizeof(_websocket_header);
		h->len = msg_length;
	} else if (msg_length <= 0xffff) {
		LogHandling(0, "      Handle [msg_length <= 0xffff]", true);
		h->len = 126;

		_extended_16 *extended = (_extended_16 *)(sendbuf + sendbuf_size);
		sendbuf_size += sizeof(_extended_16);

		extended->value[0] = (msg_length >> 8) & 0xff;
		extended->value[1] = msg_length & 0xff;
	} else {
		LogHandling(0, "      Handle [msg_length is else]", true);
		h->len = 127;

		_extended_64 *extended = (_extended_64 *)(sendbuf + sendbuf_size);
		sendbuf_size += sizeof(_extended_64);

		extended->value[0] = ((msg_length >> 56) & 0xff);
		extended->value[1] = ((msg_length >> 48) & 0xff);
		extended->value[2] = ((msg_length >> 40) & 0xff);
		extended->value[3] = ((msg_length >> 32) & 0xff);
		extended->value[4] = ((msg_length >> 24) & 0xff);
		extended->value[5] = ((msg_length >> 16) & 0xff);
		extended->value[6] = ((msg_length >> 8) & 0xff);
		extended->value[7] = ((msg_length >> 0) & 0xff);
	}

	pData = sendbuf + sendbuf_size;
	// LogHandling(0, "        pData address: [" + std::to_string(&pData) + "]", true);

	memcpy(pData, text.c_str(), (size_t) msg_length);
	LogHandling(0, "        msg_length: [" + std::to_string(msg_length) + "] chars", true);

	sendbuf_size += (size_t) msg_length;
	LogHandling(0, "        sendbuf_size: [" + std::to_string(sendbuf_size) + "] chars", true);

	std::cout << "\n\n" << "Send response: " << text << "\n\n" << std::endl;

	// sendClientResponse(ClientSocket, sendbuf_size, sendbuf, iSendResult);
	if(clientInit) {
		sendClientResponse(ClientSocket, sendbuf_size, sendbuf, iSendResult);
	} else {
		// update all other clients
		LogHandling(0, "        updating [" + std::to_string(clientsMap.size()) + "] clients...", true);

		for (auto &[key, value] : clientsMap)
		{
			// if (!bulletMoveTo && value.socket != ClientSocket)
			// if (!clientMoveTo && value.socket != ClientSocket)
			// if (!clientMoveTo && typeid(&value.socket).name() == SOCKET && value.id > 0)
			// if (!clientMoveTo && (&value.socket != nullptr) && value.id > 0)
			// if (!clientMoveTo && value != nullptr && (value.socket != nullptr) && value.id > 0)
			// if (!clientMoveTo && value != nullptr && (value.socket != nullptr))
			// if (!clientMoveTo && (value.socket != nullptr))
			// if (!clientMoveTo && (value.id > 0))
			if ((value.id > 0)) // || (value.id.size() > 0))
			{
				sendClientResponse(value.socket, sendbuf_size, sendbuf, iSendResult);
				// WSockClient tempCl = value;
				// ssAbmt << value.id << "," << value.x << "," << value.y << "#";
				/*for (auto &[index, bullet] : clientsMap[tempCl.socketKey].bullets)
				{
					WSockClientBullet wsockClientBullet = bullet;
					LogHandling(0, "        Bullet Coords [id: " + wsockClientBullet.bulletKey + ", x: " +  std::to_string(wsockClientBullet.x) + ", y: " + std::to_string(wsockClientBullet.y) + "]", true);
					ssAbmt << wsockClientBullet.bulletKey << "," << wsockClientBullet.x << "," << wsockClientBullet.y << ","; // << "";
				}*/
			} else {
				// clientsMap.erase([key, value]);
				clientsMap.erase(key);
			}
		}
	}
}

int processConnection(SOCKET ClientSocket)
{
	int iResult;
	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	while (true) {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

		if (iResult > 0) {
			LogHandling(0, "\n\n  Received [" + std::to_string(iResult) + "] bytes...\n", true);

			recvbuf[iResult] = 0;
			char sendbuf[1024] = {0};
			size_t sendbuf_size = 0;

			// see if it's requesting a key
			char *pKey = strstr(recvbuf, "Sec-WebSocket-Key:");

			if (pKey) {
				// Handle Client Key Request
				LogHandling(0, "\n  Handle Client Key Request...\n", true);

				const char *pTemplateResponse = getClientKey(ClientSocket, (char *) pKey, sendbuf);

				// Initialize random number generator.
				// srand(time(0));
				// int clientId = (rand() % 10) + 1;
				// LogHandling(0, "      Generated Client ID: [" + std::to_string(clientId) + "]", true);
				// const char *pTemplateResponse = getClientKey(ClientSocket, clientId, (char *) pKey, sendbuf);

				// const std::map<int, const char*> clientSocketData = getClientKey(ClientSocket, (char *) pKey, sendbuf);
				// const char *pTemplateResponse = clientSocketData[0];

				_snprintf_s(sendbuf, _countof(sendbuf), pTemplateResponse);
				sendbuf_size = strlen(sendbuf);

				sendClientResponse(ClientSocket, sendbuf_size, sendbuf, iSendResult);
			} else {
				// Handle Other Client Requests
				char *text = (char *)"ACK_CLIENT_CMD"; // TODO: return client ID
				handleClientCommand(ClientSocket, recvbuf, sendbuf, sendbuf_size, iSendResult);
			}
		}
	}

	return iResult;
}

int main(int argc, char *argv[]) {
	// worldObjects = getWorldMap();
	// strWorldObjects = getWorldMap();
	// std::cout << "[debug] World Map: " << strWorldObjects << std::endl;
	setWorldMap();
	std::cout << "World Objects Count: " << worldObjects.size() << std::endl;

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;

	HANDLE hThread;
	DWORD dwThreadId;
	int clntAddrLen;

	if (argc != 2) {
		LogHandling(0, "[debug] 인자값 이상. port값 넣으세요", true);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		LogHandling(0, "[debug] WSAStartup() Error", true);
	}

	hServSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (hServSock == INVALID_SOCKET) {
		LogHandling(0, "[debug] WSASocket() Error", true);
	}

	ZeroMemory(&servAddr, sizeof(servAddr));

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR *) &servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
		LogHandling(0, "[debug] bind() Error", true);
	}

	if (listen(hServSock, 5) == SOCKET_ERROR) {
		LogHandling(0, "[debug] listen() Error", true);
	}

	LogHandling(0, "\n\n********** Server listening for connections on: [ws://" + ((std::string) inet_ntoa(servAddr.sin_addr)) + ":" + std::to_string(ntohs(servAddr.sin_port)) + "] **********\n\n\n", true);

	while (1) {
		clntAddrLen = sizeof(clntAddr);
		hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &clntAddrLen);
		if (hClntSock == INVALID_SOCKET) {
			LogHandling(0, "[debug] Accept() Error", true);
		}

		LogHandling(0, ">>> Connection Request From: [ws://" + ((std::string) inet_ntoa(clntAddr.sin_addr)) + ":" + std::to_string(ntohs(clntAddr.sin_port)) + "] <<<\n\n", true);

		hThread = (HANDLE)_beginthreadex(NULL, 0, RequestHandler, (LPVOID)hClntSock, 0, (unsigned *)&dwThreadId);
		if (hThread == INVALID_HANDLE_VALUE) {
			LogHandling(0, "[debug] _beginthreadex() error", true);
		}
	}
	// closesocket(hServSock);
	// WSACleanup();
	// return 0;
}

unsigned WINAPI RequestHandler(void *arg)
{
	SOCKET hClntSock = (SOCKET) arg;
	return processConnection(hClntSock);
	// return 0;
}

void LogHandling(int type, const std::string message, bool show)
{
	if (show)
	{
		switch (type)
		{
			case 0:
				fputs((const char*) "[info] ", stdout);
				fputs(message.c_str(), stdout);
				fputc('\n', stdout);
				break;
			case 1:
				fputs((const char*) "[warning] ", stdout);
				fputs(message.c_str(), stdout);
				fputc('\n', stdout);
				break;
			case 2:
				fputs((const char*) "[error] ", stderr);
				fputs(message.c_str(), stderr);
				fputc('\n', stderr);
				break;
			default:
				break;
		}
	}
	// exit(1);
}