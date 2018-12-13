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
    for(auto c : address) {
        if(!isdigit(c)) {
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
