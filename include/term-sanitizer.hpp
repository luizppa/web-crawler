#ifndef TERM_SANITIZER_H
#define TERM_SANITIZER_H

#include<string>
#include<utility>

#define EXCLUDE_INTERVALS_SIZE 3

namespace web_crawler {

    class TermSanitizer {
        private:
            static constexpr std::pair<int, int> exclude_intervals[EXCLUDE_INTERVALS_SIZE] = {
                std::pair<int, int>(33, 47),
                std::pair<int, int>(58, 96),
                std::pair<int, int>(123, 126)
            };
            static bool char_is_in(char c, const std::pair<int, int> intervals[], int n);

        public:
            static std::string sanitize(std::string term);
    };
}

#endif