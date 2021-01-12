#include<iostream>

#include "./include/crawler.hpp"

#define PAGES_TO_COLLECT 100000

int main(int argc, char* const argv[]){
    if(argc < 2){
        std::cout << "Not enough arguments" << std::endl;
        return 1;
    }
    web_crawler::Crawler* crawler = new web_crawler::Crawler(argv[1], PAGES_TO_COLLECT);
    crawler->crawl();
    crawler->report();
    crawler->join_tasks();
    delete crawler;
    return 0;
}