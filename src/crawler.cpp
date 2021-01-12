#include<CkSpider.h>
#include<string>
#include<cstring>
#include<iostream>
#include<fstream>
#include<chrono>
#include<vector>
#include<queue>
#include<map>
#include<thread>
#include<ctime>
#include<unistd.h>

#include "../include/crawler.hpp"
#include "../include/record.hpp"

namespace web_crawler {
    Crawler::Crawler(const char* seed_file_path, int pages_to_collect){
        this->threads_log.open(THREADS_LOG_PATH);
        std::ifstream input_file;
        input_file.open(seed_file_path);
        if(input_file.is_open()){
            std::string url;
            while(!input_file.eof()){
                input_file  >> url;
                this->seed_urls.push_back(url);
            }
            input_file.close();
            this->pages_to_collect = pages_to_collect;
        }
    }

    Crawler::~Crawler(){
        for(std::map<std::string, Record*>::iterator it = this->registry.begin(); it != this->registry.end(); ++it){
            delete it->second;
        }
        this->threads_log.close();
    }

    bool Crawler::finished(){
        return this->visited_pages >= this->pages_to_collect;
    }

    bool Crawler::waiting_tasks(){
        return this->scheduler.empty() && this->tasks.size() > 0;
    }

    bool Crawler::processing_tasks(){
        return !this->scheduler.empty() || this->tasks.size() > 0;
    }

    bool Crawler::max_threads_reached(){
        return this->tasks.size() >= MAX_THREADS;
    }

    std::string Crawler::get_next_url(){
        CkSpider spider;
        std::string url;
        while(this->processing_tasks() || this->waiting_tasks()){
            url = this->scheduler.pop();
            std::string domain;
            try{
                domain = std::string(spider.getUrlDomain(url.c_str()));
            }
            catch(...){
                continue;
            }
            if(this->registry.find(domain) == this->registry.end()){
                this->mutex.lock();
                this->registry[domain] = new Record(domain);
                this->registry[domain]->start_visiting(url);
                this->mutex.unlock();
                return url;
            }
            else if(this->registry[domain]->can_start_visiting(url)){
                this->mutex.lock();
                this->registry[domain]->start_visiting(url);
                this->mutex.unlock();
                return url;
            }
            else{
                this->scheduler.push(url);
                usleep(1000000);
            }
        }
        return std::string("");
    }

    void Crawler::start_task(){
        std::thread* thread = new std::thread(this->crawl_url, this);
        this->threads_log << "Created task " << thread->get_id() << '\n';
        this->tasks.push_back(thread);
    }

    void Crawler::join_tasks(){
        std::vector<std::thread*>::iterator it = this->tasks.begin();
        while(it != this->tasks.end()){
            std::thread* thread = *it;
            ++it;
            this->tasks.erase(it-1);
            this->threads_log << "Disposing thread: " << thread->get_id() << '\n';
            thread->join();
            delete thread;
        }
    }

    void Crawler::queue_if_unvisited(std::string url){
        CkSpider spider;
        std::string domain = std::string(spider.getUrlDomain(url.c_str()));
        if(this->registry.find(domain) == this->registry.end() || !this->registry[domain]->has_visited(url)){
            this->mutex.lock();
            this->scheduler.push(url);
            this->mutex.unlock();
        }
    }

    void Crawler::crawl_url(Crawler* crawler){
        CkSpider spider;
        spider.AddAvoidOutboundLinkPattern("*twitter.com*");
        spider.put_ConnectTimeout(5);
        std::ofstream error_log(ERROR_LOG_PATH);
        while(!crawler->finished() && crawler->processing_tasks()){
            while(crawler->waiting_tasks()){
                usleep(100);
            }
            if(crawler->finished() || !crawler->processing_tasks()){
                break;
            }
            std::string url = crawler->get_next_url();
            const char* domain = spider.getUrlDomain(url.c_str());
            std::string registry_entry = std::string(domain);
            if(url.length() == 0 || domain == nullptr || strlen(domain) == 0){
                continue;
            }
            spider.Initialize(domain);
            spider.AddUnspidered(url.c_str());
            bool success = spider.CrawlNext();
            if(success){
                int links_number = spider.get_NumUnspidered();
                spider.SleepMs(100);
                for(int i = 0; i < links_number; i++){
                    auto start_time = std::chrono::high_resolution_clock::now();
                    success = spider.CrawlNext();
                    auto end_time = std::chrono::high_resolution_clock::now();
                    int time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
                    if(crawler->finished()){
                        break;
                    }
                    else if(success){
                            crawler->mutex.lock();
                            crawler->registry[registry_entry]->visit_page(url, time);
                            crawler->visited_pages++;
                            std::cout << spider.lastHtmlTitle()  << '\n' << spider.lastUrl() << "\n\n";
                            crawler->mutex.unlock();

                            for(int i = 0; i < spider.get_NumOutboundLinks(); i++){
                                try{
                                    std::string url = std::string(spider.getOutboundLink(i));
                                    crawler->queue_if_unvisited(url);
                                }
                                catch(...){
                                    error_log << "Can't resolve domain for: " << spider.getOutboundLink(i) << "\n" << std::endl;
                                }
                            }
                            spider.ClearOutboundLinks();
                    }
                    else{
                        error_log << spider.lastErrorText() << '\n';
                    }
                    spider.SleepMs(100);
                }
                crawler->registry[registry_entry]->finish();

                for(int i = 0; i < spider.get_NumUnspidered(); i++){
                    spider.SkipUnspidered(i);
                }
            }
        }
        error_log.close();
    }

    void Crawler::crawl(){
        for(std::vector<std::string>::iterator it = this->seed_urls.begin(); it != this->seed_urls.end(); ++it){
            this->queue_if_unvisited(*it);
        }
        auto start_time = std::chrono::high_resolution_clock::now();
        while(!this->finished() && this->processing_tasks()){
            this->start_task();
            
            while(this->max_threads_reached()  && !this->finished()){
                usleep(100000);
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        this->milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        if(!this->finished()){
            std::cout << "BAD SEED \n\n";
        }
    }

    void Crawler::report(){
        int total_time = 0;
        int total_pages = 0;
        std::map<std::string, int> visited_links;
        for(std::map<std::string, Record*>::iterator it = this->registry.begin(); it != this->registry.end(); ++it){
            Record* record = it->second;
            if(record->get_visited_pages() > 0){
                visited_links[it->first] = record->get_visited_pages();
                total_pages += record->get_visited_pages();
                total_time += record->get_total_time();
            }
        }

        std::cout << "\n" << visited_links.size() <<  " links visited:\n\n";
        for(std::map<std::string, int>::iterator it = visited_links.begin(); it != visited_links.end(); ++it){
            std::cout << "Collected " << it->second << " pages from " << it->first << "\n";
        }

        float average_time_in_seconds = (((float)total_time)/((float)total_pages))/1000.0;

        std::cout << "\nCrawled " << total_pages << " in " << ((float)this->milliseconds)/1000.0 << " seconds (" << MAX_THREADS << " simultaneous processes)\n";
        std::cout << average_time_in_seconds << " in average per page\n\n";
    }
}
