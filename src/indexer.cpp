#include<fstream>
#include<iostream>
#include<string>
#include<sstream>

#include "../include/term-sanitizer.hpp"
#include "../include/indexer.hpp"
#include "../third_party/nlohmann/json.hpp"
 
namespace web_crawler {

    Indexer::Indexer(){
        this->dictionary = new std::map<std::string, IndexCell*>();
    }

    Indexer::~Indexer(){
        delete this->dictionary;
    }

    void Indexer::add_to_dictionary(std::string word, int document, int position){
        IndexCell* dictionary_entry;
        if(this->dictionary->find(word) == this->dictionary->end()){
            dictionary_entry = new IndexCell(word);
            this->dictionary->insert(std::pair<std::string, IndexCell*>(word, dictionary_entry));
        }
        else{
            dictionary_entry = this->dictionary->at(word);
        }
        dictionary_entry->add_occurence(document, position);
    }

    void Indexer::build_index(int pages_to_index){
        std::ifstream index(COLLECTION_PATH);
        std::string line;
        for(int i = 0; i < pages_to_index && std::getline(index, line); i++){
            int position = 0;
            std::cout << "Building index for " << i+1 << "th page...\t";
            nlohmann::json document_json = nlohmann::json::parse(line);
            std::cout << document_json["url"] << "\n";
            try{
                std::istringstream file_content(TermSanitizer::html_text(document_json["html_content"]));
                std::string word;
                while(file_content >> word){
                    word = TermSanitizer::sanitize(word);
                    if(word.size() > 0){
                        this->add_to_dictionary(word, i, position);
                        position++;
                    }
                }
                std::cout << "done\n";
            }
            catch(...){
                std::cout << "failed\n";
                continue;
            }
        }
    }

    void Indexer::build_index(const char* html_path, int pages_to_index){
        for(int i = 1; i <= pages_to_index; i++){
            std::stringstream file_name;
            int position = 0;
            file_name << html_path << i << ".html";
            std::cout << "Building index for " << file_name.str() << "...\t";
            try{
                std::istringstream file_content(TermSanitizer::html_text(file_name.str()));
                std::string word;
                while(file_content >> word){
                    word = TermSanitizer::sanitize(word);
                    if(word.size() > 0){
                        this->add_to_dictionary(word, i, position);
                        position++;
                    }
                }
                std::cout << "done\n";
            }
            catch(...){
                std::cout << "failed\n";
                continue;
            }
        }
    }

    void Indexer::save_index(){
        std::ofstream index_file(INDEX_PATH);
        for(std::map<std::string, IndexCell*>::iterator it = this->dictionary->begin(); it != this->dictionary->end(); ++it){
            IndexCell* cell = it->second;
            index_file << cell->get_term() << ' ' << cell->get_ni();
            std::map<int, DocumentOccurrence*>* documents = cell->get_documents();
            for(std::map<int, DocumentOccurrence*>::iterator doc_it = documents->begin(); doc_it != documents->end(); ++doc_it){
                DocumentOccurrence* document = doc_it->second;
                index_file << ' ' << document->get_id() << ' ' << document->get_occurrencies();
                std::vector<int>* positions = document->get_positions();
                for(std::vector<int>::iterator position = positions->begin(); position != positions->end(); ++position){
                    index_file << ' ' << *position;
                }
            }
            index_file << '\n';
        }
        index_file.close();
    }

    void Indexer::load_index(std::ifstream& index_file){
        std::string word;
        while(std::getline(index_file, word, ' ')){
            int ni;
            index_file >> ni;
            for(int i = 0; i < ni; i++){
                int document_id, document_occurrences;
                index_file >> document_id >> document_occurrences;;
                for(int j = 0; j < document_occurrences; j++){
                    int position;
                    index_file >> position;
                    this->add_to_dictionary(word, document_id, position);
                }
            }
        }
    }

    void Indexer::load_index(const char* file_path){
        std::ifstream index_file = std::ifstream(file_path);
        this->load_index(index_file);
    }

    void Indexer::load_index(std::string file_path){
        std::ifstream index_file = std::ifstream(file_path);
        this->load_index(index_file);
    }

    void Indexer::report(){
        std::cout << this->dictionary->size() << " words in dictionary\n";
        int word_occurrences = 0;
        for(std::map<std::string, IndexCell*>::iterator it = this->dictionary->begin(); it != this->dictionary->end(); ++it){
            IndexCell* index = it->second;
            word_occurrences += index->get_ni();
        }
        double list_average_size = ((float)word_occurrences)/((float)this->dictionary->size());
        std::cout << "Index entry average size: " << list_average_size << " occurrences\n";
    }

}