#ifndef SEARCH_H
#define SEARCH_H

#define RESULTS_PER_PAGE 5

#include<string>

namespace search_engine {
    
    void search(std::string query, std::string index_path, std::string collection_path);
}

#endif