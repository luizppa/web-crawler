#include"../include/json.hpp"
#include"../third_party/nlohmann/json.hpp"
#include"../third_party/rapidjson/document.h"

namespace search_engine {

    namespace json {

        char escape_sequence(char sequence){
            switch (sequence) {
                case 'n':
                case 't':
                case 'r':
                case 's': return ' ';
                default: return sequence;
            }
        }

        void jump(std::stringstream& json_stream, char token){
            char c;
            while(json_stream >> c){
                if(c == token) break;
            }
        }

        std::string get_next_string(std::stringstream& json_stream){
            char c;
            std::stringstream output_stream;

            json_stream >> c;
            while(c != '"'){
                if(c == '\\'){
                    json_stream >> c;
                    output_stream << escape_sequence(c);
                }
                else if(c == '\n'){
                    output_stream << ' ';
                }
                else {
                    output_stream << c;
                }
                json_stream >> c;
            }
            return output_stream.str();
        }

        /**
         * @brief Parse the JSON formated document entry from the collection file into pair(url, body).
         * Efficiency: 👎
         * @param json_str string with the json formated content to parse
         * @return std::pair<std::string, std::string> containing the document url and body respectively
         */
        std::pair<std::string, std::string> parse(std::string json_str){
            std::stringstream json_stream(json_str);
            std::string url;
            std::string body;

            jump(json_stream, '"');
            jump(json_stream, '"');
            jump(json_stream, '"');
            url = get_next_string(json_stream);
            jump(json_stream, '"');
            jump(json_stream, '"');
            jump(json_stream, '"');
            body = get_next_string(json_stream);

            return std::pair<std::string, std::string>(url, body);
        }

        /**
         * @brief Parse the JSON formated document entry from the collection file into pair(url, body).
         * Efficiency: 🤏
         * @param json_str string with the json formated content to parse
         * @return std::pair<std::string, std::string> containing the document url and body respectively
         */
        std::pair<std::string, std::string> nlohmann_parse(std::string json_str){
            nlohmann::json json_obj = nlohmann::json::parse(json_str);
            std::string url = json_obj["url"];
            std::string body = json_obj["html_content"];
            return std::pair<std::string, std::string>(url, body);
        }

        /**
         * @brief Parse the JSON formated document entry from the collection file into pair(url, body).
         * Efficiency: 👏
         * @param json_str string with the json formated content to parse
         * @return std::pair<std::string, std::string> containing the document url and body respectively
         */
        std::pair<std::string, std::string> rapidjson_parse(std::string json_str){
            rapidjson::Document json_obj;
            json_obj.Parse(json_str.c_str());
            std::string url = json_obj["url"].GetString();
            std::string body = json_obj["html_content"].GetString();
            return std::pair<std::string, std::string>(url, body);
        }

    }

}