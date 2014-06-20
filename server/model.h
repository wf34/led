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

enum LedColor {
    UNDEF = 1,
    RED,
    GREEN,
    BLUE
};

struct CameraState {
    CameraState():
        state_(false),
        frequency_(0),
        color_(RED) {
    }

    bool state_;
    int frequency_;
    LedColor color_;
};

class Model {
    public:
        void run();

        bool getState(bool& state) const;
        bool getFreq(int& freq) const;
        bool getColor(std::string& color) const;

        bool setState(bool state);
        bool setFreq(int freq);
        bool setColor(const std::string& color);

    private:
        static void createNewSession(const std::string& id, Model* ctx);
        static void pipesCheck(int fd, short event, void *arg);
        static void sigIntCb(evutil_socket_t fd, short event, void *arg);

        std::map<std::string,Session> sessions_;
        struct event_base* base_;
	    struct event * sigInt_;
        struct event * pipesCheckEvent;
        CameraState cam_;
};

#endif /* MODEL_H__ */

