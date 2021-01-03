#ifndef CRAWLER_H
#define CRAWLER_H

#include<string>
#include<vector>
#include<queue>
#include<map>
#include<thread>

namespace web_crawler {

    class Crawler {
        private:
            int pages_to_collect;
            int visited_pages = 0;
            std::vector<std::string> seed_urls;
            std::vector<std::thread*> threads;
            std::map<std::string, int> registry;
            std::queue<std::string> scheduler;

            static void crawl_url(std::string url, Crawler* crawler);

        public:
            Crawler(const char* seed_file_path, int pages_to_collect);
            void crawl();
    };

}

#endif