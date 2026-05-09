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

#include "AlarmMessageBuilder.h"
#include <cctype>

using namespace gpap_message::alarm;
using namespace gpap_message::deserialize;

AlarmMessageBuilder::AlarmMessageBuilder()
    : level(AlarmMessage::Level::Level1), idLength(0), idBuffer({}),
      designatorLength(0), designatorBuffer({}), messageLength(0),
      messageBuffer({})
{
}

std::size_t AlarmMessageBuilder::deserializeLevel(const char *const buffer, const std::size_t numBytes)
{
    if (numBytes == 0 || buffer == nullptr)
    {
        return 0;
    }

    this->level = static_cast<AlarmMessage::Level>(buffer[0]);

    return 1;
}

std::size_t AlarmMessageBuilder::deserializeId(const char *const buffer, const std::size_t numBytes)
{
    if (numBytes == 0 || (buffer[0] != AlarmMessageBuilder::ID_START_CHARACTER))
    {
        return 0;
    }

    auto idLength = 0;
    auto foundEnd = false;

    // Going 1 more than the max since the end character, ], can be after the max
    // length of the ID
    while ((idLength < (AlarmMessageId::MAX_LENGTH + 1)) &&
           ((idLength + 1) < numBytes) && !foundEnd)
    {
        // Need to offset the index by 1 since we have to account for the starting
        // character, [
        auto bufferIndex = idLength + 1;
        if (buffer[bufferIndex] == AlarmMessageBuilder::ID_END_CHARACTER)
        {
            foundEnd = true;
        }
        else if (idLength < AlarmMessageId::MAX_LENGTH)
        {
            this->idBuffer[idLength] = buffer[bufferIndex];
            ++idLength;
        }
    }

    // If the terminating character, }, was not found then it is an invalid string
    if (!foundEnd)
    {
        return numBytes;
    }
    else
    {
        this->idLength = idLength;

        // Add 2 to the index value since that will account for the delimiting
        // characters, '{' and '}'
        return idLength + 2;
    }
}

std::size_t
AlarmMessageBuilder::deserializeTypeDesignator(const char *const buffer, const std::size_t numBytes)
{
    if ((numBytes == 0) ||
        buffer[0] != AlarmMessageBuilder::DESIGNATOR_START_CHARACTER)
    {
        return 0;
    }

    const auto currBuffer = buffer + 1;

    auto designatorLength = 0;
    auto foundEnd = false;
    while ((designatorLength < (AlarmTypeDesignator::DESIGNATOR_LENGTH + 1)) &&
           (designatorLength < numBytes) && !foundEnd)
    {
        // Need to offset the index by 1 since we have to account for the starting
        // character, {
        if (currBuffer[designatorLength] == AlarmMessageBuilder::DESIGNATOR_END_CHARACTER)
        {
            foundEnd = true;
        }
        else if (designatorLength < AlarmTypeDesignator::DESIGNATOR_LENGTH)
        {
            this->designatorBuffer[designatorLength] = currBuffer[designatorLength];
            ++designatorLength;
        }
    }

    if (!foundEnd ||
        (designatorLength != AlarmTypeDesignator::DESIGNATOR_LENGTH &&
         designatorLength != 0))
    {
        return numBytes;
    }
    else
    {
        this->designatorLength = designatorLength;

        // Add 2 to the index value since that will account for the delimiting
        // characters, '{' and '}'
        return designatorLength + 2;
    }
}

std::size_t AlarmMessageBuilder::deserializeMessage(const char *const buffer, const std::size_t numBytes)
{
    auto messageLength = 0;
    for (; messageLength < numBytes; ++messageLength)
    {
        if (AlarmMessageBuilder::isReservedCharacter(buffer[messageLength]))
        {
            break;
        }

        if (messageLength >= AlarmContent::MAX_LENGTH)
        {
            break;
        }

        this->messageBuffer[messageLength] = buffer[messageLength];
    }

    this->messageLength = messageLength;
    return messageLength;
}

