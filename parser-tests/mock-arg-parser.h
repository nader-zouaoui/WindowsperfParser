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

protected:
    void check_file_path(std::wstring file_path) override
    {
        if (existing_files.find(file_path) == existing_files.end())
        {
            // Simulate file does not exist
            throw_invalid_arg(file_path, L"Mock: File does not exist.");
        }
        // Else, do nothing (file exists)
    }
};
