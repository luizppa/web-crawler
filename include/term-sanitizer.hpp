#ifndef TERM_SANITIZER_H
#define TERM_SANITIZER_H

#include<string>
#include<utility>

#define EXCLUDE_INTERVALS_SIZE 6
#define MAPINGS_SIZE 26
#define SHIFT_INTERVALS_SIZE 3

namespace web_crawler {

    class TermSanitizer {
        private:
            static constexpr std::pair<int, int> exclude_intervals[EXCLUDE_INTERVALS_SIZE] = {
                std::pair<int, int>(0x0000, 0x002F),
                std::pair<int, int>(0x003A, 0x0040),
                std::pair<int, int>(0x005B, 0x0060),
                std::pair<int, int>(0x007B, 0x00BF),
                std::pair<int, int>(0x02B9, 0x036F),
                std::pair<int, int>(0x017F, 0x10FFFF)
            };
            static constexpr std::pair<std::pair<int, int>, char> mapings[MAPINGS_SIZE] = {
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00E0, 0x00E6), 'a'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00E8, 0x00EB), 'e'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00EC, 0x00EF), 'i'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00F2, 0x00F6), 'o'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00F9, 0x00FC), 'u'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00F1, 0x00F1), 'n'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00FD, 0x00FF), 'y'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x00E7, 0x00E7), 'c'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x0106, 0x010D), 'c'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x010E, 0x0111), 'c'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x0112, 0x011B), 'e'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x011C, 0x0123), 'g'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x0124, 0x0127), 'h'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x0128, 0x0133), 'i'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x0134, 0x0135), 'j'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x0136, 0x0138), 'k'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x0139, 0x0142), 'l'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x0143, 0x014B), 'n'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x014C, 0x0153), 'o'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x0154, 0x0159), 'r'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x015A, 0x0161), 's'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x0162, 0x0167), 't'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x0168, 0x0173), 'u'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x0174, 0x0175), 'w'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x0176, 0x0178), 'y'),
                std::pair<std::pair<int, int>, char>(std::pair<int, int>(0x0179, 0x017E), 'z')
            };
            static constexpr std::pair<std::pair<int, int>, int> shift_intervals[SHIFT_INTERVALS_SIZE] = {
                std::pair<std::pair<int, int>, int>(std::pair<int, int>(0x0041, 0x005A), 32),
                std::pair<std::pair<int, int>, int>(std::pair<int, int>(0x00C0, 0x00DD), 32),
                std::pair<std::pair<int, int>, int>(std::pair<int, int>(0x0100, 0x0105), -32)
            };
            static bool char_is_in(wchar_t c, const std::pair<int, int> intervals[], int n);
            static bool char_is_in(wchar_t c, const std::pair<int, int> interval);
            static wchar_t shift(wchar_t c, const std::pair<std::pair<int, int>, int> intervals[], int n);
            static char map(wchar_t c, const std::pair<std::pair<int, int>, char> intervals[], int n);
            static std::wstring to_wstring(std::string str);

        public:
            static std::string sanitize(std::string term);
            static std::string replace_all(std::string string, char a, std::string b);
    };
}

#endif