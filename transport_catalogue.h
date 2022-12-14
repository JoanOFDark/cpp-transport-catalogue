#pragma once

#include "geo.h"
#include "domain.h"

#include <iostream>
#include <deque>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <set>


namespace transport_catalogue {

    class TransportCatalogueHasher {
    public:
        std::size_t operator()(const StopToStop pair_of_pointers) const noexcept;
    private:
        std::hash<const void*> hasher_;
    };

    class TransportCatalogue {
    public:
        TransportCatalogue();
        std::deque<Stop>* GetStops();
        std::deque<Bus>* GetBuses();
        std::unordered_map<Stop*, std::set<std::string_view>>* GetStopWithBus();
        std::unordered_map<const StopToStop, double, TransportCatalogueHasher>* GetStopDistance();
        std::unordered_map<std::string_view, Stop*>* GetStopNameToStop();

        void PrepareStops();
        void PrepareDistance();
        void PrepareBus();
        void PrepareStopsWithBus();

        void AddStop(Stop& stop);
        void AddBus(Bus& bus);

        Stop* FindStop(const std::string& name);
        Bus* FindBus(const std::string& name);

        Stats GetBusInfo(const std::string& name);
        InfoToPrintStop GetStopInfo(std::string_view name);
        double GetDistance(Stop* A, Stop* B);

        void SetDistance(Stop* A, Stop* B, double distant);

    private:
        std::deque<Stop> stops;
        std::deque<Bus> buses;
        std::unordered_map<Stop*, std::set<std::string_view>> stop_with_bus;
        std::unordered_map<std::string_view, Stop*> stopname_to_stop;
        std::unordered_map<std::string_view, Bus*> busname_to_bus;
        std::unordered_map<const StopToStop, double, TransportCatalogueHasher> stop_distance;
    };
}