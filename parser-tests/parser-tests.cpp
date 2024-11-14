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

#include "pch.h"
#include "CppUnitTest.h"
#include <unordered_map>
#include "mock-arg-parser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace parsertests
{
    
    TEST_CLASS(parsertests)
    {
    public:

        TEST_METHOD(TEST_CORRECT_TEST_COMMAND)
        {
            const wchar_t* argv[] = { L"wperf", L"test", L"-v", L"--json" };
            int argc = 4;
            mock_arg_parser parser;
            parser.parse(argc, argv);

            Assert::AreEqual(true, parser.do_verbose.get());
            Assert::AreEqual(true, parser.do_json.get());
            Assert::IsTrue(COMMAND_CLASS::TEST == parser.command);
        }
        TEST_METHOD(TEST_RANDOM_ARGS_REJECTION)
        {
            const wchar_t* argv[] = { L"wperf", L"test", L"-v", L"--json", L"random" };
            int argc = 5;
            mock_arg_parser parser;
            Assert::ExpectException<std::invalid_argument>([&parser, argc, &argv]() {
                parser.parse(argc, argv);
                }
            );
        }
        // Test parsing the 'help' command with no arguments
        TEST_METHOD(TEST_HELP_COMMAND)
        {
            const wchar_t* argv[] = { L"wperf", L"--help" };
            int argc = 2;
            mock_arg_parser parser;
            parser.parse(argc, argv);

            Assert::IsTrue(parser.do_help.get());
            Assert::IsTrue(COMMAND_CLASS::HELP == parser.command);
        }

        // Test parsing the 'version' command with no arguments
        TEST_METHOD(TEST_VERSION_COMMAND)
        {
            const wchar_t* argv[] = { L"wperf", L"--version" };
            int argc = 2;
            mock_arg_parser parser;
            parser.parse(argc, argv);

            Assert::IsTrue(parser.do_version.get());
            Assert::IsTrue(COMMAND_CLASS::VERSION == parser.command);
        }

        // Test parsing the 'list' command with no arguments
        TEST_METHOD(TEST_LIST_COMMAND)
        {
            const wchar_t* argv[] = { L"wperf", L"list" };
            int argc = 2;
            mock_arg_parser parser;
            parser.parse(argc, argv);

            Assert::IsTrue(parser.do_list.get());
            Assert::IsTrue(COMMAND_CLASS::LIST == parser.command);
        }

        // Test parsing the 'sample' command with various flags
        TEST_METHOD(TEST_SAMPLE_COMMAND_WITH_MULTIPLE_CORES)
        {
            const wchar_t* argv[] = { L"wperf", L"sample", L"--annotate", L"--timeout", L"5s", L"-c", L"0,1,2" };
            int argc = 7;
            mock_arg_parser parser;

            Assert::ExpectException<std::invalid_argument>([&parser, argc, &argv]() {
                parser.parse(argc, argv);
                }
            );
        }

        // Test parsing the 'record' command with command line separator and arguments
        TEST_METHOD(TEST_RECORD_COMMAND_WITH_PROCESS)
        {
            const wchar_t* argv[] = { L"wperf", L"record", L"--", L"notepad.exe", L"test_arg" };
            int argc = 5;
            mock_arg_parser parser;
            parser.parse(argc, argv);

            Assert::IsTrue(parser.do_record.get());
            Assert::AreEqual(std::wstring(L"notepad.exe test_arg"), parser.record_commandline);
            Assert::AreEqual(std::wstring(L"notepad.exe"), parser.sample_pe_file.get());
            Assert::AreEqual(std::wstring(L"notepad.pdb"), parser.sample_pdb_file.get());
            Assert::IsTrue(COMMAND_CLASS::RECORD == parser.command);
        }

        // Test that missing required arguments cause exceptions
        TEST_METHOD(TEST_SAMPLE_COMMAND_MISSING_REQUIRED_ARG)
        {
            const wchar_t* argv[] = { L"wperf", L"sample", L"--timeout" };
            int argc = 3;
            mock_arg_parser parser;
            Assert::ExpectException<std::invalid_argument>([&parser, argc, &argv]() {
                parser.parse(argc, argv);
                }
            );
        }

        // Test that invalid arguments cause exceptions
        TEST_METHOD(TEST_SIMPLE_INVALID_ARGUMENT)
        {
            const wchar_t* argv[] = { L"wperf", L"invalid_command" };
            int argc = 2;
            mock_arg_parser parser;
            Assert::ExpectException<std::invalid_argument>([&parser, argc, &argv]() {
                parser.parse(argc, argv);
                }
            );
        }

        // Test parsing timeout with different units
        TEST_METHOD(TEST_TIMEOUT_WITH_UNITS)
        {
            const wchar_t* argv[] = { L"wperf", L"sample", L"--timeout", L"2m" };
            int argc = 4;
            mock_arg_parser parser;
            parser.parse(argc, argv);

            Assert::AreEqual(120.0, parser.count_duration.get()); // 2 minutes in seconds
        }

        // Test that invalid timeout format causes exception
        TEST_METHOD(TEST_INVALID_TIMEOUT_WITH_WRONG_UNIT)
        {
            const wchar_t* argv[] = { L"wperf", L"sample", L"--timeout", L"5x" };
            int argc = 4;
            mock_arg_parser parser;
            Assert::ExpectException<std::invalid_argument>([&parser, argc, &argv]() {
                parser.parse(argc, argv);
                }
            );
        }
        TEST_METHOD(TEST_INVALID_TIMEOUT_WITH_WRONG_FORMAT)
        {
            const wchar_t* argv[] = { L"wperf", L"sample", L"--timeout", L"5.4", L"ms"};
            int argc = 5;
            mock_arg_parser parser;
            Assert::ExpectException<std::invalid_argument>([&parser, argc, &argv]() {
                parser.parse(argc, argv);
                }
            );
        }

        // Test parsing the 'detect' command
        TEST_METHOD(TEST_DETECT_COMMAND)
        {
            const wchar_t* argv[] = { L"wperf", L"detect" };
            int argc = 2;
            mock_arg_parser parser;
            parser.parse(argc, argv);

            Assert::IsTrue(parser.do_detect.get());
            Assert::IsTrue(COMMAND_CLASS::DETECT == parser.command);
        }

        // Test parsing multiple flags together
        TEST_METHOD(TEST_MULTIPLE_FLAGS)
        {
            const wchar_t* argv[] = { L"wperf", L"sample", L"--verbose", L"-q", L"--json" };
            int argc = 5;
            mock_arg_parser parser;
            parser.parse(argc, argv);

            Assert::IsTrue(parser.do_verbose.get());
            Assert::IsTrue(parser.is_quite.get());
            Assert::IsTrue(parser.do_json.get());
            Assert::IsTrue(COMMAND_CLASS::SAMPLE == parser.command);
        }

        // Test parsing sample command with symbol argument
        TEST_METHOD(TEST_SAMPLE_COMMAND_WITH_SYMBOL)
        {
            const wchar_t* argv[] = { L"wperf", L"sample", L"--symbol", L"main" };
            int argc = 4;
            mock_arg_parser parser;
            parser.parse(argc, argv);

            Assert::AreEqual(std::wstring(L"main"), parser.symbol_arg.get());
            Assert::IsTrue(COMMAND_CLASS::SAMPLE == parser.command);
        }

        // Test parsing sample command with sample display row
        TEST_METHOD(TEST_SAMPLE_DISPLAY_ROW)
        {
            const wchar_t* argv[] = { L"wperf", L"sample", L"--sample-display-row", L"100" };
            int argc = 4;
            mock_arg_parser parser;
            parser.parse(argc, argv);

            Assert::AreEqual((uint32_t)100, parser.sample_display_row.get());
            Assert::IsTrue(COMMAND_CLASS::SAMPLE == parser.command);
        }

        // Test parsing sample command with pe_file
        TEST_METHOD(TEST_SAMPLE_COMMAND_WITH_PE_FILE)
        {
            const wchar_t* argv[] = { L"wperf", L"sample", L"--pe_file", L"C:\\Program\\sample.exe" };
            int argc = 4;
            mock_arg_parser parser;

            parser.parse(argc, argv);

            Assert::AreEqual(std::wstring(L"C:\\Program\\sample.exe"), parser.sample_pe_file.get());
            Assert::AreEqual(std::wstring(L"C:\\Program\\sample.pdb"), parser.sample_pdb_file.get());
            Assert::AreEqual(std::wstring(L"C:\\Program\\sample.exe"), parser.sample_image_name.get());
            Assert::IsTrue(COMMAND_CLASS::SAMPLE == parser.command);
        }

        // Test parsing sample command with pdb_file
        TEST_METHOD(TEST_SAMPLE_COMMAND_WITH_PDB_FILE)
        {
            const wchar_t* argv[] = { L"wperf", L"sample", L"--pdb_file", L"C:\\Program\\sample.pdb" };
            int argc = 4;
            mock_arg_parser parser;

            // Similarly, adjust or mock check_file_path for testing
            parser.parse(argc, argv);

            Assert::AreEqual(std::wstring(L"C:\\Program\\sample.pdb"), parser.sample_pdb_file.get());
            Assert::IsTrue(COMMAND_CLASS::SAMPLE == parser.command);
        }

        // Test parsing sample command with image_name
        TEST_METHOD(TEST_SAMPLE_COMMAND_WITH_IMAGE_NAME)
        {
            const wchar_t* argv[] = { L"wperf", L"sample", L"--image_name", L"notepad.exe" };
            int argc = 4;
            mock_arg_parser parser;
            parser.parse(argc, argv);

            Assert::AreEqual(std::wstring(L"notepad.exe"), parser.sample_image_name.get());
            Assert::IsTrue(COMMAND_CLASS::SAMPLE == parser.command);
        }

        // Test parsing with --force-lock flag
        TEST_METHOD(TEST_FORCE_LOCK_FLAG)
        {
            const wchar_t* argv[] = { L"wperf", L"test", L"--force-lock" };
            int argc = 3;
            mock_arg_parser parser;
            parser.parse(argc, argv);

            Assert::IsTrue(parser.do_force_lock.get());
            Assert::IsTrue(COMMAND_CLASS::TEST == parser.command);
        }

        // Test parsing the 'stat' command (not fully implemented in the parser)
        TEST_METHOD(TEST_STAT_COMMAND)
        {
            const wchar_t* argv[] = { L"wperf", L"stat" };
            int argc = 2;
            mock_arg_parser parser;
            parser.parse(argc, argv);

            // Assuming command is set correctly
            Assert::IsTrue(COMMAND_CLASS::STAT == parser.command);
        }

        // Test parsing with unknown flags
        TEST_METHOD(TEST_UNKNOWN_FLAG)
        {
            const wchar_t* argv[] = { L"wperf", L"sample", L"--unknown" };
            int argc = 3;
            mock_arg_parser parser;
            Assert::ExpectException<std::invalid_argument>([&parser, argc, &argv]() {
                parser.parse(argc, argv);
                }
            );
        }
    };
}
