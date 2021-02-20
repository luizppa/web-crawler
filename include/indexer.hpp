#ifndef INDEXER_H
#define INDEXER_H

#include<map>
#include<string>

#include"./index-cell.hpp"

#define COLLECTION_PATH "./output/collection.jl"
#define INDEX_PATH "./output/index.idx"
#define TEMP_OUTPUT_FOLDER "./output/"
#define MAX_DOCUMENTS_PER_BATCH 3

namespace web_crawler {

    class Indexer {
        private:
            std::map<std::string, IndexCell*>* dictionary;
            void add_to_dictionary(std::string word, int document, int position);
            void load_index(std::ifstream& index_file);

        public:
            Indexer();
            ~Indexer();
            void index();
            void index(const char* collection_path);
            void build_index(std::ifstream& collection_file, int pages_to_index, int iteration);
            void save_index(int iteration);
            void load_index(std::string file_path);
            void load_index(const char* file_path);
            void report();
    };

}

#endif