// transport_catalogue.cpp
#include "transport_catalogue.h"
#include <algorithm>
#include <stdexcept>
#include <unordered_set>

void TransportCatalogue::AddStop(const std::string& name, double latitude, double longitude) {
    stops_.emplace_back(Stop{name, {latitude, longitude}});
    const Stop* stop_ptr = &stops_.back();
    stopname_to_stop_[stop_ptr->name] = stop_ptr;
}

void TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string>& stop_names, bool is_roundtrip) {
    buses_.push_back({name, {}, is_roundtrip});
    Bus* bus_ptr = &buses_.back();
    busname_to_bus_[bus_ptr->name] = bus_ptr;

    for (const auto& stop_name : stop_names) {
        const Stop* stop = FindStop(stop_name);
        if (!stop) {
            stops_.push_back({stop_name, {0, 0}});
            stop = &stops_.back();
            stopname_to_stop_[stop->name] = stop;
        }
        bus_ptr->stops.push_back(stop);
        stop_to_buses_[stop->name].insert(bus_ptr->name);
    }
}

const Bus* TransportCatalogue::FindBus(const std::string& name) const {
    if (auto it = busname_to_bus_.find(name); it != busname_to_bus_.end()) {
        return it->second;
    }
    return nullptr;
}


const Stop* TransportCatalogue::FindStop(const std::string& name) const {
    if (auto it = stopname_to_stop_.find(name); it != stopname_to_stop_.end()) {
        return it->second;
    }
    return nullptr;
}



TransportCatalogue::BusInfo TransportCatalogue::GetBusInfo(const std::string& name) const {
    const Bus* bus = FindBus(name);
    if (!bus) {
        return {0, 0, 0.0};
    }
    
    int stops_count = bus->is_roundtrip ? bus->stops.size() : bus->stops.size() * 2 - 1;
    
    std::unordered_set<const Stop*> unique_stops(bus->stops.begin(), bus->stops.end());
    int unique_stops_count = unique_stops.size();

    double route_length = 0.0;
    for (size_t i = 1; i < bus->stops.size(); ++i) {
        route_length += ComputeDistance(bus->stops[i-1]->coordinates, bus->stops[i]->coordinates);
    }
    
    if (!bus->is_roundtrip && bus->stops.size() > 1) {
        for (size_t i = bus->stops.size() - 1; i > 0; --i) {
            route_length += ComputeDistance(bus->stops[i]->coordinates, bus->stops[i-1]->coordinates);
        }
    }

    return {stops_count, unique_stops_count, route_length};
}

std::set<std::string_view> TransportCatalogue::GetBusesForStop(const std::string& stop_name) const {
    if (auto it = stop_to_buses_.find(stop_name); it != stop_to_buses_.end()) {
        return it->second;
    }
    return {};
}

bool TransportCatalogue::HasStop(const std::string& name) const {
    return stopname_to_stop_.count(name) > 0;
}
