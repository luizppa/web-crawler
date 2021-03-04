#include<fstream>
#include<map>
#include<string>
#include<sstream>
#include<iostream>
#include<algorithm>

#include"../include/search.hpp"
#include"../include/index-cell.hpp"
#include"../include/document-occurrence.hpp"
#include "../third_party/nlohmann/json.hpp"

namespace search_engine {

    void go_to_line_begin(std::ifstream &file){
        char c;

        file >> c;
        while(c != '\n' && file.tellg() > 1){
            file.seekg(((int)file.tellg() - 1), std::ios::beg);
            file >> c;
            std::cout << c; 
        }

        if(c != '\n'){
          file.seekg(0, std::ios::beg);  
        }
    }

    std::map<std::string, IndexCell*>* get_query_params(std::string query){
        std::stringstream query_stream(query);
        std::map<std::string, IndexCell*>* query_params = new std::map<std::string, IndexCell*>();
        std::string term;

        while(query_stream >> term){
            query_params->insert(std::pair<std::string, IndexCell*>(term, nullptr));
        }

        return query_params;
    }

    std::string search_index_entry(std::string search_term, std::string index_path){
        std::ifstream index_file(index_path, std::ios::binary);
        std::string term, index_entry;
        std::stringstream index_entry_stream;
        long long int begin = 0, end, middle, last_middle = 0;
        index_file.seekg(0, std::ios::end);
        end = (long long int)index_file.tellg();

        while(begin != end && middle != last_middle){
            last_middle = middle;
            middle = (long long int)((begin+end)/2.0);
            index_file.seekg(middle, std::ios::beg);
            std::getline(index_file, index_entry, '\n');

            std::getline(index_file, index_entry, '\n');
            index_entry_stream.str(index_entry);
            std::getline(index_entry_stream, term, ' ');
            if(search_term.compare(term) == 0){
                return index_entry;
            }
            else if(std::lexicographical_compare(search_term.begin(), search_term.end(), term.begin(), term.end())){
                end = middle;
            }
            else{
                begin = middle;
            }
        }

        return "";
    }

    std::map<int, std::string>* build_response(std::map<std::string, IndexCell*>* response_entries, std::string collection_path){
        std::map<std::string, IndexCell*>::iterator index_it;
        std::map<int, DocumentOccurrence*>::iterator document_it;
        std::map<int, std::string>::iterator response_it;
        std::ifstream collection_file(collection_path);
        std::map<int, std::string>* response_documents = new std::map<int, std::string>();
        std::string collection_line;
        unsigned int documents_found = 0;

        if(response_entries->size() <= 0){
            return response_documents;
        }

        for(index_it = response_entries->begin(); index_it != response_entries->end(); ++index_it){
            IndexCell* cell = index_it->second;
            if(cell != nullptr){
                std::map<int, DocumentOccurrence*>* documents = cell->get_documents();
                for(document_it = documents->begin(); document_it != documents->end(); ++document_it){
                    response_documents->insert(std::pair<int, std::string>(document_it->first, ""));
                }
            }
        }

        for(int i = 0; std::getline(collection_file, collection_line, '\n'); i++){
            if(response_documents->find(i) != response_documents->end()){
                nlohmann::json document_json = nlohmann::json::parse(collection_line);
                std::string url = document_json["url"];
                response_documents->operator[](i) = url;
                documents_found++;
                if(documents_found == response_documents->size()){
                    break;
                }
            }
        }

        return response_documents;
    }

    void search(std::string query, std::string index_path, std::string collection_path){
        std::map<std::string, IndexCell*>* query_params = get_query_params(query);
        std::map<std::string, IndexCell*>::iterator query_it = query_params->begin();
        std::map<int, std::string>::iterator response_it;
        std::map<std::string, IndexCell*>::iterator index_it;
        std::string term;

        while(query_it != query_params->end()){
            term = query_it->first;
            ++query_it;
            std::string index_entry = search_index_entry(term, index_path);
            if(index_entry.length() > 0){
                IndexCell* cell = IndexCell::load(index_entry, 5);
                query_params->operator[](term) = cell;
            }
            else{
                std::cout << "Sem correspondencia\n";
                query_params->erase(term);
            }
        }

        std::map<int, std::string>* response = build_response(query_params, collection_path);
        for(response_it = response->begin(); response_it != response->end(); ++response_it){
            std::string url = response_it->second;
            std::cout << url << "\n\n";
        }

        for(index_it = query_params->begin(); index_it != query_params->end(); ++index_it){
            delete index_it->second;
        }
        delete query_params;
        delete response;
    }

}