#include<iostream>

#include "./include/crawler.hpp"

#define PAGES_TO_COLLECT 100000
#define PAGES_TO_INDEX 5000

void run(web_crawler::Crawler* crawler, int argc, char* const argv[]){
    int i = 1;
    while(i < argc){
        if(argv[i][0] == '-'){
            int op = (int)argv[i][1];
            switch(op){
                case 'c':
                    if(argc > i + 1 && argv[i+1][0] != '-'){
                        crawler->crawl(argv[i+1], PAGES_TO_COLLECT);
                        crawler->join_tasks();
                        i++;
                    }
                    else {
                        std::cout << "Seed not provided\n";
                    }
                    break;

                case 'b':
                    if(argc > i + 1 && argv[i+1][0] != '-'){
                        crawler->build_index(argv[i+1], PAGES_TO_INDEX);
                        crawler->save_index();
                        i++;
                    }
                    else {
                        crawler->build_index(PAGES_TO_INDEX);
                        crawler->save_index();
                    }
                    break;

                case 'l':
                    if(argc > i + 1 && argv[i+1][0] != '-'){
                        crawler->load_index(argv[i+1]);
                        i++;
                    }
                    else {
                        crawler->load_index(INDEX_PATH);
                    }
                    break;
                
                default:
                    std::cout << "Unkown option \"" << argv[i] << "\"\n";
                    break;
            }
        }
        else{
            std::cout << "Unkown option \"" << argv[i] << "\"\n";
        }
        i++;
    }
}

int main(int argc, char* const argv[]){
    if(argc < 2){
        std::cout << "Not enough arguments" << std::endl;
        return 1;
    }
    web_crawler::Crawler* crawler = new web_crawler::Crawler();
    run(crawler, argc, argv);
    crawler->report();
    delete crawler;
    return 0;
}