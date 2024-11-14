#pragma once

#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <string>
#include <type_traits>
#include <iosfwd>
#include <unordered_map>
#include <regex>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cwctype>
#include <unordered_map>
#include <numeric>

/// <summary>
/// Function tokenizes string which contains a range, returning the validity of the string (and the output vector)
/// Example Input:
/// 
///     L"0-4"
/// 
/// </summary>
/// <typeparam name="T"> INT</typeparam>
/// <param name="Range">Input Range as a wide string</param>
/// <param name="Separator">Wide Character to seperate range, e.g. L'-' </param>
/// <param name="Output">Output Vector</param>
/// <returns></returns>
template<typename T>
_Success_(return) bool TokenizeWideStringofIntRange(_In_ std::wstring Range, _In_ wchar_t Separator, _Out_ std::vector<T>&Output) {

    auto dash_pos = Range.find(Separator);

    if (dash_pos != std::wstring::npos) {
        std::wstring startWString = Range.substr(0, dash_pos);
        std::wstring endWString = Range.substr(dash_pos + 1);

        if (std::all_of(startWString.begin(), startWString.end(), ::isdigit) && std::all_of(endWString.begin(), endWString.end(), ::isdigit)) {

            T startNum = (T)_wtoi(startWString.c_str());
            T endNum = (T)_wtoi(endWString.c_str());

            T lowerBound = startNum < endNum ? startNum : endNum;
            T upperBound = startNum < endNum ? endNum : startNum;

            for (T i = lowerBound; i <= upperBound; i++) {
                Output.push_back((i));
            }
        }
        else
            return false;
    }
    else
        return false;

    return true;
}


/// <summary>
/// Function tokenizes string and returns vector in INT values.
/// Example string input:
///
///    L"0,2,3,5"
///
/// </summary>
/// <param name="Input">Input WSTRING</param>
/// <param name="Delimiter">Delimeter used to tokenize INPUT</param>
/// <param name="Output">Vector with tokenized values (is cleared by function)</param>
/// <returns>Count of elements tokenized</returns>
template<typename T>
bool TokenizeWideStringOfInts(_In_ std::wstring Input, _In_  const wchar_t Delimiter, _Out_ std::vector<T>& Output) {
    static_assert(std::is_integral<T>::value, "Integral type required in Output<T>");

    std::wstring token;
    std::wistringstream ss(Input);

    std::wstring regex_string = L"^[0-9]+\\-[0-9]+$";
    std::wregex r(regex_string);
    std::wsmatch match;

    Output.clear();
    while (std::getline(ss, token, Delimiter)) {
        if (std::all_of(token.begin(), token.end(), ::isdigit))
            Output.push_back((T)_wtoi(token.c_str()));
        else if (std::regex_search(token, match, r)) {              //enables the function to accept different input formats, here: ranges e.g 1-3 = 1,2,3
            TokenizeWideStringofIntRange(token, L'-', Output);
        }
        else
            return false;
    }

    return true;
}


double ConvertNumberWithUnit(double number, std::wstring unit, const std::unordered_map<std::wstring, double>& unitConversionMap);
std::wstring ReplaceFileExtension(std::wstring filename, std::wstring ext);
