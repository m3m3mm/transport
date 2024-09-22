#include "stat_reader.h"
#include <iostream>
#include <iomanip>

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output) {
    auto space_pos = request.find(' ');
    if (space_pos == request.npos) {
        return;
    }

    std::string_view command = request.substr(0, space_pos);
    std::string_view name = request.substr(space_pos + 1);

    if (command == "Bus") {
        const auto* bus = transport_catalogue.FindBus(std::string(name));
        if (!bus) {
            output << "Bus " << name << ": not found" << std::endl;
        } else {
            const auto bus_info = transport_catalogue.GetBusInfo(std::string(name));
            output << "Bus " << name << ": " << bus_info.stops_count << " stops on route, "
                   << bus_info.unique_stops_count << " unique stops, "
                   << std::setprecision(6) << bus_info.route_length << " route length" << std::endl;
        }
    }
}
