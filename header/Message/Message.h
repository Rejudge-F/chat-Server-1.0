#include <stdio.h>
#include <stdlib.h>
#include <string>
class Message {
public:
    Message ();
    ~Message();
    bool setMessage(std::string _message);
    std::string getMessage();
    int getAddress();
    void clear();
private:
    std::string address;
    std::string message;
};
