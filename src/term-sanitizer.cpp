#include<sstream>

#include"../include/term-sanitizer.hpp"

namespace web_crawler {

    bool TermSanitizer::char_is_in(char c, const std::pair<int, int> intervals[], int n){
        for(int i = 0; i < n; i++){
            if(c >= intervals[i].first && c <= intervals[i].second){
                return true;
            }
        }
        return false;
    }

    std::string TermSanitizer::sanitize(std::string term){
        std::stringstream stream;
        std::string::iterator it = term.begin();
        while(it != term.end()){
            if(!TermSanitizer::char_is_in(*it, TermSanitizer::exclude_intervals, EXCLUDE_INTERVALS_SIZE)){
                stream << *it;
            }
            ++it;
        }
        return stream.str();
    }
}