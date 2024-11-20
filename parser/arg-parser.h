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
    NO_COMMAND
};

struct arg_base_type {
    const wstring key;
    const wstring alias;
    wstring description;
    inline bool operator==(const wstring& arg) const {
        return is_match(arg);
    }
    bool is_match(const wstring& arg) const {
        return !arg.empty() && (arg == key || arg == alias);
    }
};

struct flag_bool_type : arg_base_type {
    bool value;
    bool get() const {
        return value;
    }
    bool is_set() const {
        return !value;
    }
};

struct flag_with_argument : arg_base_type {
    wstring value;
    wstring get() const {
        return value;
    }
    bool is_set() const {
        return !value.empty();
    }
};

struct arg_type : flag_bool_type {
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
    arg_type do_man = {
      L"stat",
      L"",
      L"",
      false,
      COMMAND_CLASS::MAN
    };

#pragma endregion

#pragma region Boolean Flags
    flag_bool_type do_json = {
      L"--json",
      L"",
      L"Define output type as JSON.",
      false,
    };
    flag_bool_type do_kernel = {
      L"-k",
      L"",
      L"Count kernel mode as well (disabled by default).",
      false,
    };
    flag_bool_type do_force_lock = {
      L"--force-lock",
      L"",
      LR"(Force driver to give lock to current `wperf` process, use when you want
      to interrupt currently executing `wperf` session or to recover from the lock.)",
      false,
    };
    // used to be called sample_display_short
    flag_bool_type sample_display_long = {
      L"--sample-display-long",
      L"",
      L"Display decorated symbol names.",
      false,
    };
    flag_bool_type do_verbose = {
      L"--verbose",
      L"-v",
      L"Enable verbose output also in JSON output.",
      false,
    };
    flag_bool_type is_quite = {
      L"-q",
      L"",
      L"Quiet mode, no output is produced.",
      false,
    };
    flag_bool_type do_annotate = {
      L"--annotate",
      L"",
      L"Enable translating addresses taken from samples in sample/record mode into source code line numbers.",
      false,
    };
    flag_bool_type do_disassembly = {
      L"--disassemble",
      L"",
      L"Enable disassemble output on sampling mode. Implies 'annotate'.",
      false,
    };
    flag_bool_type do_timeline = {
      L"-t",
      L"",
      L"Enable timeline mode (count multiple times with specified interval). Use `-i` to specify timeline interval, and `-n` to specify number of counts.",
      false,
    };

#pragma endregion

