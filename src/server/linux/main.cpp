#define WIN32_LEAN_AND_MEAN

// #include <windows.h>
// #include <winsock2.h>
// #include <ws2tcpip.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>

#include <fcntl.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <stdint.h>
    #ifndef _SOCKET_T_DEFINED
        typedef int socket_t;
        #define _SOCKET_T_DEFINED
    #endif
    #ifndef INVALID_SOCKET
        #define INVALID_SOCKET (-1)
    #endif
    #ifndef SOCKET_ERROR
        #define SOCKET_ERROR   (-1)
    #endif
    #define closesocket(s) ::close(s)
    #include <errno.h>
    #define socketerrno errno
    #define SOCKET_EAGAIN_EINPROGRESS EAGAIN
    #define SOCKET_EWOULDBLOCK EWOULDBLOCK

#include <thread>
#include <sstream>
#include <map>
// #include "../lib/sha1.h";
#include "../lib/sha1.cpp";

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8082"
#define WEBSOCKET_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define BUF_SIZE 2048
#define BUF_SMALL 100
#define MAX_WIDTH 680
#define MAX_HEIGHT 480

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
	double x = 30;
	double y = 30;
	std::map<std::string, WSockClientBullet> bullets; // = {};
													  // WSockClientBullet bullets[100];
													  // std::map<int, WSockClientBullet> bullets = new std::map<int, WSockClientBullet>({});
};

unsigned WINAPI RequestHandler(void *arg);
void ErrorHandling(char *message);
void LogHandling(int type, std::string message, bool show);

std::map<std::string, WSockClient> clientsMap = {}; // new std::map<std::string, WSockClient>(); // {0};

char *getClientKey(SOCKET ClientSocket, char *pKey, char *sendbuf)
{
	// parse just the key part
	pKey = strchr(pKey, ' ') + 1;
	char *pEnd = strchr(pKey, '\r');
	*pEnd = 0;

	LogHandling(0, "    Parsing Client Headers...\n      Client Socket Key: [" + ((std::string) pKey) + ((std::string) "]\n"), true);

	WSockClient wsockClient;
	wsockClient.socket = ClientSocket;
	wsockClient.socketKey = pKey;
	wsockClient.bullets = {};

	// Initialize random number generator.
	srand(time(0));
	wsockClient.id = (rand() % 10) + 1;
	LogHandling(0, "      Client Socket ID: [" + std::to_string(wsockClient.id) + "]", true);

	// clientsMap.insert(std::make_pair(pKey, wsockClient));
	clientsMap.insert_or_assign(pKey, wsockClient);
	LogHandling(0, "      Connected Clients Count: [" + std::to_string(clientsMap.size()) + "]\n", true);

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
	   << "Sec-WebSocket-Accept: "
	   << pKey
	   << "\r\n\r\n";

	std::string bld = ss.str();
	const char *pTemplateResponse = bld.c_str();

	return strdup(pTemplateResponse);
}

