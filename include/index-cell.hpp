#ifndef INDEX_CELL_H
#define INDEX_CELL_H

#include<map>

#include"./document-occurence.hpp"

namespace web_crawler {
    class IndexCell {
        private:
            char* term;
            double ni;
            std::map<int, DocumentOccurrence> documents;

        public:
            IndexCell(char* term);
            void addDocument(int document_id, int position);
    }
}

#endif