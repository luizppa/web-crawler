#include <CkSpider.h>
#include<string>
#include<cstring>
#include<iostream>
#include<fstream>
#include<chrono>
#include<vector>
#include<queue>
#include<map>
#include<thread>
#include<unistd.h>

#include "../include/crawler.hpp"
#include "../include/crawler_task.hpp"

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
        this->threads_log.close();
    }

    bool Crawler::finished(){
        return this->visited_pages >= this->pages_to_collect;
    }

    bool Crawler::waiting_tasks(){
        return this->scheduler.size() == 0 && this->tasks.size() > 0;
    }

    bool Crawler::processing_tasks(){
        return  this->tasks.size() > 0 || this->scheduler.size() > 0;
    }

    bool Crawler::max_threads_reached(){
        return this->tasks.size() >= MAX_THREADS;
    }

    void Crawler::start_task(std::string url){
        CrawlerTask* task = new CrawlerTask();
        std::thread* thread = new std::thread(this->crawl_url, url, this, task);
        this->threads_log << "Created task " << thread->get_id() << " for " << url << '\n';
        task->set_thread(thread);
        this->tasks.push_back(task);
    }

    void Crawler::dispose_idle_tasks(){
        std::vector<CrawlerTask*>::iterator it = this->tasks.begin();
        while(it != this->tasks.end()){
            CrawlerTask* task = *it;
            if(task->is_finished()){
                it = it + 1;
                this->threads_log << "Disposing thread: " << task->get_thread()->get_id() << '\n';
                this->tasks.erase(it-1);
                delete task;
            }
            else{
                it = it + 1;
            }
        }
    }

    void Crawler::queue_if_unvisited(std::string url){
        if(this->registry.find(url) == this->registry.end()){
            this->scheduler.push(url);
            this->registry[url] = 0;
        }
    }

    void Crawler::crawl_url(std::string url, Crawler* crawler, CrawlerTask* task){
        CkSpider spider;
        spider.AddAvoidOutboundLinkPattern("*twitter.com*");
        spider.put_ConnectTimeout(5);
        const char* domain = spider.getUrlDomain(url.c_str());
        if(url.length() == 0 || domain == NULL || strlen(domain) == 0){
            task->finish();
            return;
        }
        std::ofstream error_log(ERROR_LOG_PATH);
        spider.Initialize(domain);
        spider.AddUnspidered(url.c_str());
        bool success = spider.CrawlNext();
        if(success){
            int links_number = spider.get_NumUnspidered();
            spider.SleepMs(100);
            for(int i = 0; i < links_number; i++){
                success = spider.CrawlNext();
                if(crawler->finished()){
                    break;
                }
                else if(success){
                    crawler->registry[url]++;
                    crawler->visited_pages++;
                    std::cout << spider.lastHtmlTitle()  << '\n' << spider.lastUrl();
                    std::cout << "\nVisited pages: " << crawler->visited_pages;
                    std::cout << "\nActive tasks: " << crawler->tasks.size() << "\n\n";

                    for(int i = 0; i < spider.get_NumOutboundLinks(); i++){
                        std::string url = std::string(spider.getOutboundLink(i));
                        crawler->queue_if_unvisited(url);
                    }
                    spider.ClearOutboundLinks();
                }
                else{
                    error_log << spider.lastErrorText() << '\n';
                }
                spider.SleepMs(100);
            }

            // for(int i = 0; i < spider.get_NumUnspidered(); i++){
            //     spider.SkipUnspidered(i);
            // }
        }
        error_log.close();
        task->finish();
    }

    void Crawler::crawl(){
        for(std::vector<std::string>::iterator it = this->seed_urls.begin(); it != this->seed_urls.end(); ++it){
            this->scheduler.push(*it);
            this->registry[*it] = 0;
        }

        while(!this->finished() && this->processing_tasks()){
            std::string url = this->scheduler.front();
            this->scheduler.pop();
            this->start_task(url);
            
            while((this->max_threads_reached() || this->waiting_tasks())  && !this->finished()){
                usleep(100000);
                this->dispose_idle_tasks();
            }
        }

        if(!this->finished()){
            std::cout << "BAD SEED \n\n";
        }

        this->threads_log << "Joining " << this->tasks.size() << " threads active\n";
        usleep(500000);
        this->dispose_idle_tasks();
    }

    void Crawler::report(){
        std::map<std::string, int> visited_links;
        for(std::map<std::string, int>::iterator it = this->registry.begin(); it != this->registry.end(); ++it){
            if(it->second > 0){
                visited_links[it->first] = it->second;
            }
        }

        std::cout << "\n" << visited_links.size() <<  " links visited:\n\n";
        for(std::map<std::string, int>::iterator it = visited_links.begin(); it != visited_links.end(); ++it){
            std::cout << "Collected " << it->second << " pages from " << it->first << '\n';
        }
    }
}
