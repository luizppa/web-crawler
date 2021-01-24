#include"../include/index-cell.hpp"
#include"../include/document-occurrence.hpp"

namespace web_crawler{
    IndexCell::IndexCell(char* term){
        this->term = term;
    }
    void IndexCell::addDocument(int document_id, int position){
        if(this->documents.find(document_id) == this->documents.end()){
            DocumentOccurrence* document = new DocumentOccurrence(document_id);
            document.addOccurence(position);
            documents[document_id] = document;
            this->ni ++;
        }
        else{
            documents[document_id].addOccurence(position);
        }
    }
}