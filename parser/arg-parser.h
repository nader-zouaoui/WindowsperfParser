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

#define ARM_SPE_EVENT_PREFIX L"arm_spe_0"

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
static const std::set<COMMAND_CLASS> commands_with_events_and_metrics = { 
    COMMAND_CLASS::STAT, 
    COMMAND_CLASS::SAMPLE, 
    COMMAND_CLASS::RECORD,
    COMMAND_CLASS::TIMELINE,
    COMMAND_CLASS::SPE 
};

static const std::set<COMMAND_CLASS> commands_with_no_args = {
    COMMAND_CLASS::HELP,
    COMMAND_CLASS::VERSION,
    COMMAND_CLASS::LIST,
    COMMAND_CLASS::DETECT,
    COMMAND_CLASS::TEST,
};

struct arg_base_type {
    const wstring key;
    const wstring alias;
    inline bool operator==(const wstring& arg) const {
      return is_match(arg);
    }
    bool is_match(const wstring& arg) const {
      return !arg.empty() && (arg == key || arg == alias);
    }

};
template <typename T = bool>
struct flag_type : arg_base_type {
    wstring description;
    T value;
    T get() const {
      return value;
    }
};

struct arg_type : flag_type<bool> {
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
    arg_type do_list = {
      L"list",
      L"-l",
      L"",
      false,
      COMMAND_CLASS::LIST
    };
    arg_type do_test = {
      L"test",
      L"",
      L"",
      false,
      COMMAND_CLASS::TEST
    };
    arg_type do_help = {
      L"-h",
      L"--help",
      L"",
      false,
      COMMAND_CLASS::HELP
    };
    arg_type do_version = {
      L"--version",
      L"",
      L"",
      false,
      COMMAND_CLASS::VERSION
    };
    arg_type do_detect = {
      L"detect",
      L"",
      L"",
      false,
      COMMAND_CLASS::DETECT
    };
    arg_type do_sample = {
      L"sample",
      L"",
      L"",
      false,
      COMMAND_CLASS::SAMPLE
    };
    arg_type do_record = {
      L"record",
      L"",
      L"",
      false,
      COMMAND_CLASS::RECORD
    };
    arg_type do_count = {
      L"stat",
      L"",
      L"",
      false,
      COMMAND_CLASS::STAT
    };

#pragma endregion

#pragma region Flags
    flag_type<bool> do_json = {
      L"--json",
      L"",
      L"Define output type as JSON.",
      false,
    };
    flag_type<bool> do_kernel = {
      L"--k",
      L"",
      L"Count kernel mode as well (disabled by default).",
      false,
    };
    flag_type<bool> do_force_lock = {
      L"--force-lock",
      L"",
      LR"(Force driver to give lock to current `wperf` process, use when you want
      to interrupt currently executing `wperf` session or to recover from the lock.)",
      false,
    };
    // used to be called sample_display_short
    flag_type<bool> sample_display_long = {
      L"--sample-display-long",
      L"",
      L"Display decorated symbol names.",
      false,
    };
    flag_type<bool> do_verbose = {
      L"--verbose",
      L"-v",
      L"Enable verbose output also in JSON output.",
      false,
    };
    flag_type<bool> is_quite = {
      L"-q",
      L"",
      L"Quiet mode, no output is produced.",
      false,
    };
    flag_type<bool> do_annotate = {
      L"--annotate",
      L"",
      L"Enable translating addresses taken from samples in sample/record mode into source code line numbers.",
      false,
    };
    flag_type<bool> do_disassembly = {
      L"--disassemble",
      L"",
      L"Enable disassemble output on sampling mode. Implies 'annotate'.",
      false,
    };
    flag_type<wstring> record_commandline = {
      L"--",
      L"",
      L"-- Process name is defined by COMMAND. User can pass verbatim arguments to the process with[ARGS].",
      L"",
    };     // <sample_pe_file> <arg> <arg> <arg> ...

    flag_type<std::vector<uint8_t>> cores_idx = {
      L"-c",
      L"--cores",
      L"Specify comma separated list of CPU cores, and or ranges of CPU cores, to count on, or one CPU to sample on.",
      std::vector<uint8_t> {},
    };

    flag_type<double> count_duration = {
      L"--timeout",
      L"sleep",
      LR"(Specify counting or sampling duration. If not specified, press
      Ctrl+C to interrupt counting or sampling. Input may be suffixed by
      one (or none) of the following units, with up to 2 decimal
      points: "ms", "s", "m", "h", "d" (i.e. milliseconds, seconds,
      minutes, hours, days). If no unit is provided, the default unit
      is seconds. Accuracy is 0.1 sec.)",
      -1.0,
    };
    flag_type<std::wstring> symbol_arg = {
      L"--symbol",
      L"-s",
      L"Filter results for specific symbols (for use with 'record' and 'sample' commands).",
      L"",
    };
    flag_type<uint32_t> record_spawn_delay = {
      L"--record_spawn_delay",
      L"",
      L"Set the waiting time, in milliseconds, before reading process data after spawning it with `record`.",
      1000,
    };
    flag_type<uint32_t> sample_display_row = {
      L"--sample-display-row",
      L"",
      L"Set how many samples you want to see in the summary (50 by default).",
      50,
    };
    flag_type<std::wstring> sample_pe_file = {
      L"--pe_file",
      L"",
      L"Specify the PE filename (and path).",
      L"",
    };
    flag_type<std::wstring> sample_image_name = {
      L"--image_name",
      L"",
      L"Specify the image name you want to sample.",
      L"",
    };
    flag_type<std::wstring> sample_pdb_file = {
      L"--pdb_file",
      L"",
      L"Specify the PDB filename (and path), PDB file should directly corresponds to a PE file set with `- - pe_file`.",
      L"",
    };
    flag_type<std::wstring> events_string = {
        L"-e",
        L"",
        LR"(        Specify comma separated list of event names (or raw events) to count, for
        example `ld_spec,vfp_spec,r10`. Use curly braces to group events.
        Specify comma separated list of event names with sampling frequency to
        sample, for example `ld_spec:100000`.

        Raw events: specify raw evens with `r<VALUE>` where `<VALUE>` is a 16-bit
        hexadecimal event index value without leading `0x`. For example `r10` is
        event with index `0x10`.

        Note: see list of available event names using `list` command.)",
        L"",
    };
