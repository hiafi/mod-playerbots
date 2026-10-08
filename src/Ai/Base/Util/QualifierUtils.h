/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_QUALIFIERUTILS_H
#define PLAYERBOTS_QUALIFIERUTILS_H

#include "Common.h"
#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <string>
#include <vector>

namespace ai::qualifier
{

// Narrows a count to a uint8 value, saturating instead of wrapping.
inline uint8 ClampCount(uint32 count)
{
    return static_cast<uint8>(std::min<uint32>(count, std::numeric_limits<uint8>::max()));
}

// Parses a comma-separated list of numbers such as "12,104". Returns an empty vector unless the qualifier
// holds exactly `count` valid, finite numbers, so callers can bail out on a missing or garbage qualifier.
inline std::vector<float> ParseNumbers(std::string const& qualifier, size_t count)
{
    std::vector<float> numbers;
    size_t start = 0;
    while (start <= qualifier.size())
    {
        size_t end = qualifier.find(',', start);
        if (end == std::string::npos)
            end = qualifier.size();

        std::string const token = qualifier.substr(start, end - start);
        char* parsedEnd = nullptr;
        errno = 0;
        float const number = std::strtof(token.c_str(), &parsedEnd);
        if (token.empty() || parsedEnd != token.c_str() + token.size() || errno != 0 || !std::isfinite(number))
            return {};

        numbers.push_back(number);
        start = end + 1;
    }

    if (numbers.size() != count)
        return {};

    return numbers;
}

// Splits on a separator and keeps empty fields, so two adjacent separators give an empty field between them.
inline std::vector<std::string> Split(std::string const& qualifier, char separator)
{
    std::vector<std::string> fields;
    size_t start = 0;
    while (true)
    {
        size_t const end = qualifier.find(separator, start);
        if (end == std::string::npos)
        {
            fields.push_back(qualifier.substr(start));
            break;
        }

        fields.push_back(qualifier.substr(start, end - start));
        start = end + 1;
    }

    return fields;
}

// Parses a comma-separated list of spell ids such as "589,594". Returns an empty vector if the list is empty
// or any entry is not a positive whole number.
inline std::vector<uint32> ParseIds(std::string const& list)
{
    std::vector<uint32> ids;
    for (std::string const& token : Split(list, ','))
    {
        char* parsedEnd = nullptr;
        errno = 0;
        unsigned long long const id = std::strtoull(token.c_str(), &parsedEnd, 10);
        if (token.empty() || token[0] == '-' || parsedEnd != token.c_str() + token.size() || errno != 0 || id == 0 ||
            id > std::numeric_limits<int32>::max())
            return {};

        ids.push_back(static_cast<uint32>(id));
    }

    return ids;
}

// Parses one finite number; false (and `out` untouched) on anything else.
inline bool ParseNumber(std::string const& token, float& out)
{
    std::vector<float> const numbers = ParseNumbers(token, 1);
    if (numbers.empty())
        return false;

    out = numbers[0];
    return true;
}

}  // namespace ai::qualifier

#endif
