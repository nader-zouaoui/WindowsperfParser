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


arg_parser::arg_parser(const int argc, const wchar_t* argv[])
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
        wstring arg = raw_args.front();

        if (try_match_and_set_arg(raw_args, do_verbose)) continue;
        if (try_match_and_set_arg(raw_args, do_json)) continue;

        if (try_match_and_set_arg(raw_args, record_commandline_separator)) {
            parse_record_commandline(raw_args);
            break;
        }

        if (try_match_and_set_arg(raw_args, do_annotate)) continue;
        if (try_match_and_set_arg(raw_args, do_kernel)) continue;
        if (try_match_and_set_arg(raw_args, do_force_lock)) continue;
        if (try_match_and_set_arg(raw_args, sample_display_long)) continue;
        if (try_match_and_set_arg(raw_args, is_quite)) continue;
        if (try_match_and_set_arg(raw_args, do_annotate)) continue;
        if (initial_size == raw_args.size())
        {
            std::wcout << L"Unknown argument: " << raw_args[0] << L"\n";

            // TODO: handle unkown argument here
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
    raw_args_vect.erase(raw_args_vect.begin());
    return true;
}
#pragma endregion

