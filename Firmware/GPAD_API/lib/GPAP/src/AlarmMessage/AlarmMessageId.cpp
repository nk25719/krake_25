/*
  Copyright (C) 2026 Public Invention

  This program includes free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  See the GNU Affero General Public License for more details.
  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "AlarmMessageId.h"

#include <algorithm>
#include <cctype>

#ifndef PIO_UNIT_TESTING
#include <Print.h>
#else
#include <MockPrint.h>
using Print = MockPrint;
#endif

using namespace gpap_message::alarm;

AlarmMessageId::AlarmMessageId(
    const std::size_t idLength,
    const Buffer id)
    : idLength(idLength), id(std::move(AlarmMessageId::validateId(idLength, id))) {}

std::array<char, AlarmMessageId::TOTAL_MAX_LENGTH> AlarmMessageId::validateId(
    const std::size_t idLength,
    const Buffer id)
{
    std::array<char, AlarmMessageId::TOTAL_MAX_LENGTH> validatedId = {};
    auto validatedIdIterator = validatedId.begin();

    // The recorded real-length of the message ID cannot me more than the max
    // number of elements
    if (idLength > id.size())
    {
        return validatedId;
    }

    auto startIterator = id.cbegin();
    auto endIterator = id.cbegin() + idLength;

    const bool allHex = std::all_of(
        startIterator,
        endIterator,
        [&validatedIdIterator](char hexChar)
        {
            *validatedIdIterator = hexChar;
            validatedIdIterator = std::next(validatedIdIterator, 1);

            return std::isxdigit(static_cast<unsigned char>(hexChar));
        });

    // Invalid characters leave the object printable but empty; validation rejects
    // malformed GPAP messages before construction on embedded builds.
    if (!allHex)
    {
        validatedId.fill('\0');
    }

    *validatedIdIterator = '\0';
    return std::move(validatedId);
}

AlarmMessageId::~AlarmMessageId() {}

std::size_t AlarmMessageId::printTo(Print &print) const
{
    auto beginIterator = this->id.cbegin();
    const auto endIterator = std::next(beginIterator, this->idLength);

    auto bytesWritten = 0;

    std::for_each(
        beginIterator,
        endIterator,
        [&bytesWritten, &print](const char &character)
        {
            bytesWritten += print.print(character);
        });

    bytesWritten += print.print('\0');
    return bytesWritten;
}
