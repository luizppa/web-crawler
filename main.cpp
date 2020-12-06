#include <CkSpider.h>
#include<iostream>
#include<chrono>

void crawl_through(char const starting_url[], int n){
    CkSpider spider;
    int pages_to_crawl = n+1;
    int total_time = 0;

    spider.Initialize(starting_url);
    spider.AddUnspidered(starting_url);

    for (int i = 0; i < pages_to_crawl; i++) {
        bool success;
        auto start_time = std::chrono::high_resolution_clock::now();
        success = spider.CrawlNext();
        auto end_time = std::chrono::high_resolution_clock::now();
        total_time += std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        if(success) {
            std::cout << spider.lastHtmlTitle()  << std::endl << spider.lastUrl() << std::endl << std::endl;
        }
        else {
            if (spider.get_NumUnspidered() == 0) {
                std::cout << "No more URLs to spider" << std::endl;
                break;
            }
            else {
                std::cout << spider.lastErrorText() << std::endl;
            }
        }
    }

    float total_time_in_seconds = ((float)total_time)/1000.0;
    float average_time_in_seconds = total_time_in_seconds/((float)pages_to_crawl);
    std::cout << "Duration: " << total_time << " milliseconds" << std::endl;
    std::cout << "Average time spent per page: " << average_time_in_seconds << " milliseconds" << std::endl;
}

int main(int argc, char* const argv[]){
    if(argc < 3){
        std::cout << "Not enough arguments" << std::endl;
        return 1;
    }
    crawl_through(argv[1], atoi(argv[2]));
    return 0;
}