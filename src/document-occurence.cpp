#include"../document-occurence.hpp"

namespace web_crawler{
    DocumentOccurrence::DocumentOccurrence(int document_id){
        this->document_id = document_id;
    }

    void DocumentOccurrence::addOccurence(int position){
        this->positions.pushBack(position);
        this->occurrencies++;
    }
}