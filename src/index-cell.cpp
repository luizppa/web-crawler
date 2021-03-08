#include<sstream>
#include<iostream>
#include<algorithm>
#include<cmath>

#include"../include/index-cell.hpp"
#include"../include/document-occurrence.hpp"

namespace search_engine{
    IndexCell::IndexCell(std::string term){
        this->term = term;
        this->documents = new std::map<int, DocumentOccurrence*>();
    }
    
    IndexCell::~IndexCell(){
        std::map<int, DocumentOccurrence*>::iterator it;
        for(it = this->documents->begin(); it != this->documents->end(); ++it){
            delete it->second;
        }
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
            document->add_occurence(position);
            documents->insert(std::pair<int, DocumentOccurrence*>(document_id, document));
            this->ni ++;
        }
        else{
            documents->at(document_id)->add_occurence(position);
        }
    }

    void IndexCell::merge(IndexCell* cell){
        if(cell->term.compare(this->term) != 0){
            throw "Terms does not match";
        }
        std::map<int, DocumentOccurrence*>* documents = cell->documents;
        std::map<int, DocumentOccurrence*>::iterator it;
        for(it = documents->begin(); it != documents->end(); ++it){
            DocumentOccurrence* document = it->second;
            std::vector<int>* positions = document->get_positions();
            std::vector<int>::iterator position;
            for(position = positions->begin(); position != positions->end(); ++position){
                this->add_occurence(document->get_id(), *position);
            }
        }
    }

    double IndexCell::tf(int frequency){
        return (1.0 + log((double)frequency));
    }

    double IndexCell::idf(){
        return log(COLLECTION_SIZE/(double)this->get_ni());
    }

    double IndexCell::query_tf_idf(std::string query){
        std::stringstream query_stream(query);
        std::string term;
        int frequency = 0;
        while(std::getline(query_stream, term, ' ')){
            if(this->term.compare(term) == 0){
                frequency++;
            }
        }
        if(frequency > 0){
            return this->tf(frequency) * this->idf();
        }
        else{
            return 0.0;
        }
    }

    double IndexCell::document_tf_idf(int document_id){
        DocumentOccurrence* document = this->documents->operator[](document_id);
        if(document != nullptr){
            return this->tf(document->get_occurrencies()) * this->idf();
        }
        else{
            return 0.0;
        }
    }

    std::string IndexCell::dump(){
        std::stringstream output_stream;
        output_stream << this->get_term() << ' ' << this->get_ni();
        std::map<int, DocumentOccurrence*>* documents = this->get_documents();
        std::map<int, DocumentOccurrence*>::iterator it;
        for(it = documents->begin(); it != documents->end(); ++it){
            DocumentOccurrence* document = it->second;
            output_stream << ' ' << document->get_id() << ' ' << document->get_occurrencies();
            std::vector<int>* positions = document->get_positions();
            for(std::vector<int>::iterator position = positions->begin(); position != positions->end(); ++position){
                output_stream << ' ' << *position;
            }
        }
        return output_stream.str();
    }

    std::string IndexCell::sort_dump(){
        std::stringstream output_stream;
        output_stream << this->get_term() << ' ' << this->get_ni();
        std::map<int, DocumentOccurrence*>* documents = this->get_documents();
        std::map<int, DocumentOccurrence*>::iterator map_it;
        std::vector<DocumentOccurrence*>::iterator vector_it;
        std::vector<DocumentOccurrence*> documents_vector;
        for(map_it = documents->begin(); map_it != documents->end(); ++map_it){
            documents_vector.push_back(map_it->second);
        }
        std::sort(documents_vector.begin(), documents_vector.end(), DocumentOccurrence::compare);

        for(vector_it = documents_vector.begin(); vector_it != documents_vector.end(); ++vector_it){
            DocumentOccurrence* document = *vector_it;
            output_stream << ' ' << document->get_id() << ' ' << document->get_occurrencies();
            std::vector<int>* positions = document->get_positions();
            for(std::vector<int>::iterator position = positions->begin(); position != positions->end(); ++position){
                output_stream << ' ' << *position;
            }
        }
        return output_stream.str();
    }

    IndexCell* IndexCell::load(std::string index_entry){
        std::stringstream stream(index_entry);
        std::string word;
        int ni;

        std::getline(stream, word, ' ');
        stream >> ni;

        IndexCell* cell = new IndexCell(word);
        for(int i = 0; i < ni; i++){
            int document_id, document_occurrences;
            stream >> document_id >> document_occurrences;
            for(int j = 0; j < document_occurrences; j++){
                int position;
                stream >> position;
                cell->add_occurence(document_id, position);
            }
        }
        return cell;
    }

    IndexCell* IndexCell::load(std::string index_entry, int max_documents){
        std::stringstream stream(index_entry);
        std::string word;
        int ni;

        std::getline(stream, word, ' ');
        stream >> ni;
        ni = ni < max_documents ? ni : max_documents;

        IndexCell* cell = new IndexCell(word);
        for(int i = 0; i < ni; i++){
            int document_id, document_occurrences;
            stream >> document_id >> document_occurrences;
            for(int j = 0; j < document_occurrences; j++){
                int position;
                stream >> position;
                cell->add_occurence(document_id, position);
            }
        }
        return cell;
    }

}