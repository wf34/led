
#include "session.h"
#include "model.h"

std::vector<std::string>&
split(const std::string &s, char delim,
      std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string>
split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

const std::string Session::cmdGetState = "get-led-state";
const std::string Session::cmdGetColor = "get-led-color";
const std::string Session::cmdGetFreq = "get-led-rate";
const std::string Session::cmdSetState = "set-led-state";
const std::string Session::cmdSetColor = "set-led-color";
const std::string Session::cmdSetFreq = "set-led-rate";


Session::Session():
    reqFd_(-1),
    ev_(NULL) {
}

Session::~Session() {
    if (ev_)
         event_free(ev_);
}


Session::Session(const std::string& id, Model* model):
    id_(id),
    reqFd_(-1),
    ev_(NULL),
    model_(model) {
}


Session::Session(const Session& other):
    id_(other.id_),
    reqFd_(other.reqFd_),
    ev_(other.ev_),
    model_(other.model_) {
}


std::string
Session::getFifo(bool req) {
    std::stringstream ss;
    ss << FIFO_DIR << "/led_pipe_" << id_
       << (req ? "_req" : "_resp");
    return ss.str();
}


bool
Session::open(struct event_base* base) {
    reqFd_ = ::open(getFifo().c_str(), O_RDWR | O_NONBLOCK);

	if (reqFd_ == -1)
        return false;

    ev_ = event_new(base, reqFd_,
                    EV_READ|EV_PERSIST,
                    Session::readFromPipe, this); 
    event_add(ev_, NULL);
    return true;
}


void
Session::process(const std::string& request,
                 std::string& response) {
    std::vector<std::string> inp = split(request, ' ');
    for(int i = 0; i < inp.size(); ++i)
        inp[i].erase(inp[i].find_last_not_of(" \n\r\t")+1);

    std::stringstream respStream;
    if(inp[0] == cmdGetState) {
        bool state = false;
        if(model_->getState(state))
            respStream << "OK " << (state? "on" : "off");
    } else if (inp[0] == cmdGetFreq) {
        int freq = 0;
        if(model_->getFreq(freq))
            respStream << "OK " << freq;
    } else if (inp[0] == cmdGetColor) {
        std::string color;
        if(model_->getColor(color))
            respStream << "OK " << color;
    } else if (inp[0] == cmdSetState &&
               inp.size() == 2) {
        if ((inp[1] == "on" ||
             inp[1] == "off") &&
            model_->setState(inp[1] == "on"))
            respStream << "OK";
    } else if (inp[0] == cmdSetFreq &&
               inp.size() == 2) {
        int freq = atoi(inp[1].c_str());
        if(model_->setFreq(freq))
            respStream << "OK";
    } else if (inp[0] == cmdSetColor &&
               inp.size() == 2) {
        if(model_->setColor(inp[1]))
            respStream << "OK";
    }

    if(respStream.str().empty())
        respStream << "FAILED";

    response = respStream.str();
}


void
Session::readFromPipe(evutil_socket_t fd,
                      short event, void* ctx) {
    Session* curr = static_cast<Session*>(ctx);
	
    char request[MESSAGE_BUFFER_LEN];
	memset(request, 0, sizeof(char)*MESSAGE_BUFFER_LEN);
    
    int len = read(fd, request, sizeof(request) - 1);
	if (len <= 0) {
		if (len == -1)
			perror("read");
		return;
	}

    std::string response;
    curr->process(request, response); 

    int respFd = ::open(curr->getFifo(false).c_str(),
                        O_WRONLY);
    ::write(respFd, response.c_str(), response.size());
    ::close(respFd);
}

void
Session::close() {
    event_del(ev_);
}

