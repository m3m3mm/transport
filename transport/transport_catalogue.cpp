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
    
    // Подсчёт количества остановок
    int stops_count = bus->stop_names.size();
    
    // Подсчёт уникальных остановок
    std::unordered_set<std::string_view> unique_stops;
    for (const auto& stop_name : bus->stop_names) {
        unique_stops.insert(stop_name);
    }
    int unique_stops_count = unique_stops.size();

    // Вычисление длины маршрута
    double route_length = 0.0;
    for (size_t i = 1; i < bus->stop_names.size(); ++i) {
        const Stop* stop1 = FindStop(bus->stop_names[i - 1]);
        const Stop* stop2 = FindStop(bus->stop_names[i]);
        if (stop1 && stop2) {
            route_length += ComputeDistance(stop1->coordinates, stop2->coordinates);
        }
    }

    // Обработка обратного маршрута для не кольцевых маршрутов
    if (!bus->is_roundtrip) {
        for (size_t i = bus->stop_names.size(); i > 1; --i) {
            const Stop* stop1 = FindStop(bus->stop_names[i - 1]);
            const Stop* stop2 = FindStop(bus->stop_names[i - 2]);
            if (stop1 && stop2) {
                route_length += ComputeDistance(stop1->coordinates, stop2->coordinates);
            }
        }
    }

    return {stops_count, unique_stops_count, route_length};
}
