#include<CkSpider.h>
#include<string>
#include<map>
#include<iostream>

#include"../include/record.hpp"

namespace search_engine {
    Record::Record(std::string domain){
        this->domain = domain;
    }

    int Record::get_visited_pages(){
        return this->visited_pages;
    }

    int Record::get_total_time(){
        return this->total_time;
    }

    bool Record::is_currently_visiting(){
        return this->currently_visiting;
    }

    bool Record::has_visited(std::string url){
        return this->urls.find(url) != this->urls.end();
    }

    bool Record::can_start_visiting(std::string url){
        return !this->currently_visiting && !this->has_visited(url) && this->valid(url);
    }

    bool Record::valid(std::string url){
        CkSpider spider;
        try{
            std::string domain = std::string(spider.getUrlDomain(url.c_str()));
            if(this->domain == domain){
                return true;
            }
            else{
                return false;
            }
        }
        catch(...){
            return false;
        }
    }

    void Record::start_visiting(std::string url){
        this->currently_visiting = true;
        this->urls[url] = 0;
    }

    void Record::visit_page(std::string url, int time){
        this->visited_pages++;
        this->total_time += time;
        this->urls[url] += 1;
    }

    void Record::finish(){
        this->currently_visiting = false;
    }
}