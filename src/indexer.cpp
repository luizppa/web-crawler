#include<fstream>
#include<iostream>
#include<string>
#include<sstream>
#include<vector>
#include<map>
#include<cstdio>
#include<chrono>

#include "../include/term-sanitizer.hpp"
#include "../include/indexer.hpp"
#include "../third_party/nlohmann/json.hpp"
 
namespace search_engine {

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
    
    void Indexer::index(){
        int i, seconds;
        std::ifstream collection_file(COLLECTION_PATH);
        std::chrono::_V2::system_clock::time_point end_time, start_time;

        start_time = std::chrono::high_resolution_clock::now();

        for(i = 0; !collection_file.eof(); i++){
            this->build_index(collection_file, MAX_DOCUMENTS_PER_BATCH, i);
        }

        end_time = std::chrono::high_resolution_clock::now();
        seconds = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
        std::cout << "Built " << i << " indexes in " << seconds << " seconds, preparing to merge...\n";

        this->merge(i);
        
        for(int j = 0; j < i; j++){
            std::stringstream tmp_file_name;
            tmp_file_name << TEMP_OUTPUT_FOLDER << j << "-index.tmp";
            std::remove(tmp_file_name.str().c_str());
        }
        
        end_time = std::chrono::high_resolution_clock::now();
        seconds = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
        std::cout << "Total indexing time: " << seconds << " seconds \n";
    }

    void Indexer::index(const char* collection_path){
        int i, seconds;
        std::ifstream collection_file(collection_path);
        std::chrono::_V2::system_clock::time_point end_time, start_time;

        start_time = std::chrono::high_resolution_clock::now();

        for(i = 0; !collection_file.eof(); i++){
            this->build_index(collection_file, MAX_DOCUMENTS_PER_BATCH, i);
        }

        end_time = std::chrono::high_resolution_clock::now();
        seconds = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
        std::cout << "Built " << i << " indexes in " << seconds << " seconds, preparing to merge...\n";

        this->merge(i);

        for(int j = 0; j < i; j++){
            std::stringstream tmp_file_name;
            tmp_file_name << TEMP_OUTPUT_FOLDER << j << "-index.tmp";
            std::remove(tmp_file_name.str().c_str());
        }
        
        end_time = std::chrono::high_resolution_clock::now();
        seconds = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
        std::cout << "Total indexing time: " << seconds << " seconds \n";
    }
    
    void Indexer::build_index(std::ifstream& collection_file, int pages_to_index, int iteration){
        std::string line;
        for(int i = 0; i < pages_to_index && std::getline(collection_file, line); i++){
            int position = 0;
            nlohmann::json document_json = nlohmann::json::parse(line);
            std::cout << "Building index for (" << (pages_to_index*iteration)+i << ") " << document_json["url"] << "...\t";
            try{
                std::istringstream file_content(TermSanitizer::html_text(document_json["html_content"]));
                std::string word;
                while(file_content >> word){
                    word = TermSanitizer::sanitize(word);
                    if(word.size() > 0){
                        this->add_to_dictionary(word, (pages_to_index*iteration)+i, position);
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
        if(!this->dictionary->empty()){
            this->save_index(iteration);
            for(std::map<std::string, IndexCell*>::iterator it = this->dictionary->begin(); it != this->dictionary->end(); ++it){
                delete it->second;
            }
            this->dictionary->clear();
        }
    }

    void Indexer::save_index(int iteration){
        std::stringstream output_file_name;
        output_file_name << TEMP_OUTPUT_FOLDER << iteration << "-index.tmp";
        std::ofstream index_file(output_file_name.str());
        for(std::map<std::string, IndexCell*>::iterator it = this->dictionary->begin(); it != this->dictionary->end(); ++it){
            IndexCell* cell = it->second;
            index_file << cell->dump() << '\n';
        }
        index_file.close();
    }

    void Indexer::merge(int indexes_number){
        std::map<std::string, std::pair<std::vector<int>, IndexCell*>> cells;
        std::vector<std::ifstream*> files;
        std::ofstream output_file(INDEX_PATH);
        std::string index_line;
        std::vector<std::ifstream*>::iterator it;
        IndexCell* cell;
        
        auto start_time = std::chrono::high_resolution_clock::now();

        /* Initializes files vector and first index cells */
        std::cout << "Merging indexes...\t";
        for(int i = 0; i < indexes_number; i++){
            std::stringstream file_name;
            file_name << TEMP_OUTPUT_FOLDER << i << "-index.tmp";
            std::ifstream* index_file = new std::ifstream(file_name.str());
            if(index_file->is_open() && std::getline(*index_file, index_line, '\n')){
                files.push_back(index_file);
                cell = IndexCell::load(index_line);
                if(cells.find(cell->get_term()) == cells.end()){
                    std::vector<int> documents;
                    documents.push_back(i);
                    cells[cell->get_term()] = std::pair<std::vector<int>, IndexCell*>(documents, cell);
                }
                else{
                    cells[cell->get_term()].first.push_back(i);
                    cells[cell->get_term()].second->merge(cell);
                    delete cell;
                }
            }
            else if(index_file->is_open()){
                index_file->close();
            }
            else if (index_file != nullptr){
                delete index_file;
            }
        }
        
        /* Read, merges and saves index cells */
        std::pair<std::vector<int>, IndexCell*> pair;
        while(!cells.empty()){
            pair = cells.begin()->second;
            std::vector<int> documents_to_consume = pair.first;
            cell = pair.second;
            output_file << cell->dump() << '\n';
            delete cell;
            cells.erase(cells.begin());
            std::vector<int>::iterator document;
            for(document = documents_to_consume.begin(); document != documents_to_consume.end(); ++document){
                if(std::getline(*files.at(*document), index_line, '\n')){
                    cell = IndexCell::load(index_line);
                    if(cells.find(cell->get_term()) == cells.end()){
                        std::vector<int> documents;
                        documents.push_back(*document);
                        cells[cell->get_term()] = std::pair<std::vector<int>, IndexCell*>(documents, cell);
                    }
                    else{
                        cells[cell->get_term()].first.push_back(*document);
                        cells[cell->get_term()].second->merge(cell);
                        delete cell;
                    }
                }
            }
        }

        /* Close files */
        output_file.close();
        for(it = files.begin(); it != files.end(); ++it){
            std::ifstream* index_file = *it;
            index_file->close();
            delete index_file;
        }
        std::cout << "done!\n";
        auto end_time = std::chrono::high_resolution_clock::now();
        int seconds = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
        std::cout << "Merged " << indexes_number << " indexes in " << seconds << " seconds.\n";
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