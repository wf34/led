
#include "model.h"

Model::Model() {
    cameraMutex_ = new pthread_mutex_t();
    pthread_mutex_init(cameraMutex_, NULL);
    cam_.color_ = RED;
    cam_.state_ = true;
    cam_.frequency_ = 5;
}


Model::~Model() {
    setColor(UNDEF);
    pthread_mutex_destroy(cameraMutex_);

    for(std::map<std::string,Session>::iterator it = sessions_.begin();
        it != sessions_.end(); ++it)
        it->second.close();
    
    event_base_loopbreak(base_);
    if(pipesCheckEvent_) {
        event_del(pipesCheckEvent_);
        event_free(pipesCheckEvent_);
    }
    if(base_)
        event_base_free(base_);
}


bool
Model::getState(bool& state) const {
    MutexWrapper w(cameraMutex_);
    state = cam_.state_;
    return true;
}


bool
Model::getFreq(int& freq) const {
    MutexWrapper w(cameraMutex_);
    freq = cam_.frequency_;
    return true;
}


bool
Model::getColor(std::string& color) const {
    MutexWrapper w(cameraMutex_);
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

LedColor Model::getColor() const {
    MutexWrapper w(cameraMutex_);
   return cam_.color_; 
};


bool
Model::setState(bool state) {
    MutexWrapper w(cameraMutex_);
    cam_.state_ = state;
    return true;
}


bool
Model::setFreq(int freq) {
    MutexWrapper w(cameraMutex_);
    cam_.frequency_ = freq;
    return true;
}


bool
Model::setColor(const std::string& color) {
    MutexWrapper w(cameraMutex_);
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
Model::setColor(LedColor color) {
    MutexWrapper w(cameraMutex_);
    cam_.color_ = color;
}

void
Model::createNewSession(const std::string& id, Model* ctx) {
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
Model::run() {
    
    int status = mkdir(FIFO_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status != 0  && errno != EEXIST) {
        printf("return mkdir %d\n", status);
        exit(1);
    }
	
	base_ = event_base_new();
	
    // timer for clients detection
    struct timeval time;
    time.tv_sec = 1;
    time.tv_usec = 0;
    pipesCheckEvent_ = event_new(base_, 0, EV_PERSIST,
                                Model::pipesCheck, this);
    evtimer_add(pipesCheckEvent_, &time);

	event_base_dispatch(base_);
}

