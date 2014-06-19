
#include "model.h"

void
Model::createNewSession(const std::string& id, Model* ctx) {
    printf("Adding session %s\n", id.c_str());
    ctx->sessions_[id] = Session(id);
    ctx->sessions_[id].open(ctx->base_, &Model::readFromPipe);

    //for (std::map<std::string, Session>::iterator it = ctx->sessions_.begin();
    //    it!= ctx->sessions_.end();
    //    it++) {
    //    printf("we have: %s\n", it->first.c_str());
    //}
}

void
Model::readFromPipe(evutil_socket_t fd, short event, void* ctx) {
    Session* curr = static_cast<Session*>(ctx);
    printf("Got read!\n");
	char buf[255];
	int len;
	//struct event *ev = evfifo;

	//fprintf(stderr, "readFromPipe called with fd: %d, event: %d, arg: %p\n", (int)fd, event, ev);
	len = read(fd, buf, sizeof(buf) - 1);

	if (len <= 0) {
		if (len == -1)
			perror("read");
		else if (len == 0)
			fprintf(stderr, "Connection closed %d\n", errno);
		//event_del(ev);
		//event_base_loopbreak(event_get_base(ev));
		return;
	}

	buf[len] = '\0';
	fprintf(stdout, "Read: %s\n", buf);
    
    char outBuff[64]; 
    memset(outBuff, 0, sizeof(char)*64);
    sprintf(outBuff, "%s", "responseresponse");
    int respFd = ::open(curr->getFifo(false).c_str(), O_WRONLY);
    ::write(respFd, outBuff, strlen(outBuff));
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

