#include"../include/crawler_task.hpp"

namespace web_crawler {

    CrawlerTask::CrawlerTask(){
        this->thread = nullptr;
        this->finished = false;
    }

    CrawlerTask::~CrawlerTask(){
        // this->thread->join();
        // delete this->thread;
    }

    void CrawlerTask::set_thread(std::thread *thread){
        this->thread = thread;
    }

    std::thread* CrawlerTask::get_thread(){
        return this->thread;
    }

    void CrawlerTask::finish(){
        this->finished = true;
    }

    bool CrawlerTask::is_finished(){
        return this->finished;
    }

}