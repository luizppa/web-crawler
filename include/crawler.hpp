#ifndef CRAWLER_H
#define CRAWLER_H

#include<string>
#include<vector>
#include<queue>
#include<map>
#include<thread>
#include<fstream>

#include"./crawler_task.hpp"

#define MAX_THREADS 20
#define THREADS_LOG_PATH "./output/threads.log"
#define ERROR_LOG_PATH "./output/error.log"

namespace web_crawler {

    class Crawler {
        private:
            int pages_to_collect;
            int visited_pages = 0;
            std::vector<std::string> seed_urls;
            std::vector<CrawlerTask*> tasks;
            std::map<std::string, int> registry;
            std::queue<std::string> scheduler;
            std::ofstream threads_log;

            void queue_if_unvisited(std::string url);
            void dispose_idle_tasks();
            static void crawl_url(std::string url, Crawler* crawler, CrawlerTask* task);

        public:
            Crawler(const char* seed_file_path, int pages_to_collect);
            ~Crawler();
            bool finished();
            bool waiting_tasks();
            bool processing_tasks();
            bool max_threads_reached();
            void start_task(std::string url);
            void crawl();
            void report();
    };

}

#endif