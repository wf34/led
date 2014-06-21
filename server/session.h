#ifndef SESSION_H__
#define SESSION_H__

#include <string>
#include <sstream>
#include <vector>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <event2/event-config.h>
#include <event2/event.h>

#define FIFO_DIR "/tmp/led"
#define MESSAGE_BUFFER_LEN 256

class Model;

class Session {
    public:
        Session(const std::string& id, Model* m);
        Session();
        Session(const Session& other);
        ~Session();

        bool open(struct event_base* base);
        void close();

    private:
        void process(const std::string& request,
                     std::string& response);

        static void readFromPipe(evutil_socket_t fd,
                                 short event, void* ctx);
        std::string getFifo(bool req = true);

        Model* model_;
        std::string id_;
        int reqFd_;
        struct event* ev_;

        static const std::string cmdGetState;
        static const std::string cmdGetColor;
        static const std::string cmdGetFreq; 
        static const std::string cmdSetState;
        static const std::string cmdSetColor;
        static const std::string cmdSetFreq; 
};

#endif /* SESSION_H__ */