AlarmMessage AlarmMessageBuilder::buildAlarmMessage(const char *const buffer, const std::size_t numBytes)
{
    AlarmMessageBuilder builder = AlarmMessageBuilder();

    auto totalBytes = builder.deserializeLevel(buffer, numBytes);

    auto foundId = false;
    auto foundDesignator = false;
    auto foundMessage = false;

    while ((totalBytes < numBytes) && !(foundId && foundDesignator && foundMessage))
    {
        auto currBuffer = buffer + totalBytes;
        auto currNumBytes = numBytes - totalBytes;
        if (currBuffer[0] == AlarmMessageBuilder::ID_START_CHARACTER && !foundId)
        {
            totalBytes += builder.deserializeId(currBuffer, currNumBytes);

            foundId = true;
        }
        else if (currBuffer[0] == AlarmMessageBuilder::DESIGNATOR_START_CHARACTER && !foundDesignator)
        {
            totalBytes += builder.deserializeTypeDesignator(currBuffer, currNumBytes);

            foundDesignator = true;
        }
        else if (!foundMessage)
        {
            totalBytes += builder.deserializeMessage(currBuffer, currNumBytes);

            foundMessage = true;
        }
    }

    // The use of the lambda function here, and for creating the `PossibleParameter<AlarmTypeDesignator> allows
    // for conditionally setting the variable value messageId. If messageId was assigned outside of a lambda it
    // could not be const. The compiler will inline the lambda since it is called right away so there is no
    // "inefficiency" due to leveraging this
    const AlarmMessage::PossibleMessageId messageId = [](const std::size_t numBytes, const AlarmMessageId::Buffer buffer)
    {
        if (numBytes == 0)
        {
            return std::move(AlarmMessage::PossibleMessageId());
        }

        const AlarmMessageId messageId(numBytes, std::move(buffer));
        const AlarmMessage::PossibleMessageId possibleMessageId(std::move(messageId));

        return possibleMessageId;
    }(builder.idLength, std::move(builder.idBuffer));

    const AlarmMessage::PossibleTypeDesignator typeDesignator = [](const std::size_t numBytes, const AlarmTypeDesignator::Buffer buffer)
    {
        if (numBytes == 0)
        {
            return AlarmMessage::PossibleTypeDesignator();
        }

        const AlarmTypeDesignator typeDesignator(std::move(buffer));
        const AlarmMessage::PossibleTypeDesignator possibleTypeDesignator(std::move(typeDesignator));

        return possibleTypeDesignator;
    }(builder.designatorLength, std::move(builder.designatorBuffer));

    const auto content =
        AlarmContent(builder.messageLength, std::move(builder.messageBuffer));

    return AlarmMessage(builder.level,
                        std::move(content),
                        std::move(messageId),
                        std::move(typeDesignator));
}

bool AlarmMessageBuilder::isValidAlarmMessage(const char *const buffer, const std::size_t numBytes) noexcept
{
    if (buffer == nullptr || numBytes == 0)
    {
        return false;
    }

    const char level = buffer[0];
    if (level < '0' || level > '5')
    {
        return false;
    }

    bool foundId = false;
    bool foundDesignator = false;
    bool foundMessage = false;
    std::size_t index = 1;

    while (index < numBytes)
    {
        const char current = buffer[index];
        if (current == ID_START_CHARACTER)
        {
            if (foundId)
            {
                return false;
            }
            foundId = true;
            std::size_t idLength = 0;
            ++index;
            while (index < numBytes && buffer[index] != ID_END_CHARACTER)
            {
                if (++idLength > AlarmMessageId::MAX_LENGTH || isReservedCharacter(buffer[index]) ||
                    !std::isxdigit(static_cast<unsigned char>(buffer[index])))
                {
                    return false;
                }
                ++index;
            }
            if (index >= numBytes || buffer[index] != ID_END_CHARACTER)
            {
                return false;
            }
            ++index;
            continue;
        }

        if (current == DESIGNATOR_START_CHARACTER)
        {
            if (foundDesignator)
            {
                return false;
            }
            foundDesignator = true;
            std::size_t designatorLength = 0;
            ++index;
            while (index < numBytes && buffer[index] != DESIGNATOR_END_CHARACTER)
            {
                if (++designatorLength > AlarmTypeDesignator::DESIGNATOR_LENGTH || isReservedCharacter(buffer[index]) ||
                    !std::isdigit(static_cast<unsigned char>(buffer[index])))
                {
                    return false;
                }
                ++index;
            }
            if (index >= numBytes || buffer[index] != DESIGNATOR_END_CHARACTER ||
                (designatorLength != 0 && designatorLength != AlarmTypeDesignator::DESIGNATOR_LENGTH))
            {
                return false;
            }
            ++index;
            continue;
        }

        if (foundMessage || isReservedCharacter(current))
        {
            return false;
        }
        foundMessage = true;
        std::size_t messageLength = 0;
        while (index < numBytes && !isReservedCharacter(buffer[index]))
        {
            if (++messageLength > AlarmContent::MAX_LENGTH)
            {
                return false;
            }
            ++index;
        }
    }

    return true;
}

bool AlarmMessageBuilder::isReservedCharacter(const char character)
{
    return (character == AlarmMessageBuilder::DESIGNATOR_START_CHARACTER ||
            character == AlarmMessageBuilder::DESIGNATOR_END_CHARACTER ||
            character == AlarmMessageBuilder::ID_START_CHARACTER ||
            character == AlarmMessageBuilder::ID_END_CHARACTER);
}
