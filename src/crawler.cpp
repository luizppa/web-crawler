#include <CkSpider.h>
#include<string>
#include<iostream>
#include<fstream>
#include<chrono>
#include<vector>
#include<queue>
#include<map>
#include<thread>

#include "../include/crawler.hpp"

namespace web_crawler {
    Crawler::Crawler(const char* seed_file_path, int pages_to_collect){
        std::ifstream input_file;
        input_file.open(seed_file_path);
        if(input_file.is_open()){
            std::string url;
            while(!input_file.eof()){
                input_file  >> url;
                this->seed_urls.push_back(url);
            }
            this->pages_to_collect = pages_to_collect;
        }
    }

    void Crawler::crawl_url(std::string url, Crawler* crawler){
        CkSpider spider;
        spider.AddAvoidOutboundLinkPattern("*twitter.com*");
        spider.put_ConnectTimeout(5);
        if(spider.getUrlDomain(url.c_str()) == NULL){
            return;
        }
        spider.Initialize(spider.getUrlDomain(url.c_str()));
        spider.AddUnspidered(url.c_str());
        bool success = spider.CrawlNext();
        if(success){
            int links_number = spider.get_NumUnspidered();
            crawler->registry[url] = links_number;
            spider.SleepMs(100);
            for(int i = 0; i < links_number && crawler->visited_pages < crawler->pages_to_collect; i++){
                success = spider.CrawlNext();
                if(success && crawler->visited_pages < crawler->pages_to_collect){
                    crawler->visited_pages++;
                    std::cout << spider.lastHtmlTitle()  << std::endl << spider.lastUrl() << std::endl << "Visited pages: " << crawler->visited_pages << std::endl << std::endl;
                    for(int i = 0; i < spider.get_NumOutboundLinks(); i++){
                        std::string url = std::string(spider.getOutboundLink(i));
                        if(crawler->registry.find(url) == crawler->registry.end()){
                            crawler->scheduler.push(url);
                            crawler->registry[url] = 0;
                        }
                    }
                    spider.ClearOutboundLinks();
                    spider.SleepMs(100);
                }
                else if(crawler->visited_pages >= crawler->pages_to_collect){
                    break;
                }
                else{
                    std::cout << spider.lastErrorText() << std::endl;
                }
            }

            for(int i = 0; i < spider.get_NumUnspidered(); i++){
                spider.SkipUnspidered(i);
            }
        }
    }

    void Crawler::crawl(){
        for(std::vector<std::string>::iterator it = this->seed_urls.begin(); it != this->seed_urls.end(); ++it){
            this->scheduler.push(*it);
            this->registry[*it] = 0;
        }

        while(this->visited_pages < this->pages_to_collect && (this->scheduler.size() > 0 || this->threads.size() > 0)){
            std::string current_url = this->scheduler.front();
            this->scheduler.pop();

            this->threads.push_back(new std::thread(this->crawl_url, current_url, this));
            while(this->scheduler.size() == 0){
                std::thread* thread = this->threads.at(0);
                this->threads.erase(this->threads.begin());
                std::cout  << "Joining thread " << thread->get_id() << "... \n";
                thread->join();
                delete thread;
            }
        }

        if(visited_pages < this->pages_to_collect){
            std::cout << "BAD SEED \n\n";
        }

        std::cout << this->threads.size() << " threads active:\n";
        for(std::vector<std::thread*>::iterator it = this->threads.begin(); it != this->threads.end(); ++it){
            std::thread* thread = *it;
            std::cout  << "Joining thread " << thread->get_id() << "... \n";
            thread->join();
            delete thread;
        }
    }
}
