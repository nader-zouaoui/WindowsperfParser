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

#pragma once

#include <map>
#include <vector>
#include <deque>
#include <array>
#include <string>
#include <set>

using namespace std;


typedef std::vector<std::wstring> wstr_vec;

#pragma region arg structs
enum COMMAND_CLASS {
    STAT,
    SAMPLE,
    RECORD,
    TEST,
    DETECT,
    HELP,
    VERSION,
    LIST,
    MAN,
    SPE,
    TIMELINE,
    NO_COMMAND
};

const std::set<COMMAND_CLASS> commands_with_no_args = {
    COMMAND_CLASS::HELP,
    COMMAND_CLASS::VERSION,
    COMMAND_CLASS::LIST,
    COMMAND_CLASS::DETECT,
    COMMAND_CLASS::TEST,
};

struct arg_base_type {
    bool value;
    const wstring key;
    const wstring alias;
    bool is_match(const wstring& arg) const {
        return arg == key || arg == alias;
    }
    bool get() const {
        return value;
    }
};

struct flag_type : arg_base_type {
    wstring description;
    bool is_required;
};

struct arg_type : arg_base_type {
    const COMMAND_CLASS command;
};
#pragma endregion

class arg_parser
{
public:
    #pragma region Methods
    arg_parser();
    void parse(
        _In_ const int argc,
        _In_reads_(argc) const wchar_t* argv[]
    );
    #pragma endregion

    #pragma region Commands
    arg_type do_list = { false, L"list", L"-l", COMMAND_CLASS::LIST };
    arg_type do_test = { false, L"test", L"", COMMAND_CLASS::TEST };
    arg_type do_help = { false, L"-h", L"--help", COMMAND_CLASS::HELP };
    arg_type do_version = { false, L"--version", L"", COMMAND_CLASS::VERSION };
    arg_type do_detect = { false, L"detect", L"", COMMAND_CLASS::DETECT };

    #pragma endregion

    #pragma region Flags
    flag_type do_json = { false, L"--json", L"", L"Define output type as JSON.", false };
    flag_type do_kernel = { false, L"--k", L"", L"Count kernel mode as well (disabled by default).", false };
    flag_type do_force_lock = {
        false,
        L"--force-lock",
        L"",
        LR"(Force driver to give lock to current `wperf` process, use when you want
            to interrupt currently executing `wperf` session or to recover from the lock.)",
        false
    };
    // used to be called sample_display_short
    flag_type sample_display_long = { false, L"--sample-display-long", L"", L"Display decorated symbol names.", false };
    flag_type do_verbose = { false, L"--verbose", L"-v", L"Enable verbose output also in JSON output.", false };
    flag_type is_quite = { false, L"-q", L"", L"Quiet mode, no output is produced.", false };
    flag_type do_annotate = {
        false,
        L"--annotate",
        L"",
        L"Enable translating addresses taken from samples in sample/record mode into source code line numbers.",
        false
    };
    flag_type do_disassembly = {
        false,
        L"--disassemble",
        L"",
        L"Enable disassemble output on sampling mode. Implies 'annotate'.",
        false
    };
    flag_type record_commandline_separator = {
        false,
        L"--",
        L"",
        L"-- Process name is defined by COMMAND. User can pass verbatim arguments to the process with[ARGS].",
        false
    };
    #pragma endregion
    
    #pragma region Attributes
    
    COMMAND_CLASS command = COMMAND_CLASS::NO_COMMAND;

    #pragma endregion

    #pragma region Not implemented yet
    bool do_count;
    bool do_timeline;
    bool do_sample;
    bool do_record;
    bool do_man;
    bool do_symbol;
    bool do_export_perf_data;
    bool do_cwd = false;            // Set current working dir for storing output files
    bool report_l3_cache_metric;
    bool report_ddr_bw_metric;
    std::vector<uint8_t> cores_idx;
    uint8_t dmc_idx;
    double count_duration;
    double count_interval;
    int count_timeline;
    uint32_t record_spawn_delay = 1000;
    std::wstring man_query_args;
    std::wstring symbol_arg;
    std::wstring sample_image_name;
    std::wstring sample_pe_file;
    std::wstring sample_pdb_file;
    std::wstring record_commandline;        // <sample_pe_file> <arg> <arg> <arg> ...
    std::wstring timeline_output_file;
    std::wstring m_cwd;                     // Current working dir for storing output files
    uint32_t sample_display_row = 50;
    std::map<uint32_t, uint32_t> sampling_inverval;     //!< [event_index] -> event_sampling_interval
    bool m_sampling_with_spe = false;                   // SPE: User requested sampling with SPE
    std::map<std::wstring, uint64_t> m_sampling_flags;      // SPE: sampling flags
    #pragma endregion

#pragma region Private Methods
private:
    void parse_record_commandline(wstr_vec& raw_args_vect);
    bool try_match_and_set_arg(wstr_vec& raw_args_vect, flag_type& flag);
    bool try_match_and_set_arg(wstr_vec& raw_args_vect, arg_type& flag);
#pragma endregion

};