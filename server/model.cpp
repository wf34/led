
#include "model.h"

bool
Model::getState(bool& state) const {
    state = cam_.state_;
    return true;
}


bool
Model::getFreq(int& freq) const {
    freq = cam_.frequency_;
    return true;
}


bool
Model::getColor(std::string& color) const {
    if (cam_.color_ == UNDEF)
        return false;
    if (cam_.color_ == RED)
        color = "red";
    else if (cam_.color_ == BLUE)
        color = "blue";
    else if (cam_.color_ == GREEN)
        color = "green";
    return true;
}


bool
Model::setState(bool state) {
    cam_.state_ = state;
    return true;
}


bool
Model::setFreq(int freq) {
    cam_.frequency_ = freq;
    return true;
}


bool
Model::setColor(const std::string& color) {
    if (color == "red")
        cam_.color_ = RED;
    else if (color == "green")
        cam_.color_ = GREEN;
    else if (color == "blue")
        cam_.color_ = BLUE;
    else
        return false;

    return true;
}

void
Model::createNewSession(const std::string& id, Model* ctx) {
    printf("Adding session %s\n", id.c_str());
    ctx->sessions_[id] = Session(id, ctx);
    ctx->sessions_[id].open(ctx->base_);
}


void
Model::pipesCheck(int fd, short event, void *arg) {
    Model* ctx = static_cast<Model*>(arg);
    DIR * d = opendir(FIFO_DIR);
    if (NULL == d) {
        printf("error\n");
        exit(1);
    }

    struct dirent* dir;
    while ((dir = readdir(d)) != NULL) {
         std::string currFile(dir->d_name);
         // get id value
         if (currFile.size() > 9 &&
             currFile.substr(0, 9) == "led_pipe_" &&
             currFile.substr(currFile.length()-4, currFile.length()) == "_req") {
             std::string id = currFile.substr(9,16);
             if (ctx->sessions_.find(id) != ctx->sessions_.end())
                 continue;
            createNewSession(id, ctx);
         }
     }
     closedir(d);
}


void
Model::sigIntCb(evutil_socket_t fd, short event, void *arg) {
    printf("caught SIGINT\n");
	struct event_base *base = static_cast<event_base*>(arg);
	event_base_loopbreak(base);
}


void
Model::run() {
    
    int status = mkdir(FIFO_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status != 0  && errno != EEXIST) {
        printf("return mkdir %d\n", status);
        exit(1);
    }
	
	base_ = event_base_new();

	// catch SIGINT so that event
	sigInt_ = evsignal_new(base_, SIGINT, Model::sigIntCb, base_);
	event_add(sigInt_, NULL);

    // timer for clients detection
    struct timeval time;
    time.tv_sec = 1;
    time.tv_usec = 0;
    pipesCheckEvent = event_new(base_, 0, EV_PERSIST,
                                Model::pipesCheck, this);
    evtimer_add(pipesCheckEvent, &time);

	event_base_dispatch(base_);
}

