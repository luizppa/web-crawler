#include<queue>
#include<mutex>

#include"../include/scheduler.hpp"

namespace web_crawler {
    Scheduler::Scheduler(){}

    int Scheduler::size() {
        return this->schedule.size();
    }

    bool Scheduler::empty() {
        return this->schedule.empty();
    }

    void Scheduler::push(std::string url) {
        this->m.lock();
        this->schedule.push(url);
        this->m.unlock();
    }
 
    std::string Scheduler::pop() {
        std::string url;
        this->m.lock();
        if(!this->schedule.empty()) {
            url = this->schedule.front();
            this->schedule.pop();
        }
        this->m.unlock();
        return url;
    }
}