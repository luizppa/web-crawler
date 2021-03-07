#ifndef JSON_H
#define JSON_H

#include<string>
#include<sstream>

namespace search_engine {

    namespace json {

        std::pair<std::string, std::string> parse(std::string json_str);
        std::pair<std::string, std::string> nlohmann_parse(std::string json_str);
        std::pair<std::string, std::string> rapidjson_parse(std::string json_str);

    }

}

#endif