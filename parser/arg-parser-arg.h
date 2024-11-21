#pragma once
#include <string>
#include <vector>
#include <set>
#include <functional>

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
        const int arg_count
    );
    bool is_match(const std::wstring& arg) const;
    virtual std::wstring get_help() const;
    virtual std::wstring get_usage_text() const;
    arg_parser_arg add_alias(std::wstring new_alias);
    int get_arg_count() const;
    std::wstring get_name() const;
    std::wstring get_alias_string() const;
    arg_parser_arg add_check_func(std::function<bool(const std::wstring&)> check_func);
    void set_is_parsed();
    bool parse(std::vector<std::wstring> arg_vect);
};

class arg_parser_arg_opt : arg_parser_arg {
    arg_parser_arg_opt(
        const std::wstring name,
        const std::vector<std::wstring> alias,
        const std::wstring description,
        const int arg_count
    ) : arg_parser_arg(name, alias, description, 0) {};
};

class arg_parser_arg_pos : arg_parser_arg {
    arg_parser_arg_pos(
        const std::wstring name,
        const std::vector<std::wstring> alias,
        const std::wstring description,
        const int arg_count
    ) : arg_parser_arg(name, alias, description, arg_count) {};
};

class arg_parser_arg_command : arg_parser_arg {
    arg_parser_arg_command(
        const std::wstring name,
        const std::vector<std::wstring> alias,
        const std::wstring description,
        const int arg_count,
        const std::wstring examples
    ) : arg_parser_arg(name, alias, description, 1) {};
    const std::wstring m_examples;
};