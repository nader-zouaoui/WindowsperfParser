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
#include "utils.h"
#include <iostream>
#include <codecvt>
#include <locale>
#include <cwchar>

arg_parser::arg_parser() {}

void arg_parser::parse(
    _In_ const int argc,
    _In_reads_(argc) const wchar_t* argv[]
)
{
    arg_array = argv;
    wstr_vec raw_args;
    for (int i = 1; i < argc; i++)
    {
        raw_args.push_back(argv[i]);
    }
    while (raw_args.size() > 0)
    {
        size_t initial_size = raw_args.size();
        if (command == COMMAND_CLASS::NO_COMMAND) {
            try_match_and_set_arg(raw_args, do_list);
            try_match_and_set_arg(raw_args, do_help);
            try_match_and_set_arg(raw_args, do_version);
            try_match_and_set_arg(raw_args, do_test);
            try_match_and_set_arg(raw_args, do_detect);
            try_match_and_set_arg(raw_args, do_sample);
            try_match_and_set_arg(raw_args, do_record);
        }

        if (command == COMMAND_CLASS::SAMPLE || command == COMMAND_CLASS::RECORD)
            parse_sampling_args(raw_args);

        if (commands_with_no_args.find(command) != commands_with_no_args.end()) goto standard_arguments;

        if (try_match_and_set_arg(raw_args, record_commandline_separator)) {
            if (command != COMMAND_CLASS::RECORD && command != COMMAND_CLASS::STAT && command != COMMAND_CLASS::TIMELINE && command != COMMAND_CLASS::SPE)
                throw_invalid_arg(raw_args.front(), L"warning: only `stat` and `record` support process spawn!");

            parse_record_commandline(raw_args);
            break;
        }

    standard_arguments:
        try_match_and_set_arg(raw_args, do_json);
        try_match_and_set_arg(raw_args, do_verbose);
        try_match_and_set_arg(raw_args, do_force_lock);

        if (initial_size == raw_args.size())
        {
            throw_invalid_arg(raw_args.front());
        }
    }
#pragma region Check for required flags per command
    switch (command)
    {
    case STAT:
        break;
    case SAMPLE:
        check_sampling_required_args();
        break;
    case RECORD:
        check_record_required_args();
        break;
    case TEST:
        break;
    case DETECT:
        break;
    case HELP:
        break;
    case VERSION:
        break;
    case LIST:
        break;
    case MAN:
        break;
    case SPE:
        break;
    case TIMELINE:
        break;
    case NO_COMMAND:
        break;
    default:
        break;
    }

#pragma endregion

}

#pragma region Sampling section parsing

void arg_parser::parse_sampling_args(wstr_vec& raw_args_vect)
{
    while (raw_args_vect.size() > 0)
    {
        size_t initial_size = raw_args_vect.size();
        try_match_and_set_arg(raw_args_vect, do_annotate);
        try_match_and_set_arg(raw_args_vect, do_kernel);
        try_match_and_set_arg(raw_args_vect, sample_display_long);
        try_match_and_set_arg(raw_args_vect, is_quite);
        try_match_and_set_arg(raw_args_vect, do_disassembly);
        if (cores_idx == raw_args_vect.front()) {
            raw_args_vect.erase(raw_args_vect.begin());
            check_flag_value_existance(raw_args_vect);

            parse_cpu_core(raw_args_vect, 1);
        }
        if (count_duration == raw_args_vect.front()) {
            raw_args_vect.erase(raw_args_vect.begin());
            check_flag_value_existance(raw_args_vect);

            count_duration.value = convert_timeout_arg_to_seconds(raw_args_vect.front());
            raw_args_vect.erase(raw_args_vect.begin());
        }
        if (symbol_arg == raw_args_vect.front()) {
            raw_args_vect.erase(raw_args_vect.begin());
            check_flag_value_existance(raw_args_vect);

            symbol_arg.value = raw_args_vect.front();
            raw_args_vect.erase(raw_args_vect.begin());
        }
        if (record_spawn_delay == raw_args_vect.front()) {
            raw_args_vect.erase(raw_args_vect.begin());
            check_flag_value_existance(raw_args_vect);

            record_spawn_delay.value = convert_timeout_arg_to_seconds(raw_args_vect.front());
            raw_args_vect.erase(raw_args_vect.begin());
        }
        if (sample_display_row == raw_args_vect.front()) {
            raw_args_vect.erase(raw_args_vect.begin());
            check_flag_value_existance(raw_args_vect);
            try
            {
                sample_display_row.value = _wtoi(raw_args_vect.front().c_str());
                raw_args_vect.erase(raw_args_vect.begin());
            }
            catch (const std::exception&)
            {
                throw_invalid_arg(raw_args_vect.front());
            }
        }
        if (sample_pe_file == raw_args_vect.front()) {
            raw_args_vect.erase(raw_args_vect.begin());
            check_flag_value_existance(raw_args_vect);
            check_file_path(raw_args_vect.front());
        }
        if (sample_pdb_file == raw_args_vect.front()) {
            raw_args_vect.erase(raw_args_vect.begin());
            check_flag_value_existance(raw_args_vect);
            check_file_path(raw_args_vect.front());

            sample_pdb_file.value = raw_args_vect.front();
            raw_args_vect.erase(raw_args_vect.begin());
        }
        if (sample_image_name == raw_args_vect.front()) {
            raw_args_vect.erase(raw_args_vect.begin());
            check_flag_value_existance(raw_args_vect);

            sample_image_name.value = raw_args_vect.front();
            raw_args_vect.erase(raw_args_vect.begin());
        }

        if (initial_size == raw_args_vect.size()) break;
    }
}

