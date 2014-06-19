
#include "session.h"


Session::Session():
    reqFd_(-1),
    respFd_(-1),
    ev_(NULL) {
}


Session::Session(const std::string& id):
    id_(id),
    reqFd_(-1),
    respFd_(-1),
    ev_(NULL) {
}


Session::Session(const Session& other):
    id_(other.id_),
    reqFd_(other.reqFd_),
    respFd_(other.respFd_),
    ev_(other.ev_) {
}


std::string
Session::getFifo(bool req) {
    std::stringstream ss;
    ss << FIFO_DIR << "/led_pipe_" << id_
       << (req ? "_req" : "_resp");
    return ss.str();
}


bool
Session::open(struct event_base* base, event_callback_fn cb ) {
    reqFd_ = ::open(getFifo().c_str(), O_RDWR | O_NONBLOCK);
    //respFd_ = ::open(getFifo(false).c_str(), O_WRONLY | O_NONBLOCK);

	if (reqFd_ == -1) {
        printf("fd invalid %d %d errno %d\n",
               reqFd_, respFd_, errno);
        return false;
    }

    ev_ = event_new(base, reqFd_,
                    EV_READ|EV_PERSIST,
                    *cb, this); 
    event_add(ev_, NULL);
    return true;
}

