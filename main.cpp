#include <CkSpider.h>
#include<string>
#include<iostream>
#include<fstream>
#include<chrono>
#include<vector>
#include<queue>
#include<map>
#include<thread>

#define PAGES_TO_COLLECT 100000

std::vector<std::string> read_input(const char* input_file_path){
    std::vector<std::string> seed;
    std::ifstream input_file;
    input_file.open(input_file_path);
    if(input_file.is_open()){
        std::string url;
        while(!input_file.eof()){
            input_file  >> url;
            seed.push_back(url);
        }
    }
    return seed;
}

void crawl_through(std::vector<std::string> seed_urls){
    int visited_pages = 0;
    bool success;
    CkSpider spider;
    std::map<std::string, int> crawled_urls;
    std::queue<std::string> long_term_scheduler;

    for(std::vector<std::string>::iterator it = seed_urls.begin(); it != seed_urls.end(); ++it){
        long_term_scheduler.push(*it);
        crawled_urls[*it] = 0;
    }

    while(visited_pages < PAGES_TO_COLLECT && long_term_scheduler.size() > 0){
        std::string current_url = long_term_scheduler.front();
        long_term_scheduler.pop();

        if(spider.getUrlDomain(current_url.c_str()) == NULL){
            continue;
        }
        spider.Initialize(spider.getUrlDomain(current_url.c_str()));
        spider.AddUnspidered(current_url.c_str());
        success = spider.CrawlNext();
        if(success){
            visited_pages++;
            int links_number = spider.get_NumUnspidered();
            crawled_urls[current_url] = links_number;
            spider.SleepMs(100);
            for(int i = 0; i < links_number; i++){
                visited_pages++;
                success = spider.CrawlNext();
                if(success){
                    std::cout << spider.lastHtmlTitle()  << std::endl << spider.lastUrl() << std::endl << std::endl;
                    spider.SleepMs(100);
                }
                else{
                    std::cout << spider.lastErrorText() << std::endl;
                }
            }

            for(int i = 0; i < spider.get_NumUnspidered(); i++){
                spider.SkipUnspidered(i);
            }

            for(int i = 0; i < spider.get_NumOutboundLinks(); i++){
                std::string url = std::string(spider.getOutboundLink(i));
                if(crawled_urls.find(url) == crawled_urls.end()){
                    long_term_scheduler.push(url);
                    crawled_urls[url] = 0;
                }
            }
        }

    }
}

int main(int argc, char* const argv[]){
    if(argc < 2){
        std::cout << "Not enough arguments" << std::endl;
        return 1;
    }
    std::vector<std::string> seed_urls =  read_input(argv[1]);
    crawl_through(seed_urls);
    return 0;
}