void arg_parser::parse_cpu_core(wstr_vec& raw_args_vect, uint8_t MAX_CPU_CORES)
{
    wstring cores = raw_args_vect.front();
    if (TokenizeWideStringOfInts(cores.c_str(), L',', cores_idx.value) == false)
    {
        throw_invalid_arg(raw_args_vect.front());
    }
    if (cores_idx.value.size() > MAX_CPU_CORES)
    {
        std::wostringstream error_message;
        error_message << L"Maximum number of cores allowed is " << MAX_CPU_CORES;
        throw_invalid_arg(raw_args_vect.front(), error_message.str());
    }
    raw_args_vect.erase(raw_args_vect.begin());
}


void arg_parser::check_sampling_required_args() {
    // TODO: implement this function
}

void arg_parser::check_record_required_args() {
    // TODO: implement this function
}
#pragma endregion


#pragma region command line parsing after "--"
void arg_parser::parse_record_commandline(wstr_vec& raw_args_vect)
{
    while (raw_args_vect.size() > 0)
    {
        wstring arg = raw_args_vect.front();

        if (sample_pe_file.value.empty())
        {
            sample_pe_file.value = arg;
            record_commandline = arg;
        }
        else
            record_commandline += L" " + arg;

        raw_args_vect.erase(raw_args_vect.begin());
    }
}
#pragma endregion

#pragma region arg_matching and setting
bool arg_parser::try_match_and_set_arg(wstr_vec& raw_args_vect, flag_type<bool>& flag)
{
    if (raw_args_vect.size() == 0)
        return false;

    if (!(flag == raw_args_vect.front()))
        return false;

    flag.value = true;
    raw_args_vect.erase(raw_args_vect.begin());
    return true;
}

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
#pragma endregion

#pragma region Utils
bool arg_parser::check_timeout_arg(std::wstring number_and_suffix, const std::unordered_map<std::wstring, double>& unit_map)
{
    std::wstring accept_units;

    for (const auto& pair : unit_map)
    {
        if (!accept_units.empty()) {
            accept_units += L"|";
        }
        accept_units += pair.first;
    }

    std::wstring regex_string = L"^(0|([1-9][0-9]*))(\\.[0-9]{1,2})?(" + accept_units + L")?$";
    std::wregex r(regex_string);

    std::wsmatch match;
    if (std::regex_search(number_and_suffix, match, r)) {
        return true;
    }
    else {
        return false;
    }
}

double arg_parser::convert_timeout_arg_to_seconds(std::wstring number_and_suffix)
{
    std::unordered_map<std::wstring, double> unit_map = { {L"s", 1}, { L"m", 60 }, {L"ms", 0.001}, {L"h", 3600}, {L"d" , 86400} };

    if (!check_timeout_arg(number_and_suffix, unit_map)) {
        throw_invalid_arg(number_and_suffix);
    }
    //logic to split number and suffix
    int i = 0;
    for (; i < number_and_suffix.size(); i++)
    {
        if (!std::isdigit(number_and_suffix[i]) && (number_and_suffix[i] != L'.')) {
            break;
        }
    }

    std::wstring number_wstring = number_and_suffix.substr(0, i);

    double number;
    try {
        number = std::stod(number_wstring);
    }
    catch (...) {
        throw_invalid_arg(number_and_suffix);
    }

    std::wstring suffix = number_and_suffix.substr(i);

    //default to seconds if unit is not provided
    if (suffix.empty()) {
        return number;
    }

    //check if the unit exists in the map
    auto it = unit_map.find(suffix);
    if (it == unit_map.end())
    {
        throw_invalid_arg(number_and_suffix);

    }
    //Note: This exception should never be reached, as it should be caught in the regex construction of check_timeout_arg
    //However, if the unit map/regex construction is changeed in the future, this serves as a good safety net
    return ConvertNumberWithUnit(number, suffix, unit_map);
}

void arg_parser::check_file_path(wstring file_path)
{
    if (std::filesystem::exists(file_path) == false)
    {
        std::wostringstream error_message;
        error_message << L"File path '" << file_path << L"' doesn't exist";
        throw_invalid_arg(file_path, error_message.str());
    }
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
    for (int i = 1; arg_array[i] != nullptr; ++i) {
        if (i > 0) {
            command += L" ";
        }
        command += arg_array[i];
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
