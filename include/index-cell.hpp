#ifndef INDEX_CELL_H
#define INDEX_CELL_H

#include<map>
#include<string>

#include"./document-occurrence.hpp"

namespace web_crawler {
    class IndexCell {
        private:
            std::string term;
            double ni;
            std::map<int, DocumentOccurrence*> documents;

        public:
            IndexCell(std::string term);
            void addDocument(int document_id, int position);
    };
}

#endif