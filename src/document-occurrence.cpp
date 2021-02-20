#include"../include/document-occurrence.hpp"

namespace web_crawler{
    DocumentOccurrence::DocumentOccurrence(int document_id){
        this->document_id = document_id;
        this->positions = new std::vector<int>();
    }
    
    DocumentOccurrence::~DocumentOccurrence(){
        delete this->positions;
    }

    int DocumentOccurrence::get_id(){
        return this->document_id;
    }

    int DocumentOccurrence::get_occurrencies(){
        return this->occurrencies;
    }

    std::vector<int>* DocumentOccurrence::get_positions(){
        return this->positions;
    }

    void DocumentOccurrence::add_occurence(int position){
        this->positions->push_back(position);
        this->occurrencies++;
    }
}