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
    buses_.push_back({name, stop_names, is_roundtrip});
    const Bus* bus_ptr = &buses_.back();
    busname_to_bus_[bus_ptr->name] = bus_ptr;

    // Добавляем остановки в список маршрута автобуса
    for (const auto& stop_name : bus_ptr->stop_names) {
        stop_to_buses_[stop_name].insert(name);
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

    int stops_count = bus->is_roundtrip ? bus->stop_names.size() : bus->stop_names.size() * 2 - 1;

    std::unordered_set<std::string_view> unique_stops;
    for (const auto& stop_name : bus->stop_names) {
        unique_stops.insert(stop_name);
    }
    int unique_stops_count = unique_stops.size();

    double route_length = 0.0;
    for (size_t i = 0; i < bus->stop_names.size() - 1; ++i) {
        const Stop* stop1 = FindStop(bus->stop_names[i]);
        const Stop* stop2 = FindStop(bus->stop_names[i + 1]);
        if (stop1 && stop2) {
            double distance = ComputeDistance(stop1->coordinates, stop2->coordinates);
            route_length += distance;
        }
    }

    if (bus->is_roundtrip) {
        const Stop* stop1 = FindStop(bus->stop_names.front());
        const Stop* stop2 = FindStop(bus->stop_names.back());
        if (stop1 && stop2) {
            double distance = ComputeDistance(stop1->coordinates, stop2->coordinates);
            route_length += distance;
        }
    }

    return {stops_count, unique_stops_count, route_length};
}

double TransportCatalogue::GetDistance(const std::string& stop_name_from, const std::string& stop_name_to) const {
    std::string key = stop_name_from + "->" + stop_name_to;
    auto it = distances_.find(key);
    if (it != distances_.end()) {
        return it->second;
    }

    // Проверка в обратном направлении
    key = stop_name_to + "->" + stop_name_from;
    it = distances_.find(key);
    if (it != distances_.end()) {
        return it->second;
    }

    return 0.0; // Если расстояние не задано
}


double TransportCatalogue::CalculateRouteLength(const Bus& bus) const {
    double total_length = 0.0;

    // Расчёт расстояния для прямого маршрута
    for (size_t i = 1; i < bus.stop_names.size(); ++i) {
        total_length += GetDistance(bus.stop_names[i - 1], bus.stop_names[i]);
    }
    
    // Если маршрут некольцевой, прибавляем расстояние для обратного пути
    if (!bus.is_roundtrip) {
        for (size_t i = bus.stop_names.size() - 1; i > 0; --i) {
            total_length += GetDistance(bus.stop_names[i], bus.stop_names[i - 1]);
        }
    }
    
    return total_length;
}
