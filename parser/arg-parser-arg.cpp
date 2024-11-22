#include "arg-parser-arg.h"
#include <stdexcept>

arg_parser_arg::arg_parser_arg(
    const std::wstring name,
    const std::vector<std::wstring> alias,
    const std::wstring description,
    const std::vector<std::wstring> default_values,
    const int arg_count
) : m_name(name), m_aliases(alias), m_description(description), m_arg_count(arg_count), m_values(default_values) {};

inline bool arg_parser_arg::operator==(const std::wstring& other_arg) const
{
    return is_match(other_arg);
}

bool arg_parser_arg::is_match(const std::wstring& other_arg) const
{
    return !other_arg.empty() && (other_arg == m_name || std::find(m_aliases.begin(), m_aliases.end(), other_arg) != m_aliases.end());
}

std::wstring arg_parser_arg::get_help() const
{
    return m_name + L" or " + get_alias_string() + L"/n         " + m_description;
}

std::wstring arg_parser_arg::get_usage_text() const
{
    return m_description;
}

arg_parser_arg arg_parser_arg::add_alias(std::wstring new_alias) 
{
    m_aliases.push_back(new_alias);
    return *this;
}

int arg_parser_arg::get_arg_count() const
{
    return m_arg_count;
}

std::wstring arg_parser_arg::get_name() const
{
    return m_name;
}

std::wstring arg_parser_arg::get_alias_string() const
{
    // convert alias vector to wstring
    std::wstring alias_string;
    for (auto& m_alias : m_aliases) {
        alias_string.append(m_alias);
    }
    return alias_string;
}

arg_parser_arg arg_parser_arg::add_check_func(std::function<bool(const std::wstring&)> check_func)
{
    m_check_funcs.push_back(check_func);
    return *this;
}

void arg_parser_arg::set_is_parsed()
{
    m_is_parsed = true;
}

bool arg_parser_arg::is_parsed()
{
    return m_is_parsed;
}

bool arg_parser_arg::is_set()
{
    return is_parsed() && m_values.size() == m_arg_count;
}

std::vector<std::wstring> arg_parser_arg::get_values()
{
    return m_values;
}

bool arg_parser_arg::parse(std::vector<std::wstring> arg_vect)
{
    if (arg_vect.size() == 0 || !is_match(arg_vect[0]))
        return false;

    if (m_arg_count == -1 && arg_vect.size() > 0) m_arg_count = arg_vect.size() - 1;

    if (arg_vect.size() < m_arg_count + 1)
        throw std::invalid_argument("Not enough arguments provided.");

    if (m_arg_count == 0)
    {
        set_is_parsed();
        return true;
    }
    
    for (int i = 1; i < m_arg_count + 1; ++i)
    {
        for (auto& check_func : m_check_funcs)
        {
            if (!check_func(arg_vect[i]))
                throw std::invalid_argument("Invalid arguments provided.");
        }
        m_values.push_back(arg_vect[i]);
    }
    set_is_parsed();
    return true;
}