void sendClientResponse(SOCKET ClientSocket, size_t sendbuf_size, char *sendbuf, int iSendResult)
{
	LogHandling(0, "    Attempting To Send Response...", true),
	LogHandling(0, "      Has a valid connection? [" + ((std::string) (&ClientSocket == nullptr ? "No]" : "Yes]")), true);

	if (sendbuf_size > 0)
	{
		iSendResult = send(ClientSocket, sendbuf, (int)sendbuf_size, 0);

		if (iSendResult == SOCKET_ERROR)
		{	
			LogHandling(2, "       Send failed with error: " + std::to_string(WSAGetLastError()) + ((std::string) "\n"), true);
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

std::string handleBulletMoveToCommand(SOCKET ClientSocket, const char *bulletMoveTo)
{
	std::ostringstream ssAbmt;

	if (bulletMoveTo)
	{
		LogHandling(0, "    Handle [BulletMoveTo] Command...", true);

		bulletMoveTo = strchr(bulletMoveTo, ' ') + 1;
		WSockClient tempCl;

		for (auto &[key, value] : clientsMap)
		{
			if (value.socket == ClientSocket)
			{
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
		std::string bulletId = (std::string) token.c_str();

		// dir
		s.erase(0, pos + delimiter.length());
		pos = s.find(delimiter);
		token = s.substr(0, pos);
		std::string direction = (std::string) token.c_str();
		
		// x
		s.erase(0, pos + delimiter.length());
		pos = s.find(delimiter);
		token = s.substr(0, pos);
		std::string xPos = (std::string) token.c_str();

		// y
		s.erase(0, pos + delimiter.length());
		std::string yPos = (std::string) s.c_str();

		/*if(clientsMap[tempCl.socketKey].bullets.length() > 0) {
			WSockClientBullet wsockClientBullet = clientsMap[tempCl.socketKey].bullets[bulletId];
			std::cout << "      (bulletMoveTo) Move Bullet To: [id: " << bulletId << ", x: " << xPos << ", y: " << yPos << "]" << std::endl;
			wsockClientBullet.x = std::stod(xPos);
			wsockClientBullet.y = std::stod(yPos);

			ssAbmt << "ACK_BULLET_MOVETO:" << bulletId << "," << xPos << "," << yPos;
		} else {*/

		WSockClientBullet wsockClientBullet = clientsMap[tempCl.socketKey].bullets[bulletId];
		wsockClientBullet.bulletKey = bulletId;
		wsockClientBullet.direction = direction;
		wsockClientBullet.x = std::stod(xPos);
		wsockClientBullet.y = std::stod(yPos);

		LogHandling(0, "      (bulletMoveTo) Insert or Assign Bullet: [id: " + bulletId + ", direction: " + direction + ", x: " + xPos + ", y: " + yPos + "]", true);
		clientsMap[tempCl.socketKey].bullets.insert_or_assign(bulletId, wsockClientBullet); // insert(std::make_pair(bulletId, wsockClientBullet));

		ssAbmt << "ACK_BULLET_MOVETO:" << bulletId << "," << direction << "," << xPos << "," << yPos;
		//}

		LogHandling(0, "      (bulletMoveTo) Bullet Count: [" + std::to_string(clientsMap[tempCl.socketKey].bullets.size()) + "]", true);
		// printf("      (bulletMoveTo) Bullet Count: [%d]\n", clientsMap[tempCl.socketKey].bullets.size());

		/*if(tempCl.socketKey && clientsMap[tempCl.socketKey].id) {
			WSockClientBullet wsockClientBullet = clientsMap[tempCl.socketKey].bullets[bulletId];
			if(bulletId.size() > 0) {
				if((std::stod(xPos) >= 0 && std::stod(yPos) >= 0) && (std::stod(xPos) < MAX_WIDTH && std::stod(yPos) < MAX_HEIGHT)) {
					wsockClientBullet.id = bulletId;
					wsockClientBullet.x = std::stod(xPos);
					wsockClientBullet.y = std::stod(yPos);

					clientsMap[tempCl.socketKey].bullets.insert_or_assign(bulletId, wsockClientBullet);
					ssAbmt << "ACK_BULLET_MOVETO:" << bulletId << "," << xPos << "," << yPos;
				} else {
					ssAbmt << "NACK_BULLET_MOVETO:" << bulletId << "," << xPos << "," << yPos; // bullet pos
					std::string bldAbmt = ssAbmt.str();
					clientsMap[tempCl.socketKey].bullets.erase(bulletId);
				}
			} else {
				ssAbmt << "NACK_BULLET_MOVETO:" << bulletId << "," << xPos << "," << yPos; // bullet
			}
		} else {
			ssAbmt << "NACK_BULLET_MOVETO:" << bulletId << "," << xPos << "," << yPos; // client
		}*/
	}

	// std::string bldAbmt = ssAbmt.str();
	return ssAbmt ? strdup(((std::string)ssAbmt.str()).c_str()) : nullptr;
}

std::string handleClientMoveToCommand(SOCKET ClientSocket, const char *moveTo)
{
	std::ostringstream ssAbmt;

	if (moveTo)
	{
		LogHandling(0, "\n    Handle [ClientMoveTo] Command...", true);
		moveTo = strchr(moveTo, ' ') + 1;
		WSockClient tempCl;

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
		const std::string xPos = token;
		s.erase(0, pos + delimiter.length());
		const std::string yPos = s;

		// if ((std::stod(xPos) >= 0 && std::stod(yPos) >= 0) && (std::stod(xPos) < MAX_WIDTH && std::stod(yPos) < MAX_HEIGHT))
		if ((std::stod(xPos) >= 0 && std::stod(yPos) >= 0))
		{
			clientsMap[tempCl.socketKey].x = std::stod(xPos);
			clientsMap[tempCl.socketKey].y = std::stod(yPos);
			ssAbmt << "ACK_CLIENT_MOVETO:" << clientsMap[tempCl.socketKey].id << "," << xPos << "," << yPos << "#";

			// WSockClient tempCl = value;
			// ssAbmt << value.id << "," << value.x << "," << value.y << "#";
			for (auto &[index, bullet] : clientsMap[tempCl.socketKey].bullets)
			{
				WSockClientBullet wsockClientBullet = bullet;
				LogHandling(1, "        Bullet Coords [id: " + wsockClientBullet.bulletKey + ", direction: " + wsockClientBullet.direction + ", x: " +  std::to_string(wsockClientBullet.x) + ", y: " + std::to_string(wsockClientBullet.y) + "]", true);
				ssAbmt << wsockClientBullet.bulletKey << "," << wsockClientBullet.direction << "," << wsockClientBullet.x << "," << wsockClientBullet.y << ","; // << "";
			}
		}
		else
		{
			ssAbmt << "NACK_CLIENT_MOVETO:" << clientsMap[tempCl.socketKey].id << "," << xPos << "," << yPos;
		}
	}

	return ssAbmt ? strdup(((std::string)ssAbmt.str()).c_str()) : nullptr;
}

std::string handleGetClientsCommand(SOCKET ClientSocket, const char *getClients)
{
	std::ostringstream ssAbmt;

	if (getClients)
	{
		LogHandling(0, "    Handle [GetClients] Command...", true);
		getClients = strchr(getClients, ' ') + 1;
		// WSockClient tempCl;

		// std::map<std::string, WSockClient> clientsMap
		for (auto &[key, value] : clientsMap)
		{
			if (value.socket != ClientSocket)
			{
				// tempCl = value;
				// break;
				WSockClient tempCl = value;
				ssAbmt << value.id << "," << value.x << "," << value.y << "#";
				// WSockClientBullet wsockClientBullets[] = clientsMap[tempCl.socketKey].bullets; // [bulletId];
				for (auto &[index, bullet] : clientsMap[tempCl.socketKey].bullets)
				{
					WSockClientBullet wsockClientBullet = bullet;
					LogHandling(0, "        Bullet Coords [id: " + wsockClientBullet.bulletKey + ", direction: " + wsockClientBullet.direction + ", x: " +  std::to_string(wsockClientBullet.x) + ", y: " + std::to_string(wsockClientBullet.y) + "]", true);
					ssAbmt << wsockClientBullet.bulletKey << "," << wsockClientBullet.direction << "," << wsockClientBullet.x << "," << wsockClientBullet.y << ","; // << "";
				}
				/*if((value.id >= 0 && value.x >= 0 && value.y >= 0)) { //  && (std::stod(xPos) < MAX_WIDTH && std::stod(yPos) < MAX_HEIGHT)
					// clientsMap[tempCl.socketKey].x = std::stod(xPos);
					// clientsMap[tempCl.socketKey].y = std::stod(yPos);
					// ssAbmt << std::stod(value.id) << "," << std::stod(value.x) << "," << std::stod(value.y) << ";";
					ssAbmt << value.id << "," << value.x << "," << value.y << "#";

					for(auto &[bkey, bvalue] : ((WSockClient) value).bullets) {
					// for(auto &[bkey, bvalue] : value.bullets) {
						ssAbmt << bvalue.id << "," << bvalue.x << "," << bvalue.y << ","; // << "";
						/*if((std::stod(xPos) >= 0 && std::stod(yPos) >= 0) && (std::stod(xPos) < MAX_WIDTH && std::stod(yPos) < MAX_HEIGHT)) {
							wsockClientBullet.id = bulletId;
							wsockClientBullet.x = std::stod(xPos);
							wsockClientBullet.y = std::stod(yPos);

							clientsMap[tempCl.socketKey].bullets.insert_or_assign(bulletId, wsockClientBullet);
							ssAbmt << "ACK_BULLET_MOVETO:" << bulletId << "," << xPos << "," << yPos;
						} else {
							ssAbmt << "NACK_BULLET_MOVETO:" << bulletId << "," << xPos << "," << yPos; // bullet pos
							std::string bldAbmt = ssAbmt.str();
							clientsMap[tempCl.socketKey].bullets.erase(bulletId);
						}*
					}

					ssAbmt << ";";
				} else {
					// ssAbmt << "NACK_CLIENT_MOVETO:" << clientsMap[tempCl.socketKey].id << "," << xPos << "," << yPos;
					std::cout << "\n      Client [" << value.id << "] is invalid..." << std::endl;
				}*/
			}
		}

		// std::string s(moveTo);
		// std::string delimiter = ",";
		// size_t pos = 0;
		// std::string token;
		// pos = s.find(delimiter);
		// token = s.substr(0, pos);
		// const std::string xPos = token;
		// s.erase(0, pos + delimiter.length());
		// const std::string yPos = s;

		// if((std::stod(xPos) >= 0 && std::stod(yPos) >= 0) && (std::stod(xPos) < MAX_WIDTH && std::stod(yPos) < MAX_HEIGHT)) {
		// 	clientsMap[tempCl.socketKey].x = std::stod(xPos);
		// 	clientsMap[tempCl.socketKey].y = std::stod(yPos);
		// 	ssAbmt << "ACK_CLIENT_MOVETO:" << clientsMap[tempCl.socketKey].id << "," << xPos << "," << yPos;
		// } else {
		// 	ssAbmt << "NACK_CLIENT_MOVETO:" << clientsMap[tempCl.socketKey].id << "," << xPos << "," << yPos;
		// }
	}

	// return ssAbmt ? strdup(((std::string) ssAbmt.str()).c_str()) : nullptr;
	return ssAbmt.str();
	// return ssAbmt ? strdup(strcat("ACK_GET_CLIENTS:", ((std::string) ssAbmt.str()).c_str())) : nullptr;
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

	const char *clientMoveTo = strstr(client_msg, "ClientMoveTo:");
	const char *bulletMoveTo = strstr(client_msg, "BulletMoveTo:");
	const char *getClients = strstr(client_msg, "GetClients:");

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

	/*if (getClients)
	{
		const std::string msg = handleGetClientsCommand(ClientSocket, getClients);
		if (msg.size() > 0)
		{
			// text = (char*) (((std::string) "ACK_GET_CLIENTS") + ((std::string) msg));
			text = (std::string) "ACK_GET_CLIENTS:" + msg;
		}
		else
		{
			text = (std::string) "NACK_GET_CLIENTS";
		}
	}*/

	char *pData;
	h = (_websocket_header *)sendbuf;
	*h = _websocket_header{};
	h->opcode = 0x1; // 0x1 = text, 0x2 = blob
	h->fin = 1;

	// bullet logic
	// if(text == nil || strlen(text.c_str()) <= 0) {
	if (text.size() <= 0) {
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
		}

		// std::string bld = ss.str();
		// text = (char*) bld.c_str();
		text = ss.str();
	}

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

	memcpy(pData, text.c_str(), (size_t)msg_length);
	LogHandling(0, "        msg_length: [" + std::to_string(msg_length) + "] chars", true);

	sendbuf_size += (size_t)msg_length;
	LogHandling(0, "        sendbuf_size: [" + std::to_string(sendbuf_size) + "] chars", true);

	// sendClientResponse(ClientSocket, sendbuf_size, sendbuf, iSendResult);

	// update all other clients
	LogHandling(0, "        updating [" + std::to_string(clientsMap.size()) + "] clients...", true);

	for (auto &[key, value] : clientsMap)
	{
		if (value.socket != ClientSocket)
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
		}
	}
}

void processConnection(SOCKET ClientSocket)
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
				const char *pTemplateResponse = getClientKey(ClientSocket, (char *)pKey, sendbuf);

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
}

int main(int argc, char *argv[]) {
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;

	HANDLE hThread;
	DWORD dwThreadId;
	int clntAddrLen;

	if (argc != 2) {
		ErrorHandling((char *)"[DEBUG] 인자값 이상. port값 넣으세요");
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ErrorHandling((char *)"[DEBUG] WSAStartup() Error");
	}

	hServSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (hServSock == INVALID_SOCKET) {
		ErrorHandling((char *)"[DEBUG] WSASocket() Error");
	}

	ZeroMemory(&servAddr, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR *)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
		ErrorHandling((char *)"[DEBUG] bind() Error");
	}

	if (listen(hServSock, 5) == SOCKET_ERROR) {
		ErrorHandling((char *)"[DEBUG] listen() Error");
	}

	LogHandling(0, "\n\n********** Server listening for connections on: [ws://" + ((std::string) inet_ntoa(servAddr.sin_addr)) + ":" + std::to_string(ntohs(servAddr.sin_port)) + "] **********\n\n\n", true);

	while (1) {
		clntAddrLen = sizeof(clntAddr);
		hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &clntAddrLen);
		if (hClntSock == INVALID_SOCKET) {
			ErrorHandling((char *)"[DEBUG] Accept() Error");
		}

		LogHandling(0, ">>> Connection Request From: [ws://" + ((std::string) inet_ntoa(clntAddr.sin_addr)) + ":" + std::to_string(ntohs(clntAddr.sin_port)) + "] <<<\n\n", true);

		hThread = (HANDLE)_beginthreadex(NULL, 0, RequestHandler, (LPVOID)hClntSock, 0, (unsigned *)&dwThreadId);
		if (hThread == INVALID_HANDLE_VALUE) {
			ErrorHandling((char *)"[DEBUG] _beginthreadex() error");
		}
	}
	// closesocket(hServSock);
	// WSACleanup();
	// return 0;
}

unsigned WINAPI RequestHandler(void *arg)
{
	SOCKET hClntSock = (SOCKET)arg;
	processConnection(hClntSock);
	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void LogHandling(int type, const std::string message, bool show)
{
	if (show)
	{
		switch (type)
		{
		case 0:
			/*fputs((const char*) "[info] ", stdout);
			fputs(message.c_str(), stdout);
			fputc('\n', stdout);*/
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
