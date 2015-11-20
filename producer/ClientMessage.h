enum ClientMessageType {
	REGISTER = 0,
	AVAILABLE,
	NULL_MSG
};

struct ClientMessage {
	ClientMessageType type;	
	vector<string> playList;
};

