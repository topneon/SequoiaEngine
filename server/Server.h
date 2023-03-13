#pragma once

struct ConnectionStatus {
	bool asking : 1; // for connection
	bool waiting : 1;
	bool responding : 1;
	bool lost : 1;
	uint16_t packetloss : 12; // 2047
	uint16_t ping : 11; // 1023 ms
	uint8_t consistency : 5; // 0-31 (scale), to measure how stable the connection is
	uint64_t lastticksent;
	uint64_t lasttickgot;
};

class Server
{
public:
	void CheckConnections();
	void CheckDisconnections();
};

