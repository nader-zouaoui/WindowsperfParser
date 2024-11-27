#pragma once
#include <string>
#include <vector>
#include <set>
#include <functional>
#include <sstream>
#include <string>

namespace ArgParserArg {
    struct arg_parser_arg {
        const std::wstring m_name;
        std::vector<std::wstring> m_aliases{};
        const std::wstring m_description;

        int m_arg_count; // -1 for variable number of arguments
        bool m_is_parsed = false;
        std::vector<std::wstring> m_values{};

        std::vector <std::function<bool(const std::wstring&)>> m_check_funcs = {};

        inline bool operator==(const std::wstring& other) const;

    public:
        arg_parser_arg(
            const std::wstring name,
            const std::vector<std::wstring> alias,
            const std::wstring description,
            const std::vector<std::wstring> default_values = {},
            const int arg_count = 0
        );
        bool is_match(const std::wstring& arg) const;
        virtual std::wstring get_help() const;
        virtual std::wstring get_all_flags_string() const;
        virtual std::wstring get_usage_text() const;
        arg_parser_arg add_alias(std::wstring new_alias);
        int get_arg_count() const;
        std::wstring get_name() const;
        std::wstring get_alias_string() const;
        arg_parser_arg add_check_func(std::function<bool(const std::wstring&)> check_func);
        void set_is_parsed();
        bool is_parsed();
        bool is_set();
        std::vector<std::wstring> get_values();
        bool parse(std::vector<std::wstring> arg_vect);
    };

    class arg_parser_arg_opt : public arg_parser_arg {
    public:
        arg_parser_arg_opt(
            const std::wstring name,
            const std::vector<std::wstring> alias,
            const std::wstring description,
            const std::vector<std::wstring> default_values = {}
        ) : arg_parser_arg(name, alias, description, default_values, 0) {};
    };

    class arg_parser_arg_pos : public arg_parser_arg {
    public:
        arg_parser_arg_pos(
            const std::wstring name,
            const std::vector<std::wstring> alias,
            const std::wstring description,
            const std::vector<std::wstring> default_values = {},
            const int arg_count = 1
        ) : arg_parser_arg(name, alias, description, default_values, arg_count) {};
    };

    constexpr auto MAX_HELP_WIDTH = 80;
    std::wstring arg_parser_add_wstring_behind_multiline_text(const std::wstring& str, const std::wstring& prefix);
    std::wstring arg_parser_format_string_to_length(const std::wstring& str, size_t max_width = MAX_HELP_WIDTH);
}
