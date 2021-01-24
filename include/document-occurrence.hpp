#ifndef DOCUMENT_OCCURRENCE_H
#define DOCUMENT_OCCURRENCE_H

#include<vector>

namespace web_crawler {
    class DocumentOccurrence {
        private:
            int document_id;
            int occurrencies = 0;
            std::vector<int> positions;

        public:
            DocumentOccurrence(int document_id);
            void addOccurence(int position);

    }
}

#endif