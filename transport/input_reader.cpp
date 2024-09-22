#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>

namespace transport_catalogue {
namespace input {
namespace detail {

geo::Coordinates ParseCoordinates(std::string_view str) {
    size_t comma = str.find(',');
    if (comma == str.npos) {
        return {0, 0};
    }

    double lat = std::stod(std::string(str.substr(0, comma)));
    double lng = std::stod(std::string(str.substr(comma + 1)));

    return {lat, lng};
}

std::vector<std::string> ParseRoute(std::string_view route) {
    std::vector<std::string> stops;
    size_t pos = 0;
    while (pos < route.length()) {
        size_t next = route.find_first_of(">-", pos);
        if (next == std::string_view::npos) {
            next = route.length();
        }
        std::string_view stop = route.substr(pos, next - pos);
        stop.remove_prefix(std::min(stop.find_first_not_of(' '), stop.size()));
        stop.remove_suffix(stop.size() - stop.find_last_not_of(' ') - 1);
        stops.push_back(std::string(stop));
        pos = next + 1;
    }
    return stops;
}

} // namespace detail

void InputReader::ParseLine(std::string_view line) {
    size_t colon = line.find(':');
    if (colon == line.npos) {
        return;
    }

    std::string_view command_part = line.substr(0, colon);
    size_t space = command_part.find(' ');
    if (space == command_part.npos) {
        return;
    }

    commands_.push_back({
        std::string(command_part.substr(0, space)),
        std::string(command_part.substr(space + 1)),
        std::string(line.substr(colon + 2))
    });
}

void InputReader::ApplyCommands(TransportCatalogue& catalogue) const {
    for (const auto& command : commands_) {
        if (command.command == "Stop") {
            auto [lat, lng] = detail::ParseCoordinates(command.description);
            catalogue.AddStop(command.id, lat, lng);
        }
    }

    for (const auto& command : commands_) {
        if (command.command == "Bus") {
            auto stops = detail::ParseRoute(command.description);
            bool is_roundtrip = command.description.find('>') != std::string::npos;
            catalogue.AddBus(command.id, {stops.begin(), stops.end()}, is_roundtrip);
        }
    }
}

} // namespace input
} // namespace transport_catalogue