#pragma endregion

#pragma region Attributes

    COMMAND_CLASS command = COMMAND_CLASS::NO_COMMAND;
    wstr_vec arg_array;

#pragma endregion

#pragma region Not implemented yet
    bool do_timeline;
    bool do_man;
    bool do_export_perf_data;
    bool do_cwd = false;        // Set current working dir for storing output files
    bool report_l3_cache_metric;
    bool report_ddr_bw_metric;
    uint8_t dmc_idx;
    double count_interval;
    int count_timeline;
    std::wstring man_query_args;

    std::wstring timeline_output_file;
  std::wstring m_cwd;       // Current working dir for storing output files
    std::map<uint32_t, uint32_t> sampling_inverval;     //!< [event_index] -> event_sampling_interval
  bool m_sampling_with_spe = false;       // SPE: User requested sampling with SPE
    std::map<std::wstring, uint64_t> m_sampling_flags;      // SPE: sampling flags
#pragma endregion

#pragma region Protected Methods
protected:
    virtual void check_file_path(wstring file_path);
    void throw_invalid_arg(const std::wstring& arg, const std::wstring& additional_message = L"") const;
#pragma endregion

#pragma region Private Methods
private:
    void parse_record_commandline(wstr_vec& raw_args_vect);
    void check_sampling_required_args();
    void fill_pdb_and_image_name_if_empty();
    void check_record_required_args();
    void parse_sampling_args(wstr_vec& raw_args_vect);
    void parse_event_list(wstring events_string);
    void parse_cpu_core(wstr_vec& raw_args_vect, uint8_t MAX_CPU_CORES);
    bool try_match_and_set_bool_flag(wstr_vec& raw_args_vect, flag_type<bool>& flag);
    bool try_match_and_set_arg(wstr_vec& raw_args_vect, arg_type& flag);
    bool try_match_arg(wstr_vec& raw_args_vect, arg_base_type& flag);
    bool check_timeout_arg(std::wstring number_and_suffix, const std::unordered_map<std::wstring, double>& unit_map);
    double convert_timeout_arg_to_seconds(std::wstring number_and_suffix);
    void check_next_arg(const wstr_vec& raw_args_vect) const;
    void check_flag_value_existance(const wstr_vec& raw_args_vect) const;
#pragma endregion
};