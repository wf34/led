#ifndef SESSION_H__
#define SESSION_H__

#include <string>
#include <sstream>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <event2/event-config.h>
#include <event2/event.h>

#define FIFO_DIR "/tmp/led"

struct Session {

    Session(const std::string& id);
    Session();
    Session(const Session& other);

    bool open(struct event_base* base, event_callback_fn cb);
    std::string getFifo(bool req = true);
    std::string id_;
    int reqFd_;
    int respFd_;
    struct event* ev_;
};

#endif /* SESSION_H__ */

