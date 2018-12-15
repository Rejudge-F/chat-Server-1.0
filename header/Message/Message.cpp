#include <stdio.h>
#include <stdlib.h>
#include <string>
class Message {
public:
    Message ();
    ~Message();
    bool setMessage(std::string _message);
    char * getMessage();
    int getAddress();
    void clear();
private:
    char address[1024];
    char message[1024];
};
