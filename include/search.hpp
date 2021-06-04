#ifndef SEARCH_H
#define SEARCH_H

#define RESULTS_PER_PAGE 5

#include<string>
#include<map>

#include"./index-cell.hpp"

#include"../third_party/nlohmann/json.hpp"

namespace search_engine {

    class Search {

        private:
            static std::map<int, std::string> collection_index;
            static std::map<int, double> rank_weights;

            static long long int file_binary_search(std::string search_term, char const* file_path);
            static std::string file_binary_search(int search_value, char const* file_path);
            static std::map<std::string, IndexCell*>* get_query_params(std::string query);
            static std::string search_document_briefing(int id, char const* briefing_file_path);
            static IndexCell* search_index_entry(std::string word, char const* index_path);
            static double document_relevance(std::map<std::string, IndexCell*>* query_params, std::string query, int document_id);
            static bool is_valid_term(std::string term);
            static bool rank(std::pair<int, double> a, std::pair<int, double> b);
            static std::vector<std::string> build_response(std::map<std::string, IndexCell*>* query_params, std::string query, char const* briefing_file_path, int max_results);
            static float get_page_weight(int page);

        public:
            static void init_vocabulary(char const* vocabulary_path);
            static void init_rank_weights(char const* rank_path);
            static void execute_query(std::string query, char const* index_path, char const* collection_path);
    };
    
}

#endif