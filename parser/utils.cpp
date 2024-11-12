#include <string>
#include <unordered_map>
/// <summary>
/// Convert time from one of [milliseconds, seconds, minutes, hours, days] to seconds
/// </summary> 
/// <param name="number">Input number to be converted</param>
/// <param name="unit">Unit of input number</param>
/// <param name="unitConversionMap">Map of wstring unit multiplier</param>
double ConvertNumberWithUnit(double number, std::wstring unit, const std::unordered_map<std::wstring, double>& unitConversionMap)
{
    //takes a number and a unit, multiplies the number by a value to obtain number in desired units
    auto it = unitConversionMap.find(unit);
    double multiplier = it->second;
    double result = number * multiplier;

    return result;
}