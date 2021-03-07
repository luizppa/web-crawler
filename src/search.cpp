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

    /**
     * @brief Searches for a line staring with a given term on the file located at the provided path.
     * The file must be in lexical order of the first term for the search to work.
     * Complexity O(logn)
     * @param search_term String term to be searched
     * @param file_path Path to the file in which to perform the search
     * @return string of the line that matched the search, empty string if no match was found
    */
    std::string file_binary_search(std::string search_term, std::string file_path){
        std::ifstream file(file_path, std::ios::binary);
        std::string term, file_entry;
        std::stringstream file_entry_stream;
        long long int begin = 0, end, middle, last_middle = 0;
        file.seekg(0, std::ios::end);
        end = (long long int)file.tellg();

        while(begin != end && middle != last_middle){
            last_middle = middle;
            middle = (long long int)((begin+end)/2.0);
            file.seekg(middle, std::ios::beg);
            std::getline(file, file_entry, '\n');

            std::getline(file, file_entry, '\n');
            file_entry_stream.str(file_entry);
            std::getline(file_entry_stream, term, ' ');
            if(search_term.compare(term) == 0){
                file.close();
                return file_entry;
            }
            else if(std::lexicographical_compare(search_term.begin(), search_term.end(), term.begin(), term.end())){
                end = middle;
            }
            else{
                begin = middle;
            }
        }
        file.close();
        return "";
    }

    /**
     * @brief Searches for a line staring with a given value on the file located at the provided path.
     * The file must be in crescent order of the first value for the search to work.
     * Complexity O(logn)
     * @param search_value int value to be searched
     * @param file_path Path to the file in which to perform the search
     * @return int with the character position of the start of the line where the value was found or -1 if it was not found
    */
    std::string file_binary_search(int search_value, std::string file_path){
        std::ifstream file(file_path, std::ios::binary);
        std::string file_entry;
        std::stringstream file_entry_stream;
        int value;
        long long int begin = 0, end, middle, last_middle = 0;
        file.seekg(0, std::ios::end);
        end = (long long int)file.tellg();

        while(begin != end && middle != last_middle){
            last_middle = middle;
            middle = (long long int)((begin+end)/2.0);
            file.seekg(middle, std::ios::beg);
            std::getline(file, file_entry, '\n');

            std::getline(file, file_entry, '\n');
            file_entry_stream.str(file_entry);
            file_entry_stream >> value;
            if(value == search_value){
                file.close();
                return file_entry;
            }
            else if(search_value < value){
                end = middle;
            }
            else{
                begin = middle;
            }
        }
        file.close();
        return "";
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

    std::string search_document_briefing(int id, std::string briefing_file_path){
        std::string document_line = file_binary_search(id, briefing_file_path);
        if(document_line.length() <= 0){
            return "";
        }
        std::string id_str;
        std::string url;
        std::string briefing;
        std::stringstream response_stream;
        
        std::stringstream document_briefing_stream(document_line);
        // std::cout << "Index entry: " << document_line << "\n";
        std::getline(document_briefing_stream, id_str, ' ');
        std::getline(document_briefing_stream, url, ' ');
        std::getline(document_briefing_stream, briefing, '\n');
        response_stream << '(' << id_str << ") " << url << '\n' << briefing;
        return response_stream.str();
    }

    std::map<int, std::string>* build_response(std::map<std::string, IndexCell*>* response_entries, std::string briefing_file_path){
        std::map<std::string, IndexCell*>::iterator index_it;
        std::map<int, DocumentOccurrence*>::iterator document_it;
        std::map<int, std::string>::iterator response_it;
        std::map<int, std::string>* response_documents = new std::map<int, std::string>();
        std::string briefing_line;

        if(response_entries->size() <= 0){
            return response_documents;
        }

        for(index_it = response_entries->begin(); index_it != response_entries->end(); ++index_it){
            IndexCell* cell = index_it->second;
            if(cell != nullptr){
                std::map<int, DocumentOccurrence*>* documents = cell->get_documents();
                for(document_it = documents->begin(); document_it != documents->end(); ++document_it){
                    // std::cout << "Searching for document " << document_it->second->get_id() << "\n";
                    std::string document_briefing = search_document_briefing(document_it->second->get_id(), briefing_file_path);
                    std::pair<int, std::string> response_pair(document_it->first, document_briefing);
                    response_documents->insert(response_pair);
                }
            }
        }

        return response_documents;
    }

    void search(std::string query, std::string index_path, std::string briefing_path){
        std::map<std::string, IndexCell*>* query_params = get_query_params(query);
        std::map<std::string, IndexCell*>::iterator query_it = query_params->begin();
        std::map<int, std::string>::iterator response_it;
        std::map<std::string, IndexCell*>::iterator index_it;
        std::string term;

        while(query_it != query_params->end()){
            term = query_it->first;
            ++query_it;
            std::string index_entry = file_binary_search(term, index_path);
            if(index_entry.length() > 0){
                IndexCell* cell = IndexCell::load(index_entry, 5);
                // std::cout << "Index entry: " << index_entry.substr(0, 50) << '\n';
                // std::cout << "Found term " << cell->get_term() << " in " << cell->get_ni() << " relevant documents\n";
                query_params->operator[](term) = cell;
            }
            else{
                std::cout << "Could not find any result matching the provided search\n\n";
                query_params->erase(term);
            }
        }

        std::map<int, std::string>* response = build_response(query_params, briefing_path);
        for(response_it = response->begin(); response_it != response->end(); ++response_it){
            std::string response_entry = response_it->second;
            if(response_entry.length() > 0){
                std::cout << response_entry << "\n\n";
            }
        }

        for(index_it = query_params->begin(); index_it != query_params->end(); ++index_it){
            delete index_it->second;
        }
        delete query_params;
        delete response;
    }

}