#include<iostream>

#include "./include/crawler.hpp"
#include "./include/indexer.hpp"

#define PAGES_TO_COLLECT 100000

void run(web_crawler::Crawler* crawler, web_crawler::Indexer* indexer, int argc, char* const argv[]){
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
                        indexer->index(argv[i+1]);
                        i++;
                    }
                    else {
                        indexer->index();
                    }
                    break;

                case 'l':
                    if(argc > i + 1 && argv[i+1][0] != '-'){
                        indexer->load_index(argv[i+1]);
                        i++;
                    }
                    else {
                        indexer->load_index(INDEX_PATH);
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
    web_crawler::Indexer* indexer = new web_crawler::Indexer();

    run(crawler, indexer, argc, argv);
    crawler->report();
    indexer->report();

    delete crawler;
    delete indexer;
    return 0;
}