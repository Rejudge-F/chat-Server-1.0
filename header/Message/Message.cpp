
#include "Message.h"

Message::Message() {
    address = "";
    message = "";
}

Message::~Message() {
    address = "";
    message = "";
}

bool Message::setMessage(std::string _message) {
    int len = _message.length();
    for(int i = len - 2; i >= 0; i--) {
        if(_message[i] == '-' && _message[i + 1] == '>') {
            message = _message.substr(0, i);
            address = _message.substr(i + 2);
            break;
        }
    }
    len = address.length();
    for(int i = 0; i < len - 1; i++) { //len-1 for '\n'
        if(!isdigit(address[i])) {
            printf("*%c", address[i]);
            return false;
        }
    }
    return true;
}

std::string Message::getMessage() {
    return message;
}

int Message::getAddress() {
    return atoi(address.c_str());
}

void Message::clear() {
    address = "";
    message = "";
}
