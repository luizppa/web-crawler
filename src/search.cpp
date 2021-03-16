#include<fstream>
#include<map>
#include<vector>
#include<set>
#include<string>
#include<sstream>
#include<iostream>
#include<algorithm>
#include<chrono>

#include "../include/log-utils.hpp"
#include"../include/search.hpp"
#include"../include/index-cell.hpp"
#include"../include/term-sanitizer.hpp"
#include"../include/document-occurrence.hpp"
#include "../third_party/nlohmann/json.hpp"

namespace search_engine {

    std::map<std::string, int> Search::vocabulary_index;
    std::map<int, double> Search::rank_weights;

    /**
     * @brief Searches for a line staring with a given term on the file located at the provided path.
     * The file must be in lexical order of the first term for the search to work.
     * Complexity O(logn)
     * @param search_term String term to be searched
     * @param file_path Path to the file in which to perform the search
     * @return string of the line that matched the search, empty string if no match was found
    */
    std::string Search::file_binary_search(std::string search_term, char const* file_path){
        std::ifstream file(file_path, std::ios::binary);
        std::string term, file_entry;
        std::stringstream file_entry_stream;
        long long int begin = 0, end, middle = -1, last_middle = 0;
        file.seekg(0, std::ios::end);
        end = (long long int)file.tellg();
        file.seekg(0, std::ios::beg);

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
    std::string Search::file_binary_search(int search_value, char const* file_path){
        std::ifstream file(file_path, std::ios::binary);
        std::string file_entry;
        int i = 0, value;
        std::stringstream file_entry_stream;
        long long int begin = 0, end, middle = -1, last_middle = 0;
        file.seekg(0, std::ios::end);
        end = (long long int)file.tellg();
        file.seekg(0, std::ios::beg);

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
            i++;
        }
        file.close();
        return "";
    }

    std::map<std::string, IndexCell*>* Search::get_query_params(std::string query){
        std::stringstream query_stream(query);
        std::map<std::string, IndexCell*>* query_params = new std::map<std::string, IndexCell*>();
        std::string term;

        while(query_stream >> term){
            query_params->insert(std::pair<std::string, IndexCell*>(TermSanitizer::sanitize(term), nullptr));
        }

        return query_params;
    }

    std::string Search::search_document_briefing(int id, char const* briefing_path){
        std::string document_line;
        document_line = file_binary_search(id, briefing_path);

        if(document_line.length() <= 0){
            throw std::logic_error("Resource not found");
        }
        std::string id_str;
        std::string url;
        std::string body;
        
        std::stringstream document_briefing_stream(document_line);
        std::getline(document_briefing_stream, id_str, ' ');
        std::getline(document_briefing_stream, url, '\n');
        return url;
    }

    IndexCell* Search::search_index_entry(std::string word, char const* index_path){
        std::string index_entrystring;
        if(Search::vocabulary_index.find(word) != Search::vocabulary_index.end()){
            std::ifstream index_file(index_path);
            index_file.seekg(Search::vocabulary_index[word], std::ios::beg);
            std::getline(index_file, index_entrystring);
            index_file.close();
        }
        index_entrystring = file_binary_search(word, index_path);
        if(index_entrystring.length() > 0){
            IndexCell* index_entry = IndexCell::load(index_entrystring);
            return index_entry;
        }
        else{
            return nullptr;
        }
    }

    double Search::document_relevance(std::map<std::string, IndexCell*>* query_params, std::string query, int document_id){
        std::map<std::string, IndexCell*>::iterator param;
        double internal_product = 0.0;
        double q_norm = 0.0;
        double d_norm = 0.0;
        float page_weight = get_page_weight(document_id);
        for(param = query_params->begin(); param != query_params->end(); ++param){
            IndexCell* cell = param->second;
            if(cell != nullptr){
                internal_product += (cell->query_tf_idf(query) * cell->document_tf_idf(document_id) * page_weight);
                q_norm += std::pow(cell->query_tf_idf(query), 2);
                d_norm += std::pow((cell->document_tf_idf(document_id) * page_weight), 2);
            }
        }

        return internal_product/std::sqrt(d_norm) * std::sqrt(q_norm);
    }

