#ifndef INDEX_CELL_H
#define INDEX_CELL_H

#include<map>
#include<string>

#include"./document-occurrence.hpp"

namespace web_crawler {

    class IndexCell {
        private:
            std::string term;
            int ni = 0;
            std::map<int, DocumentOccurrence*>* documents;

        public:
            IndexCell(std::string term);
            ~IndexCell();
            int get_ni();
            void add_occurence(int document_id, int position);
            void merge(IndexCell* cell);
            std::map<int, DocumentOccurrence*>* get_documents();
            std::string get_term();
            std::string dump();
            static IndexCell* load(std::string index_entry);
    };
}

#endif