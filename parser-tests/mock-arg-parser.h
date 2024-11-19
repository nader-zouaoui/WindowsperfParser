#pragma once
#include "parser/arg-parser.h"
#include <unordered_set>
#include <string>

class mock_arg_parser : public arg_parser
{
public:
    // Map to simulate existing files
    std::unordered_set<std::wstring> existing_files = {
        L"C:\\Program\\sample.exe",
        L"C:\\Program\\sample.pdb"
    };
};
