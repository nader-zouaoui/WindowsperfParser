// BSD 3-Clause License
//
// Copyright (c) 2024, Arm Limited
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "arg-parser.h"
#include <iostream>
#include <codecvt>
#include <locale>
#include <cwchar>
#include <vector>
#include <sstream>

arg_parser::arg_parser() {}

void arg_parser::parse(
    _In_ const int argc,
    _In_reads_(argc) const wchar_t* argv[]
)
{
    wstr_vec raw_args;
    for (int i = 1; i < argc; i++)
    {
        raw_args.push_back(argv[i]);
        arg_array.push_back(argv[i]);
    }

    if (raw_args.size() == 0)
        throw_invalid_arg(L"", L"warning: No commands were found!");

#pragma region Command Selector
    for (auto& command : commands_list)
    {
        try_match_and_set_arg(raw_args, *command);
    }
    if (command == COMMAND_CLASS::NO_COMMAND) {
        throw_invalid_arg(raw_args.front(), L"warning: command not recognized!");
    }
#pragma endregion

    while (raw_args.size() > 0)
    {
        size_t initial_size = raw_args.size();

        // Set boolean flags to tru if they exist
        for (auto& bool_flag : boolean_flags)
        {
            try_match_and_set_arg(raw_args, *bool_flag);
        }

        // Set the string value for the flags that should have arguments
        for (auto& flag_with_argument : flags_with_argument)
        {
            try_match_and_set_arg(raw_args, *flag_with_argument);
        }

        // Parse the extra arguments after the "--" flag 
        if (try_match_arg(raw_args, record_commandline)) {
            raw_args.erase(raw_args.begin());
            parse_record_commandline(raw_args);
            break;
        }

        // if going over all the known arguments doesn't affect the size of the raw_args, then the command is unkown
        if (initial_size == raw_args.size())
        {
            throw_invalid_arg(raw_args.front(), L"Error: Unrecognized command");
        }
    }
}

#pragma region command line parsing after "--"
void arg_parser::parse_record_commandline(wstr_vec& raw_args_vect)
{
    while (raw_args_vect.size() > 0)
    {
        wstring arg = raw_args_vect.front();

        if (sample_pe_file.value.empty())
        {
            sample_pe_file.value = arg;
            record_commandline.value = arg;
        }
        else {
            record_commandline.value += L" " + arg;
        }
        raw_args_vect.erase(raw_args_vect.begin());
    }
    raw_args_vect.clear();
}
#pragma endregion

#pragma region arg_matching and setting

bool arg_parser::try_match_and_set_arg(wstr_vec& raw_args_vect, arg_type& flag)
{
    if (raw_args_vect.size() == 0)
        return false;

    if (!(flag == raw_args_vect.front()))
        return false;

    flag.value = true;
    command = flag.command;
    raw_args_vect.erase(raw_args_vect.begin());
    return true;
}

bool arg_parser::try_match_and_set_arg(wstr_vec& raw_args_vect, flag_bool_type& flag)
{
    if (raw_args_vect.size() == 0)
        return false;

    if (!(flag == raw_args_vect.front()))
        return false;

    flag.value = true;
    raw_args_vect.erase(raw_args_vect.begin());
    return true;
}

bool arg_parser::try_match_and_set_arg(wstr_vec& raw_args_vect, flag_with_argument& flag)
{
    if (!try_match_arg(raw_args_vect, flag)) return false;
    raw_args_vect.erase(raw_args_vect.begin());
    check_flag_value_existance(raw_args_vect);

    flag.value = raw_args_vect.front();
    raw_args_vect.erase(raw_args_vect.begin());
    return true;
}

bool arg_parser::try_match_arg(wstr_vec& raw_args_vect, arg_base_type& flag)
{
    if (raw_args_vect.size() == 0)
        return false;

    if (!(flag == raw_args_vect.front()))
        return false;

    return true;
}
#pragma endregion

#pragma region error handling
void arg_parser::check_flag_value_existance(const wstr_vec& raw_args_vect) const
{
    check_next_arg(raw_args_vect);
    if (raw_args_vect.front().find(L"-") == 0)
    {
        throw_invalid_arg(raw_args_vect.front(), L"Hint: Missing value for argument!");
    }
}

void arg_parser::check_next_arg(const wstr_vec& raw_args_vect) const
{
    if (raw_args_vect.size() == 0)
    {
        throw_invalid_arg(L"");
    }
}

void arg_parser::throw_invalid_arg(const std::wstring& arg, const std::wstring& additional_message) const
{
    std::wstring command = L"wperf";
    for (int i = 1; i < arg_array.size(); ++i) {
        if (i > 0) {
            command.append(L" ");
        }
        command.append(arg_array.at(i));
    }

    std::size_t pos = command.find(arg);
    if (pos == 0 || pos == std::wstring::npos) {
        pos = command.length();
    }

    std::wstring indicator(pos, L'~');
    indicator += L'^';

    /*
    TODO: THIS function should change to use GetErrorOutputStream before migrating to wperf

     */

    std::wostringstream error_message;
    error_message << L"Invalid argument detected:\n"
        << command << L"\n"
        << indicator << L"\n";
    if (!additional_message.empty()) {
        error_message << additional_message << L"\n";
    }
    std::wcerr << error_message.str();
    throw std::invalid_argument("INVALID_ARGUMENT");
}

#pragma endregion
