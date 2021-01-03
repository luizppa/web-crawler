#include<iostream>

#include "./include/crawler.hpp"

#define PAGES_TO_COLLECT 100

int main(int argc, char* const argv[]){
    if(argc < 2){
        std::cout << "Not enough arguments" << std::endl;
        return 1;
    }
    web_crawler::Crawler* crawler = new web_crawler::Crawler(argv[1], PAGES_TO_COLLECT);
    crawler->crawl();
    delete crawler;
    return 0;
}