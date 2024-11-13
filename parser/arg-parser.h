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
#include <unordered_map>

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

template <typename T = bool>
struct arg_base_type {
    T value;
    const wstring key;
    const wstring alias;
    inline bool operator==(const wstring& arg) const {
        return is_match(arg);
    }
    bool is_match(const wstring& arg) const {
        return !arg.empty() && (arg == key || arg == alias);
    }
    T get() const {
        return value;
    }
};

template <typename T = bool>
struct flag_type : arg_base_type<T> {
    wstring description;
};

struct arg_type : arg_base_type<bool> {
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
    arg_type do_sample = { false, L"sample", L"", COMMAND_CLASS::SAMPLE };
    arg_type do_record = { false, L"record", L"", COMMAND_CLASS::RECORD };

#pragma endregion

#pragma region Flags
    flag_type<bool> do_json = { false, L"--json", L"", L"Define output type as JSON." };
    flag_type<bool> do_kernel = { false, L"--k", L"", L"Count kernel mode as well (disabled by default)." };
    flag_type<bool> do_force_lock = {
        false,
        L"--force-lock",
        L"",
        LR"(Force driver to give lock to current `wperf` process, use when you want
            to interrupt currently executing `wperf` session or to recover from the lock.)"
    };
    // used to be called sample_display_short
    flag_type<bool> sample_display_long = { false, L"--sample-display-long", L"", L"Display decorated symbol names." };
    flag_type<bool> do_verbose = { false, L"--verbose", L"-v", L"Enable verbose output also in JSON output." };
    flag_type<bool> is_quite = { false, L"-q", L"", L"Quiet mode, no output is produced." };
    flag_type<bool> do_annotate = {
        false,
        L"--annotate",
        L"",
        L"Enable translating addresses taken from samples in sample/record mode into source code line numbers."
    };
    flag_type<bool> do_disassembly = {
        false,
        L"--disassemble",
        L"",
        L"Enable disassemble output on sampling mode. Implies 'annotate'."
    };
    flag_type<bool> record_commandline_separator = {
        false,
        L"--",
        L"",
        L"-- Process name is defined by COMMAND. User can pass verbatim arguments to the process with[ARGS]."
    };

    flag_type<std::vector<uint8_t>> cores_idx = {
        std::vector<uint8_t> {},
        L"-c",
        L"--cores",
        L"Specify comma separated list of CPU cores, and or ranges of CPU cores, to count on, or one CPU to sample on."
    };

    flag_type<double> count_duration = {
        -1.0,
        L"--timeout",
        L"sleep",
        LR"(Specify counting or sampling duration. If not specified, press
            Ctrl+C to interrupt counting or sampling. Input may be suffixed by
            one (or none) of the following units, with up to 2 decimal
            points: "ms", "s", "m", "h", "d" (i.e. milliseconds, seconds,
            minutes, hours, days). If no unit is provided, the default unit
            is seconds. Accuracy is 0.1 sec.)"
    };
    flag_type<std::wstring> symbol_arg = {
        L"",
        L"--symbol",
        L"-s",
        L"Filter results for specific symbols (for use with 'record' and 'sample' commands)."
    };
    flag_type<uint32_t> record_spawn_delay = {
       1000,
       L"--record_spawn_delay",
       L"",
       L"Set the waiting time, in milliseconds, before reading process data after spawning it with `record`."
    };
    flag_type<uint32_t> sample_display_row = {
       50,
       L"--sample-display-row",
       L"",
       L"Set how many samples you want to see in the summary (50 by default)."
    };
    flag_type<std::wstring> sample_pe_file = {
      L"",
      L"--pe_file",
      L"",
      L"Specify the PE filename (and path)."
    };
    flag_type<std::wstring> sample_image_name = {
      L"",
      L"--image_name",
      L"",
      L"Specify the image name you want to sample."
    };
    flag_type<std::wstring> sample_pdb_file = {
     L"",
     L"--pdb_file",
     L"",
     L"Specify the PDB filename (and path), PDB file should directly corresponds to a PE file set with `- - pe_file`."
    };
#pragma endregion

#pragma region Attributes

    COMMAND_CLASS command = COMMAND_CLASS::NO_COMMAND;

#pragma endregion

#pragma region Not implemented yet
    const wchar_t** arg_array;
    bool do_count;
    bool do_timeline;
    bool do_man;
    bool do_export_perf_data;
    bool do_cwd = false;            // Set current working dir for storing output files
    bool report_l3_cache_metric;
    bool report_ddr_bw_metric;
    uint8_t dmc_idx;
    double count_interval;
    int count_timeline;
    std::wstring man_query_args;

    std::wstring record_commandline;        // <sample_pe_file> <arg> <arg> <arg> ...
    std::wstring timeline_output_file;
    std::wstring m_cwd;                     // Current working dir for storing output files
    std::map<uint32_t, uint32_t> sampling_inverval;     //!< [event_index] -> event_sampling_interval
    bool m_sampling_with_spe = false;                   // SPE: User requested sampling with SPE
    std::map<std::wstring, uint64_t> m_sampling_flags;      // SPE: sampling flags
#pragma endregion

#pragma region Private Methods
private:
    void parse_record_commandline(wstr_vec& raw_args_vect);
    void check_sampling_required_args();
    void check_record_required_args();
    void parse_sampling_args(wstr_vec& raw_args_vect);
    void parse_cpu_core(wstr_vec& raw_args_vect, uint8_t MAX_CPU_CORES);
    bool try_match_and_set_arg(wstr_vec& raw_args_vect, flag_type<bool>& flag);
    bool try_match_and_set_arg(wstr_vec& raw_args_vect, arg_type& flag);
    bool check_timeout_arg(std::wstring number_and_suffix, const std::unordered_map<std::wstring, double>& unit_map);
    double convert_timeout_arg_to_seconds(std::wstring number_and_suffix);
    void check_next_arg(const wstr_vec& raw_args_vect) const;
    void check_flag_value(const wstr_vec& raw_args_vect) const;
    void throw_invalid_arg(const std::wstring& arg, const std::wstring& additional_message = L"") const;
#pragma endregion

};