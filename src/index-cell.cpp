#include"../include/index-cell.hpp"
#include"../include/document-occurrence.hpp"

namespace web_crawler{
    IndexCell::IndexCell(std::string term){
        this->term = term;
        this->documents = new std::map<int, DocumentOccurrence*>();
    }
    
    IndexCell::~IndexCell(){
        delete this->documents;
    }

    std::string IndexCell::get_term(){
        return this->term;
    }

    int IndexCell::get_ni(){
        return this->ni;
    }

    std::map<int, DocumentOccurrence*>* IndexCell::get_documents(){
        return this->documents;
    }

    void IndexCell::add_occurence(int document_id, int position){
        if(this->documents->find(document_id) == this->documents->end()){
            DocumentOccurrence* document = new DocumentOccurrence(document_id);
            document->addOccurence(position);
            documents->insert(std::pair<int, DocumentOccurrence*>(document_id, document));
            this->ni ++;
        }
        else{
            documents->at(document_id)->addOccurence(position);
        }
    }
}