#pragma once

struct ConnectionStatus {
	uint16_t ping; // 1023 ms
	uint8_t consistency; // 0-255 (scale), to measure how stable the connection is
	uint16_t packetloss; // 65535
	bool asking; // for connection
	bool waiting;
	bool responding;
	bool lost;
	uint64_t lastticksent;
	uint64_t lasttickgot;
};

class Server
{
public:
	void CheckConnections();
	void CheckDisconnections();
};

