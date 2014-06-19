#ifndef MODEL_H__
#define MODEL_H__

#include <string>
#include <map>
#include <cstdlib>
#include <cstring>
#include <sys/time.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <dirent.h>

#include <event2/event-config.h>
#include <event2/event.h>

#include "session.h"


class Model {
    public:
        void run();
        static void readFromPipe(evutil_socket_t fd, short event, void* ctx);

    private:
        static void createNewSession(const std::string& id, Model* ctx);
        static void pipesCheck(int fd, short event, void *arg);
        static void sigIntCb(evutil_socket_t fd, short event, void *arg);

        std::map<std::string,Session> sessions_;
        struct event_base* base_;
	    struct event * sigInt_;
        struct event * pipesCheckEvent;

};

#endif /* MODEL_H__ */

