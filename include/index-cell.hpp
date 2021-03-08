#ifndef INDEX_CELL_H
#define INDEX_CELL_H

#include<map>
#include<string>

#include"./document-occurrence.hpp"

#define COLLECTION_SIZE 1000068.0

namespace search_engine {

    class IndexCell {
        private:
            std::string term;
            int ni = 0;
            std::map<int, DocumentOccurrence*>* documents;
            double tf(int frequency);
            double idf();

        public:
            IndexCell(std::string term);
            ~IndexCell();
            int get_ni();
            void add_occurence(int document_id, int position);
            void merge(IndexCell* cell);
            double query_tf_idf(std::string query);
            double document_tf_idf(int document_id);
            std::map<int, DocumentOccurrence*>* get_documents();
            std::string get_term();
            std::string dump();
            std::string sort_dump();
            static IndexCell* load(std::string index_entry);
            static IndexCell* load(std::string index_entry, int max_documents);
    };
}

#endif