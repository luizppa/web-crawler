#ifndef CRAWLER_H
#define CRAWLER_H

#include<string>
#include<vector>
#include<queue>
#include<map>
#include<thread>
#include<fstream>
#include<mutex>

#include"./scheduler.hpp"
#include"./record.hpp"

#define MAX_THREADS 50
#define THREADS_LOG_PATH "./output/threads.log"
#define ERROR_LOG_PATH "./output/error.log"

namespace web_crawler {

    class Crawler {
        private:
            std::mutex mutex;
            int pages_to_collect;
            int visited_pages = 0;
            std::vector<std::string> seed_urls;
            std::vector<std::thread*> tasks;
            std::map<std::string, Record*> registry;
            Scheduler scheduler;
            std::ofstream threads_log;

            std::string get_next_url();
            void queue_if_unvisited(std::string url);
            void start_task();
            static void crawl_url(Crawler* crawler);

        public:
            Crawler(const char* seed_file_path, int pages_to_collect);
            ~Crawler();
            bool finished();
            bool waiting_tasks();
            bool processing_tasks();
            bool max_threads_reached();
            void join_tasks();
            void crawl();
            void report();
    };

}

#endif