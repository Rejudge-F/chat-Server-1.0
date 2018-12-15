#include "Message.h"
#include <cstring>

Message::Message() {
    memset(address, 0, sizeof(address));
    memset(message, 0, sizeof(message));
}

Message::~Message() {
}

bool Message::setMessage(std::string _message) {
    int len = _message.length();
    for(int i = len - 2; i >= 0; i--) {
        if(_message[i] == '-' && _message[i + 1] == '>') {
            strcpy(message, _message.substr(0, i).c_str());
            strcpy(address, _message.substr(i + 2).c_str());
            break;
        }
    }
    len = strlen(address);
    address[len - 1] = '\0';
    if(len == 0) return false;
    for(int i = 0; i < len - 1; i++) { //len-1 for '\n'
        if(!isdigit(address[i])) {
            return false;
        }
    }
    return true;
}

char * Message::getMessage() {
    char * ptr = message;
    return ptr;
}

int Message::getAddress() {
    return atoi(address);
}

void Message::clear() {
    memset(address, 0, sizeof(address));
    memset(message, 0, sizeof(message));
}
