#ifndef DOCUMENT_OCCURRENCE_H
#define DOCUMENT_OCCURRENCE_H

#include<vector>

namespace search_engine {
    class DocumentOccurrence {
        private:
            int document_id;
            int occurrencies = 0;
            std::vector<int>* positions;

        public:
            DocumentOccurrence(int document_id);
            ~DocumentOccurrence();
            int get_id();
            int get_occurrencies();
            std::vector<int>* get_positions();
            void add_occurence(int position);

    };
}

#endif