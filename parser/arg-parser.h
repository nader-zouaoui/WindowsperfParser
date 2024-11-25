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
#include "arg-parser-arg.h"

using namespace std;


typedef std::vector<std::wstring> wstr_vec;

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

class arg_parser_arg_command : public arg_parser_arg_opt {
public:
    arg_parser_arg_command(
        const std::wstring name,
        const std::vector<std::wstring> alias,
        const std::wstring description,
        const std::wstring examples,
        const COMMAND_CLASS command
    ) : arg_parser_arg_opt(name, alias, description), m_examples(examples), m_command(command) {};
    const COMMAND_CLASS m_command = NO_COMMAND;
    const std::wstring m_examples;

};
#pragma region arg structs


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
    arg_parser_arg_command list_command = arg_parser_arg_command::arg_parser_arg_command(
        L"list",
        { L"-l" },
        L"List available metrics and events.",
        L"",
        COMMAND_CLASS::LIST
    );

    arg_parser_arg_command test_command = arg_parser_arg_command::arg_parser_arg_command(
        L"test",
        { L"" },
        L"",
        L"",
        COMMAND_CLASS::TEST
    );
    arg_parser_arg_command help_command = arg_parser_arg_command::arg_parser_arg_command(
        L"-h",
        { L"--help" },
        L"",
        L"",
        COMMAND_CLASS::HELP
    );
    arg_parser_arg_command version_command = arg_parser_arg_command::arg_parser_arg_command(
        L"--version",
        { L"" },
        L"",
        L"",
        COMMAND_CLASS::VERSION
    );
    arg_parser_arg_command detect_command = arg_parser_arg_command::arg_parser_arg_command(
        L"detect",
        { L"" },
        L"",
        L"",
        COMMAND_CLASS::DETECT
    );
    arg_parser_arg_command sample_command = arg_parser_arg_command::arg_parser_arg_command(
        L"sample",
        { L"" },
        L"",
        L"",
        COMMAND_CLASS::SAMPLE
    );
    arg_parser_arg_command record_command = arg_parser_arg_command::arg_parser_arg_command(
        L"record",
        { L"" },
        L"",
        L"",
        COMMAND_CLASS::RECORD
    );
    arg_parser_arg_command count_command = arg_parser_arg_command::arg_parser_arg_command(
        L"stat",
        { L"" },
        L"",
        L"",
        COMMAND_CLASS::STAT
    );
    arg_parser_arg_command man_command = arg_parser_arg_command::arg_parser_arg_command(
        L"stat",
        { L"" },
        L"",
        L"",
        COMMAND_CLASS::MAN
    );

#pragma endregion

#pragma region Boolean Flags
    arg_parser_arg_opt json_opt = arg_parser_arg_opt::arg_parser_arg_opt(
        L"--json",
        {},
        L"Define output type as JSON.",
        {}
    );
    arg_parser_arg_opt kernel_opt = arg_parser_arg_opt::arg_parser_arg_opt(
        L"-k",
        {},
        L"Count kernel mode as well (disabled by default).",
        {}
    );
    arg_parser_arg_opt force_lock_opt = arg_parser_arg_opt::arg_parser_arg_opt(
        L"--force-lock",
        {},
        L"Force driver to give lock to current `wperf` process, use when you want to interrupt currently executing `wperf` session or to recover from the lock.",
        {}
    );
    // used to be called sample_display_short
    arg_parser_arg_opt sample_display_long_opt = arg_parser_arg_opt::arg_parser_arg_opt(
        L"--sample-display-long",
        {},
        L"Display decorated symbol names.",
        {}
    );
    arg_parser_arg_opt verbose_opt = arg_parser_arg_opt::arg_parser_arg_opt(
        L"--verbose",
        { L"-v" },
        L"Enable verbose output also in JSON output.",
        {}
    );
    arg_parser_arg_opt quite_opt = arg_parser_arg_opt::arg_parser_arg_opt(
        L"-q",
        {},
        L"Quiet mode, no output is produced.",
        {}
    );
    arg_parser_arg_opt annotate_opt = arg_parser_arg_opt::arg_parser_arg_opt(
        L"--annotate",
        {},
        L"Enable translating addresses taken from samples in sample/record mode into source code line numbers.",
        {}
    );
    arg_parser_arg_opt disassembly_opt = arg_parser_arg_opt::arg_parser_arg_opt(
        L"--disassemble",
        {},
        L"Enable disassemble output on sampling mode. Implies 'annotate'.",
        {}
    );
    arg_parser_arg_opt timeline_opt = arg_parser_arg_opt::arg_parser_arg_opt(
        L"-t",
        {},
        L"Enable timeline mode (count multiple times with specified interval). Use `-i` to specify timeline interval, and `-n` to specify number of counts.",
        {}
    );


#pragma endregion

