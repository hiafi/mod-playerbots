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

}  // namespace ai::qualifier

#endif
