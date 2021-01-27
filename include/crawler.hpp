#ifndef CRAWLER_H
#define CRAWLER_H

#include<string>
#include<vector>
#include<queue>
#include<map>
#include<thread>
#include<fstream>
#include<mutex>
#include<gumbo.h>

#include"./scheduler.hpp"
#include"./record.hpp"
#include"./index-cell.hpp"

#define MAX_THREADS 50
#define HTML_PATH "./output/html/"
#define INDEX_PATH "./output/index.idx"
#define THREADS_LOG_PATH "./output/threads.log"
#define ERROR_LOG_PATH "./output/error.log"

namespace web_crawler {

    class Crawler {
        private:
            std::mutex mutex;
            int pages_to_collect;
            int visited_pages = 0;
            int milliseconds = 0;
            std::vector<std::string> seed_urls;
            std::vector<std::thread*> tasks;
            std::map<std::string, Record*> registry;
            Scheduler scheduler;
            std::ofstream threads_log;
            std::map<std::string, IndexCell*> dictionary;

            std::string get_next_url();
            void queue_if_unvisited(std::string url);
            void start_task();
            static void crawl_url(Crawler* crawler);
            static std::string cleantext(GumboNode* node);
            static std::string html_text(std::string file_path);

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
            void build_index();
            void save_index();
            void load_index();
    };

}

#endif