#ifndef CRAWLER_TASK_H
#define CRAWLER_TASK_H

#include<thread>

namespace web_crawler {

    class CrawlerTask {
        private:
            std::thread *thread;
            bool finished = false;

        public:
            CrawlerTask();
            ~CrawlerTask();
            void set_thread(std::thread*);
            std::thread* get_thread();
            void finish();
            bool is_finished();
    };

}

#endif