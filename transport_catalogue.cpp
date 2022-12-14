#include "transport_catalogue.h" 

namespace transport_catalogue {

    TransportCatalogue::TransportCatalogue() {}

    std::deque<Stop>* TransportCatalogue::GetStops() {
        return &stops;
    }

    std::deque<Bus>* TransportCatalogue::GetBuses()
    {
        return &buses;
    }

    std::unordered_map<std::string_view, Stop*>* TransportCatalogue::GetStopNameToStop()
    {
        return  &stopname_to_stop;
    }

    std::unordered_map<Stop*, std::set<std::string_view>>* TransportCatalogue::GetStopWithBus()
    {
        return &stop_with_bus;
    }

    std::unordered_map<const StopToStop, double, TransportCatalogueHasher>* TransportCatalogue::GetStopDistance()
    {
        return &stop_distance;
    }

    void TransportCatalogue::PrepareStops()
    {
        for (Stop& temp_stop : stops)
            AddStop(temp_stop);
    }

    void TransportCatalogue::PrepareDistance()
    {
        for (Stop& temp_stop : stops)
        {
            for (const auto& item : temp_stop.road_distances)
                SetDistance(&temp_stop, GetStopNameToStop()->at(item.first), item.second);
        }
    }

    void TransportCatalogue::PrepareBus()
    {
        for (Bus& temp_bus : buses)
            AddBus(temp_bus);
    }

    void TransportCatalogue::PrepareStopsWithBus()
    {
        for (Bus& temp_bus : buses)
        {
            for (auto& BussStop : temp_bus.bus_stop) {
                stop_with_bus[BussStop].insert(temp_bus.name);
            }
        }

    }

    void TransportCatalogue::AddStop(Stop& stop)
    {
        stopname_to_stop[stop.name] = &stop;
    }

    void TransportCatalogue::AddBus(Bus& bus)
    {
        busname_to_bus[bus.name] = &bus;
    }

    Bus* TransportCatalogue::FindBus(const std::string& name)
    {
        if (busname_to_bus.count(name) != 0) {
            return busname_to_bus[name];
        }
        return nullptr;
    }

    Stats TransportCatalogue::GetBusInfo(const std::string& bus_name)
    {
        Stats stats{};
        if (busname_to_bus.count(bus_name) == 0) {
            return stats;
        }

        else {
            std::unordered_set<Stop*> unique_stops;
            stats.stops = static_cast<int>(busname_to_bus.at(bus_name)->bus_stop.size());
            unique_stops.insert(busname_to_bus.at(bus_name)->bus_stop.begin(), busname_to_bus.at(bus_name)->bus_stop.end());
            stats.unique_stops = static_cast<int>(unique_stops.size());
            double distance2 = 0;

            for (auto to = busname_to_bus.at(bus_name)->bus_stop.begin(), from = to++; to != busname_to_bus.at(bus_name)->bus_stop.end(); ++to, ++from) {
                {
                    detail::distant::Coordinates A, B;
                    A.lat = (*from)->latitude;
                    A.lng = (*from)->longitude;
                    B.lat = (*to)->latitude;
                    B.lng = (*to)->longitude;
                    distance2 += GetDistance(*from, *to);
                    auto distance = ComputeDistance(A, B);
                    stats.route_length = distance2;
                    stats.route_length2 += distance;
                }
            }
            stats.route_length2 = stats.route_length / stats.route_length2;
        }
        return stats;

    }

    InfoToPrintStop TransportCatalogue::GetStopInfo(std::string_view name)
    {
        InfoToPrintStop item;
        if (stopname_to_stop.count(name) != 0) {
            item.stop_exist = true;
            if (stop_with_bus.count(stopname_to_stop.at(name)) != 0) {
                for (auto& bus : stop_with_bus.at(stopname_to_stop.at(name))) {
                    item.buss.push_back(move(bus));
                }
            }
        }
        return item;
    }

    void TransportCatalogue::SetDistance(Stop* A, Stop* B, double distant)
    {
        GetStopDistance()->operator[](StopToStop(A, B)) = distant;
    }

    double TransportCatalogue::GetDistance(Stop* A, Stop* B)
    {
        if (GetStopDistance()->count(StopToStop(A, B)) != 0)
            return GetStopDistance()->at(StopToStop(A, B));
        else
            return GetStopDistance()->at(StopToStop(B, A));
        return 0.0;
    }

    Stop* TransportCatalogue::FindStop(const std::string& name)
    {
        if (stopname_to_stop.count(name) != 0) {
            return stopname_to_stop[name];
        }
        return nullptr;
    }

    std::size_t TransportCatalogueHasher::operator()(const StopToStop pair_of_pointers) const noexcept
    {
        auto ptr1 = static_cast<const void*>(pair_of_pointers.AtoB.first);
        auto ptr2 = static_cast<const void*>(pair_of_pointers.AtoB.second);
        return hasher_(ptr1) * 37 + hasher_(ptr2);
    }
}