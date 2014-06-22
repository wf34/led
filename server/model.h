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
#include <pthread.h>

#include "session.h"

enum LedColor {
    UNDEF = 0,
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

class MutexWrapper {
    public:
        MutexWrapper(pthread_mutex_t * m):
            m_(m) {
                pthread_mutex_lock(m_);
        }
        ~MutexWrapper() {
            pthread_mutex_unlock(m_);
        }
    private:
        pthread_mutex_t * m_;
};

class Model {
    public:
        Model();
        ~Model();
        void run();

        bool getState(bool& state) const;
        bool getFreq(int& freq) const;
        bool getColor(std::string& color) const;
        LedColor getColor() const;

        bool setState(bool state);
        bool setFreq(int freq);
        bool setColor(const std::string& color);
        void setColor(LedColor color);

    private:
        bool createNewSession(const std::string& id);
        static void pipesCheck(int fd, short event, void *arg);
        
        std::map<std::string,Session> sessions_;
        struct event_base* base_;
        struct event * pipesCheckEvent_;
        CameraState cam_;
        pthread_mutex_t * cameraMutex_;
};

#endif /* MODEL_H__ */

