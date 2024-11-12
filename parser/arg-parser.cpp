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


arg_parser::arg_parser() {}

void arg_parser::parse(const int argc, const wchar_t* argv[])
{
    wstr_vec raw_args;
    for (int i = 1; i < argc; i++)
    {
        std::wcout << argc << "\n";
        std::wcout << argv[i] << "\n";
        raw_args.push_back(argv[i]);
    }
    while (raw_args.size() > 0)
    {
        uint16_t initial_size = raw_args.size();

        try_match_and_set_arg(raw_args, do_list);
        try_match_and_set_arg(raw_args, do_help);
        try_match_and_set_arg(raw_args, do_version);
        try_match_and_set_arg(raw_args, do_test);
        try_match_and_set_arg(raw_args, do_detect);

        if (commands_with_no_args.find(command) != commands_with_no_args.end()) goto standard_arguments;

        if (try_match_and_set_arg(raw_args, record_commandline_separator)) {
            parse_record_commandline(raw_args);
            break;
        }

#pragma region unfinished business
        try_match_and_set_arg(raw_args, do_annotate);
        try_match_and_set_arg(raw_args, do_kernel);
        try_match_and_set_arg(raw_args, sample_display_long);
        try_match_and_set_arg(raw_args, is_quite);
        try_match_and_set_arg(raw_args, do_annotate);
#pragma endregion

    standard_arguments:
        try_match_and_set_arg(raw_args, do_json);
        try_match_and_set_arg(raw_args, do_verbose);
        try_match_and_set_arg(raw_args, do_force_lock);

        if (initial_size == raw_args.size())
        {
            std::wcout << L"Invalid argument: " << raw_args.front() << L"\n";
            // TODO: better error handling needs to be implemented here 
            throw std::invalid_argument(std::string(raw_args.front().begin(), raw_args.front().end()));
            break;
        }
    }
}

#pragma region command line parsing after "--"
void arg_parser::parse_record_commandline(wstr_vec& raw_args_vect)
{
    while (raw_args_vect.size() > 0)
    {
        wstring arg = raw_args_vect.front();

        if (sample_pe_file.empty())
        {
            sample_pe_file = arg;
            record_commandline = arg;
        }
        else
            record_commandline += L" " + arg;

        raw_args_vect.erase(raw_args_vect.begin());
    }
}
#pragma endregion

#pragma region arg_matching and setting
bool arg_parser::try_match_and_set_arg(wstr_vec& raw_args_vect, flag_type& flag)
{
    if (raw_args_vect.size() == 0)
        return false;

    if (!flag.is_match(raw_args_vect.front()))
        return false;

    flag.value = true;
    raw_args_vect.erase(raw_args_vect.begin());
    return true;
}

bool arg_parser::try_match_and_set_arg(wstr_vec& raw_args_vect, arg_type& flag)
{
    if (raw_args_vect.size() == 0)
        return false;

    if (!flag.is_match(raw_args_vect.front()))
        return false;

    flag.value = true;
    command = flag.command;
    raw_args_vect.erase(raw_args_vect.begin());
    return true;
}
#pragma endregion

void arg_parser::throw_invalid_arg(const std::wstring& arg, const std::wstring& additional_message) const
{
    std::wstring command = L"wperf";
    for (int i = 1; arg_array[i] != nullptr; ++i) {
        if (i > 0) {
            command += L" ";
        }
        command += arg_array[i];
    }

    std::size_t pos = command.find(arg);
    if (pos == std::wstring::npos) {
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
