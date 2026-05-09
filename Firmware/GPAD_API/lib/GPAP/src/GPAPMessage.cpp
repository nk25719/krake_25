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

#include "GPAPMessage.h"
#include "Deserialize/AlarmMessageBuilder/AlarmMessageBuilder.h"

using namespace gpap_message;
GPAPMessage GPAPMessage::deserialize(const char *const buffer, const std::size_t numBytes)
{
    if (buffer == nullptr || numBytes == 0)
    {
        return GPAPMessage::invalid();
    }

    const auto messageType = static_cast<MessageType>(buffer[0]);

    switch (messageType)
    {
    case MessageType::ALARM:
        if (!deserialize::AlarmMessageBuilder::isValidAlarmMessage(buffer + 1, numBytes - 1))
        {
            return GPAPMessage::invalid();
        }
        return GPAPMessage(std::move(deserialize::AlarmMessageBuilder::buildAlarmMessage(buffer + 1, numBytes - 1)));

    case MessageType::INFO:
        return GPAPMessage(InfoMessage());

    case MessageType::MUTE:
        return GPAPMessage(MuteMessage());

    case MessageType::UNMUTE:
        return GPAPMessage(UnmuteMessage());

    case MessageType::HELP:
        return GPAPMessage(HelpMessage());

    default:
        return GPAPMessage::invalid();
    }
}

MessageType GPAPMessage::getMessageType() const noexcept
{
    return this->messageType;
}

const alarm::AlarmMessage &GPAPMessage::getAlarmMessage() const
{
    return this->alarm;
}