#pragma region Flags with arguments
    flag_with_argument double_dash = {
      L"--",
      L"",
      L"-- Process name is defined by COMMAND. User can pass verbatim arguments to the process with[ARGS].",
      L"",
    };

    flag_with_argument cores_idx = {
      L"-c",
      L"--cores",
      L"Specify comma separated list of CPU cores, and or ranges of CPU cores, to count on, or one CPU to sample on.",
      L"",
    };

    flag_with_argument count_duration = {
      L"--timeout",
      L"sleep",
      LR"(Specify counting or sampling duration. If not specified, press
      Ctrl+C to interrupt counting or sampling. Input may be suffixed by
      one (or none) of the following units, with up to 2 decimal
      points: "ms", "s", "m", "h", "d" (i.e. milliseconds, seconds,
      minutes, hours, days). If no unit is provided, the default unit
      is seconds. Accuracy is 0.1 sec.)",
      L"",
    };
    flag_with_argument symbol_arg = {
      L"--symbol",
      L"-s",
      L"Filter results for specific symbols (for use with 'record' and 'sample' commands).",
      L"",
    };
    flag_with_argument record_spawn_delay = {
      L"--record_spawn_delay",
      L"",
      L"Set the waiting time, in milliseconds, before reading process data after spawning it with `record`.",
      L"",
    };
    flag_with_argument sample_display_row = {
      L"--sample-display-row",
      L"",
      L"Set how many samples you want to see in the summary (50 by default).",
      L"",
    };
    flag_with_argument sample_pe_file = {
      L"--pe_file",
      L"",
      L"Specify the PE filename (and path).",
      L"",
    };
    flag_with_argument sample_image_name = {
      L"--image_name",
      L"",
      L"Specify the image name you want to sample.",
      L"",
    };
    flag_with_argument sample_pdb_file = {
      L"--pdb_file",
      L"",
      L"Specify the PDB filename (and path), PDB file should directly corresponds to a PE file set with `- - pe_file`.",
      L"",
    };
    flag_with_argument metric_config = {
      L"-C",
      L"",
      L"Provide customized config file which describes metrics.",
      L"",
    };
    flag_with_argument event_config = {
      L"-E",
      L"",
      L"Provide customized config file which describes custom events or provide custom events from the command line.",
      L"",
    };
    flag_with_argument output_filename = {
      L"--output",
      L"-o",
      L"Specify JSON output filename.",
      L"",
    };
    flag_with_argument output_csv_filename = {
      L"--output-csv",
      L"",
      L"Specify CSV output filename. Only with timeline `-t`.",
      L"",
    };
    flag_with_argument m_cwd = {
      L"--output-prefix",
      L"--cwd",
      L"Set current working dir for storing output JSON and CSV file.",
      L"",
    };
    flag_with_argument drv_config = {
      L"--config",
      L"",
      L"Specify configuration parameters.",
      L"",
    };
    flag_with_argument count_interval = {
      L"-i",
      L"",
      L"Specify counting interval. `0` seconds is allowed. Input may be suffixed with one(or none) of the following units, with up to 2 decimal points : \"ms\", \"s\", \"m\", \"h\", \"d\" (i.e.milliseconds, seconds, minutes, hours, days).If no unit is provided, the default unit is seconds(60s by default).",
      L"",
    }; 
    flag_with_argument timeline_count = {
      L"-i",
      L"",
      L"Number of consecutive counts in timeline mode (disabled by default).",
      L"",
    };
    flag_with_argument dmc_idx = {
      L"--dmc",
      L"",
      L"Profile on the specified DDR controller. Skip `--dmc` to count on all DMCs.",
      L"",
    };
    flag_with_argument raw_metrics = {
      L"-m",
      L"",
      L"Specify comma separated list of metrics to count.\n\nNote: see list of available metric names using `list` command.",
      L"",
    };
    flag_with_argument raw_events = {
      L"-e",
      L"",
      LR"(Specify comma separated list of event names (or raw events) to count, for
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
    std::vector<arg_type*> commands_list = {
       &do_list,
       &do_test,
       &do_help,
       &do_version,
       &do_detect,
       &do_sample,
       &do_record,
       &do_count,
       &do_man
    };

    std::vector<flag_bool_type*> boolean_flags = {
       &do_json,
       &do_kernel,
       &do_force_lock,
       &sample_display_long,
       &do_verbose,
       &is_quite,
       &do_annotate,
       &do_disassembly,
       &do_timeline
    };

    std::vector<flag_with_argument*> flags_with_argument = {
       &cores_idx,
       &count_duration,
       &symbol_arg,
       &record_spawn_delay,
       &sample_display_row,
       &sample_pe_file,
       &sample_image_name,
       &sample_pdb_file,
       &metric_config,
       &event_config,
       &output_filename,
       &output_csv_filename,
       &m_cwd,
       &drv_config,
       &count_interval,
       &timeline_count,
       &dmc_idx,
       &raw_metrics,
       &raw_events,
    };

    wstr_vec arg_array;

#pragma endregion

#pragma region Protected Methods
protected:
    void throw_invalid_arg(const std::wstring& arg, const std::wstring& additional_message = L"") const;
#pragma endregion

#pragma region Private Methods
private:
    void parse_command_after_double_dash(wstr_vec& raw_args_vect);
    bool try_match_and_set_arg(wstr_vec& raw_args_vect, arg_type& flag);
    bool try_match_and_set_arg(wstr_vec& raw_args_vect, flag_bool_type& flag);
    bool try_match_and_set_arg(wstr_vec& raw_args_vect, flag_with_argument& flag);
    bool try_match_arg(wstr_vec& raw_args_vect, arg_base_type& flag);
    void check_next_arg(const wstr_vec& raw_args_vect) const;
    void check_flag_value_existance(const wstr_vec& raw_args_vect) const;
#pragma endregion
};