#pragma region Flags with arguments
    arg_parser_arg_pos extra_args_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"--",
        {},
        L"-- Process name is defined by COMMAND. User can pass verbatim arguments to the process with[ARGS].",
        {},
        -1
    );

    arg_parser_arg_pos cores_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"-c",
        { L"--cores" },
        L"Specify comma separated list of CPU cores, and or ranges of CPU cores, to count on, or one CPU to sample on.",
        {}
    );

    arg_parser_arg_pos timeout_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"--timeout",
        { L"sleep" },
        L"Specify counting or sampling duration. If not specified, press Ctrl+C to interrupt counting or sampling. Input may be suffixed by one (or none) of the following units, with up to 2 decimal points: \"ms\", \"s\", \"m\", \"h\", \"d\" (i.e. milliseconds, seconds, minutes, hours, days). If no unit is provided, the default unit is seconds. Accuracy is 0.1 sec.",
        {}
    );
    arg_parser_arg_pos symbol_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"--symbol",
        { L"-s" },
        L"Filter results for specific symbols (for use with 'record' and 'sample' commands).",
        {}
    );
    arg_parser_arg_pos record_spawn_delay_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"--record_spawn_delay",
        {},
        L"Set the waiting time, in milliseconds, before reading process data after spawning it with `record`.",
        {}
    );
    arg_parser_arg_pos sample_display_row_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"--sample-display-row",
        {},
        L"Set how many samples you want to see in the summary (50 by default).",
        { L"50" }
    );
    arg_parser_arg_pos pe_file_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"--pe_file",
        {},
        L"Specify the PE filename (and path).",
        {}
    );
    arg_parser_arg_pos image_name_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"--image_name",
        {},
        L"Specify the image name you want to sample.",
        {}
    );
    arg_parser_arg_pos pdb_file_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"--pdb_file",
        {},
        L"Specify the PDB filename (and path), PDB file should directly corresponds to a PE file set with `--pe_file`.",
        {}
    );
    arg_parser_arg_pos metric_config_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"-C",
        {},
        L"Provide customized config file which describes metrics.",
        {}
    );
    arg_parser_arg_pos event_config_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"-E",
        {},
        L"Provide customized config file which describes custom events or provide custom events from the command line.",
        {}
    );
    arg_parser_arg_pos output_filename_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"--output",
        { L"-o" },
        L"Specify JSON output filename.",
        {}
    );
    arg_parser_arg_pos output_csv_filename_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"--output-csv",
        {},
        L"Specify CSV output filename. Only with timeline `-t`.",
        {}
    );
    arg_parser_arg_pos output_prefix_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"--output-prefix",
        { L"--cwd" },
        L"Set current working dir for storing output JSON and CSV file.",
        {}
    );
    arg_parser_arg_pos config_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"--config",
        {},
        L"Specify configuration parameters.",
        {}
    );
    arg_parser_arg_pos interval_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"-i",
        {},
        L"Specify counting interval. `0` seconds is allowed. Input may be suffixed with one(or none) of the following units, with up to 2 decimal points : \"ms\", \"s\", \"m\", \"h\", \"d\" (i.e.milliseconds, seconds, minutes, hours, days).If no unit is provided, the default unit is seconds(60s by default).",
        {}
    );
    arg_parser_arg_pos iteration_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"-i",
        {},
        L"Number of consecutive counts in timeline mode (disabled by default).",
        {}
    );
    arg_parser_arg_pos dmc_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"--dmc",
        {},
        L"Profile on the specified DDR controller. Skip `--dmc` to count on all DMCs.",
        {}
    );
    arg_parser_arg_pos metrics_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"-m",
        {},
        L"Specify comma separated list of metrics to count.\n\nNote: see list of available metric names using `list` command.",
        {}
    );
    arg_parser_arg_pos events_arg = arg_parser_arg_pos::arg_parser_arg_pos(
        L"-e",
        {},
        L"Specify comma separated list of event names (or raw events) to count, for example `ld_spec,vfp_spec,r10`. Use curly braces to group events. Specify comma separated list of event names with sampling frequency to sample, for example `ld_spec:100000`. Raw events: specify raw evens with `r<VALUE>` where `<VALUE>` is a 16-bit hexadecimal event index value without leading `0x`. For example `r10` is event with index `0x10`. Note: see list of available event names using `list` command.",
        {}
    );
#pragma endregion

#pragma region Attributes

    COMMAND_CLASS m_command = COMMAND_CLASS::NO_COMMAND;
    std::vector<arg_parser_arg_command*> m_commands_list = {
       &list_command,
       &test_command,
       &help_command,
       &version_command,
       &detect_command,
       &sample_command,
       &record_command,
       &count_command,
       &man_command
    };
    
    std::vector<arg_parser_arg*> m_flags_list = {
       &json_opt,
       &kernel_opt,
       &force_lock_opt,
       &sample_display_long_opt,
       &verbose_opt,
       &quite_opt,
       &annotate_opt,
       &disassembly_opt,
       &timeline_opt,
       &cores_arg,
       &timeout_arg,
       &symbol_arg,
       &record_spawn_delay_arg,
       &sample_display_row_arg,
       &pe_file_arg,
       &image_name_arg,
       &pdb_file_arg,
       &metric_config_arg,
       &event_config_arg,
       &output_filename_arg,
       &output_csv_filename_arg,
       &output_prefix_arg,
       &config_arg,
       &interval_arg,
       &iteration_arg,
       &dmc_arg,
       &metrics_arg,
       &events_arg,
       &extra_args_arg
    };

    wstr_vec m_arg_array;

#pragma endregion

#pragma region Protected Methods
protected:
    void throw_invalid_arg(const std::wstring& arg, const std::wstring& additional_message = L"") const;
#pragma endregion
};