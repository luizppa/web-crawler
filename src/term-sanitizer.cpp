#include<sstream>
#include<codecvt>
#include <locale>
#include<algorithm>

#include"../include/term-sanitizer.hpp"

namespace search_engine {

    bool TermSanitizer::char_is_in(wchar_t c, const std::pair<int, int> intervals[], int n){
        for(int i = 0; i < n; i++){
            if(c >= intervals[i].first && c <= intervals[i].second){
                return true;
            }
        }
        return false;
    }

    bool TermSanitizer::char_is_in(wchar_t c, const std::pair<int, int> interval){
        return (c >= interval.first && c <= interval.second);
    }

    wchar_t TermSanitizer::shift(wchar_t c, const std::pair<std::pair<int, int>, int> intervals[], int n){
        for(int i = 0; i < n; i++){
            if(TermSanitizer::char_is_in(c, intervals[i].first)){
                return (c + intervals[i].second);
            }
        }
        return c;
    }

    char TermSanitizer::map(wchar_t c, const std::pair<std::pair<int, int>, char> intervals[], int n){
        for(int i = 0; i < n; i++){
            if(TermSanitizer::char_is_in(c, intervals[i].first)){
                return intervals[i].second;
            }
        }
        return (char)c;
    }

    std::wstring TermSanitizer::to_wstring(std::string str){
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring wstr = converter.from_bytes(str);
        return wstr;
    }

    std::string TermSanitizer::sanitize(std::string term){
        std::wstring ws_term = TermSanitizer::to_wstring(term);
        std::stringstream stream;
        std::wstring::iterator it = ws_term.begin();
        while(it != ws_term.end()){
            if(!TermSanitizer::char_is_in(*it, TermSanitizer::exclude_intervals, EXCLUDE_INTERVALS_SIZE)){
                wchar_t shifted = TermSanitizer::shift(*it, TermSanitizer::shift_intervals, SHIFT_INTERVALS_SIZE);
                stream << TermSanitizer::map(shifted, TermSanitizer::mapings, MAPINGS_SIZE);
            }
            ++it;
        }
        return stream.str();
        return term;
    }

    std::string TermSanitizer::replace_all(std::string string, char a, std::string b){
        std::stringstream stream;
        for(std::string::iterator it = string.begin(); it != string.end(); ++it){
            if(*it == a){
                stream << b;
            }
            else{
                stream << *it;
            }
        }
        return stream.str();
    }

    std::string TermSanitizer::clean_text(GumboNode* node){
        if(node->type == GUMBO_NODE_TEXT){
            return std::string(node->v.text.text);
        }
        else if(node->type == GUMBO_NODE_ELEMENT && node->v.element.tag != GUMBO_TAG_SCRIPT && node->v.element.tag != GUMBO_TAG_STYLE){
            GumboVector* children = &node->v.element.children;
            std::string contents = "";

            for(unsigned int i = 0; i < children->length; ++i){
                const std::string text = TermSanitizer::clean_text((GumboNode*) children->data[i]);
                if(i != 0 && !text.empty()){
                    contents.append(" ");
                }
                contents.append(text);
            }
            return contents;
        } 
        else {
            return "";
        }
    }

    std::string TermSanitizer::html_text(std::string html){
        GumboOutput* output = gumbo_parse(html.c_str());
        std::string text = TermSanitizer::clean_text(output->root);
        gumbo_destroy_output(&kGumboDefaultOptions, output);
        return text;
    }
}