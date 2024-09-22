#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <deque>
#include "geo.h"

namespace transport_catalogue {

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
};

struct Bus {
    std::string name;
    std::vector<const Stop*> stops;
    bool is_roundtrip;
};

class TransportCatalogue {
public:
    void AddStop(const std::string& name, double latitude, double longitude);
    void AddBus(const std::string& name, const std::vector<std::string>& stop_names, bool is_roundtrip);
    double CalculateRouteLength(const Bus& bus) const;
    const Bus* FindBus(const std::string& name) const;
    const Stop* FindStop(const std::string& name) const;
    double GetDistance(const std::string& stop_name_from, const std::string& stop_name_to) const;
    
    struct BusInfo {
        int stops_count;
        int unique_stops_count;
        double route_length;
    };
    
    BusInfo GetBusInfo(const std::string& name) const;
    std::set<std::string_view> GetBusesForStop(const std::string& stop_name) const;
    bool HasStop(const std::string& name) const;

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
    std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
    std::unordered_map<const Stop*, std::set<const Bus*>> stops_to_buses_;
    std::unordered_map<std::string_view, std::set<std::string_view>> stop_to_buses_;
};

} // namespace transport_catalogue
