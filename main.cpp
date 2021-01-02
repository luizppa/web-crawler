#include <CkSpider.h>
#include<iostream>
#include<fstream>
#include<chrono>
#include<vector>
#include<queue>
#include<thread>

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

void crawl_async(CkSpider* spider, int* total_time, int* visited_pages){
    bool success;
    auto start_time = std::chrono::high_resolution_clock::now();
    success = spider->CrawlNext();
    auto end_time = std::chrono::high_resolution_clock::now();
    if(success) {
        *visited_pages += 1;
        *total_time += std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        std::cout << spider->lastHtmlTitle()  << std::endl << spider->lastUrl() << std::endl << std::endl;
    }
    else {
        if(spider->get_NumUnspidered() == 0){
            std::cout << "FIM" << std::endl;
        }
        std::cout << spider->lastErrorText() << std::endl;
    }
}

void crawl_through_async(std::vector<std::string> seed_urls){
    int total_time = 0;
    int visited_pages = 0;
    std::queue<std::string> long_term_scheduler;

    for(std::vector<std::string>::iterator it = seed_urls.begin(); it != seed_urls.end(); ++it){
        long_term_scheduler.push(*it);
    }

    while(visited_pages < 100000 && long_term_scheduler.size() > 0){
        const char* current_url = long_term_scheduler.front().c_str();
        CkSpider spider;
        spider.Initialize(current_url);
        spider.AddUnspidered(current_url);
        long_term_scheduler.pop();

        while(spider.get_NumUnspidered() != 0 && visited_pages < 100000) {
            std::thread crawl(crawl_async, &spider, &total_time, &visited_pages);
            crawl.detach();
            spider.SleepMs(100);
        }
        std::cout << "Outbounds " << spider.get_NumOutboundLinks() << std::endl;

        for(int i = 0; i < spider.get_NumOutboundLinks(); i++){
            long_term_scheduler.push(std::string(spider.getOutboundLink(i)));
        }
    }

    float total_time_in_seconds = ((float)total_time)/1000.0;
    float average_time_in_seconds = total_time_in_seconds/((float)visited_pages);
    std::cout << "Duration: " << total_time_in_seconds << " seconds" << std::endl;
    std::cout << "Average time spent per page: " << average_time_in_seconds << " seconds" << std::endl;
}

void crawl_through(std::vector<std::string> seed_urls){
    int total_time = 0;
    int visited_pages = 0;
    std::queue<std::string> long_term_scheduler;

    for(std::vector<std::string>::iterator it = seed_urls.begin(); it != seed_urls.end(); ++it){
        long_term_scheduler.push(*it);
    }

    while(visited_pages < 100000 && long_term_scheduler.size() > 0){
        const char* current_url = long_term_scheduler.front().c_str();
        CkSpider spider;
        spider.Initialize(current_url);
        spider.AddUnspidered(current_url);
        long_term_scheduler.pop();

        while(visited_pages < 100000 && spider.get_NumUnspidered() != 0) {
            bool success;
            auto start_time = std::chrono::high_resolution_clock::now();
            success = spider.CrawlNext();
            auto end_time = std::chrono::high_resolution_clock::now();
            if(success) {
                visited_pages++;
                total_time += std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
                std::cout << spider.lastHtmlTitle()  << std::endl << spider.lastUrl() << std::endl << std::endl;
            }
            else {
                if(spider.get_NumUnspidered() == 0){
                    std::cout << "FIM" << std::endl;
                }
                std::cout << spider.lastErrorText() << std::endl;
            }
            spider.SleepMs(100);
        }
        std::cout << "Outbounds " << spider.get_NumOutboundLinks() << std::endl;

        for(int i = 0; i < spider.get_NumOutboundLinks(); i++){
            long_term_scheduler.push(std::string(spider.getOutboundLink(i)));
        }
    }

    float total_time_in_seconds = ((float)total_time)/1000.0;
    float average_time_in_seconds = total_time_in_seconds/((float)visited_pages);
    std::cout << "Duration: " << total_time_in_seconds << " seconds" << std::endl;
    std::cout << "Average time spent per page: " << average_time_in_seconds << " seconds" << std::endl;
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