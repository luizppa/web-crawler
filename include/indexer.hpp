#ifndef INDEXER_H
#define INDEXER_H

#include<map>
#include<string>

#include"./index-cell.hpp"

#define COLLECTION_PATH "./output/collection.jl"
#define INDEX_PATH "./output/index.idx"

namespace web_crawler {

    class Indexer {
        private:
            std::map<std::string, IndexCell*>* dictionary;
            void add_to_dictionary(std::string word, int document, int position);
            void load_index(std::ifstream& index_file);

        public:
            Indexer();
            ~Indexer();
            void build_index(int pages_to_index);
            void build_index(const char* html_path, int pages_to_index);
            void save_index();
            void load_index(std::string file_path);
            void load_index(const char* file_path);
            void report();
    };

}

#endif