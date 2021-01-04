#ifndef CRAWLER_H
#define CRAWLER_H

#include<string>
#include<vector>
#include<queue>
#include<map>
#include<thread>

#define MAX_THREADS 500

namespace web_crawler {

    class Crawler {
        private:
            int pages_to_collect;
            int visited_pages = 0;
            std::vector<std::string> seed_urls;
            std::vector<std::thread*> threads;
            std::map<std::string, int> registry;
            std::queue<std::string> scheduler;

            void queue_if_unvisited(std::string url);
            void dispose_idle_threads();
            static void crawl_url(std::string url, Crawler* crawler);

        public:
            Crawler(const char* seed_file_path, int pages_to_collect);
            bool finished();
            bool waiting_tasks();
            void crawl();
    };

}

#endif