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
#include "parser/arg-parser.h"

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
            arg_parser parser;
            parser.parse(argc, argv);

            Assert::AreEqual(true, parser.do_verbose.get());
            Assert::AreEqual(true, parser.do_json.get());
            Assert::IsTrue(COMMAND_CLASS::TEST == parser.command);
        }
        TEST_METHOD(TEST_RANDOM_ARGS_REJECTION)
        {
            const wchar_t* argv[] = { L"wperf", L"test", L"-v", L"--json", L"random" };
            int argc = 5;
            arg_parser parser;
            Assert::ExpectException<std::invalid_argument>([&parser, argc, &argv]() {
                parser.parse(argc, argv);
                }
            );
        }
    };
}
