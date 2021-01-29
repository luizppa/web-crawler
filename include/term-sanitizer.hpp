#ifndef TERM_SANITIZER_H
#define TERM_SANITIZER_H

#include<string>
#include<utility>

#define EXCLUDE_INTERVALS_SIZE 3
#define MAPINGS_SIZE 8

namespace web_crawler {

    class TermSanitizer {
        private:
            static constexpr std::pair<int, int> exclude_intervals[EXCLUDE_INTERVALS_SIZE] = {
                std::pair<int, int>(33, 47),
                std::pair<int, int>(58, 96),
                std::pair<int, int>(123, 126)
            };
            static constexpr std::pair<std::pair<int, int>, char> mapings[MAPINGS_SIZE] = {
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00E0, 0x00E6), 'a'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00E8, 0x00EB), 'e'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00EC, 0x00EF), 'i'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00F2, 0x00F6), 'o'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00F9, 0x00FC), 'u'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00F1, 0x00F1), 'n'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00FD, 0x00FF), 'y'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00E7, 0x00E7), 'c')
            };
            static bool char_is_in(wchar_t c, const std::pair<int, int> intervals[], int n);
            static bool char_is_in(wchar_t c, const std::pair<int, int> interval);
            static char map(wchar_t c, const std::pair<std::pair<int, int>, char> intervals[], int n);
            static std::wstring to_wstring(std::string str);

        public:
            static std::string sanitize(std::string term);
    };
}

#endif