    bool Search::rank(std::pair<int, double> a, std::pair<int, double> b){
        return a.second > b.second;
    }

    std::vector<std::string> Search::build_response(std::map<std::string, IndexCell*>* query_params, std::string query, char const* briefing_path, int max_results){
        std::map<std::string, IndexCell*>::iterator index_it;
        std::map<int, DocumentOccurrence*>::iterator document_it;
        std::vector<std::pair<int, double>>::iterator response_it;
        std::vector<std::pair<int, double>> response_documents;
        std::set<int> response_set;
        std::vector<std::string> response;
        std::string briefing_line;

        if(query_params->size() <= 0){
            return response;
        }

        for(index_it = query_params->begin(); index_it != query_params->end(); ++index_it){
            IndexCell* cell = index_it->second;
            if(cell != nullptr && (cell->get_ni()/COLLECTION_SIZE) < 0.1){
                std::map<int, DocumentOccurrence*>* documents = cell->get_documents();
                for(document_it = documents->begin(); document_it != documents->end(); ++document_it){
                    int document_id = document_it->first;
                    if(response_set.find(document_id) == response_set.end()){
                        double document_relevance = Search::document_relevance(query_params, query, document_id);
                        response_documents.push_back(std::pair<int, double>(document_id, document_relevance));
                        response_set.insert(document_id);
                    }
                }
            }
        }

        int i = 0;
        std::sort(response_documents.begin(), response_documents.end(), Search::rank);
        for(response_it = response_documents.begin(); response_it != response_documents.end() && i != max_results; ++response_it, i++){
            std::stringstream response_text;
            std::string document_url = Search::search_document_briefing(response_it->first, briefing_path);
            response_text << BOLDBLUE << document_url << RESET;
            response.push_back(response_text.str());
        }

        return response;
    }

    void Search::execute_query(std::string query, char const* index_path, char const* briefing_path){
        std::map<std::string, IndexCell*>* query_params = get_query_params(query);
        std::map<std::string, IndexCell*>::iterator query_it = query_params->begin();
        std::vector<std::string>::iterator response_it;
        std::map<std::string, IndexCell*>::iterator index_it;
        std::string term;
        
        std::cout << "================================================================\n";
        auto start_time = std::chrono::high_resolution_clock::now();
        while(query_it != query_params->end()){
            term = query_it->first;
            ++query_it;
            IndexCell* cell = Search::search_index_entry(term, index_path);
            if(cell != nullptr){
                query_params->operator[](term) = cell;
            }
            else{
                std::cout << "Could not find any results matching " << term << "\n\n";
                query_params->erase(term);
            }
        }

        std::vector<std::string> response = Search::build_response(query_params, query, briefing_path, RESULTS_PER_PAGE);
        for(response_it = response.begin(); response_it != response.end(); ++response_it){
            std::string response_entry = *response_it;
            if(response_entry.length() > 0){
                std::cout << response_entry << "\n\n";
            }
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        std::cout << GREEN << "Done in " << milliseconds/1000.0 << " seconds.\n" << RESET;
        std::cout << "================================================================\n\n";

        for(index_it = query_params->begin(); index_it != query_params->end(); ++index_it){
            delete index_it->second;
        }
        delete query_params;
    }

    float Search::get_page_weight(int page){
        if(Search::rank_weights.empty()){
            return 1.0;
        }
        else if(Search::rank_weights.find(page) != Search::rank_weights.end()){
            return (float) Search::rank_weights[page];
        }
        else {
            return 0.0;
        }
    }

    void Search::init_vocabulary(char const* vocabulary_path){
        std::ifstream vocabulary_file(vocabulary_path);
        std::string word, line;
        int position = vocabulary_file.tellg();

        while(vocabulary_file >> word){
            Search::vocabulary_index[word] = position;
            std::getline(vocabulary_file, line);
            position = vocabulary_file.tellg();
        }

        vocabulary_file.close();
    }

    void Search::init_rank_weights(char const* rank_path){
        std::ifstream rank_file(rank_path);
        int page;
        double weight;

        if(rank_file.is_open()){
            while(rank_file >> page){
                rank_file >> weight;
                Search::rank_weights[page] = weight;
            }
        }
    }

}