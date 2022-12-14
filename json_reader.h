#pragma once

#include "json.h"
#include "domain.h"
#include "transport_catalogue.h"

#include <iostream>
#include <array>
#include <list>

namespace transport_catalogue {
    class JSONReader {
    public:
        JSONReader() {}
        void PrepareJson(std::istream& stream);
        void GetCatalogue(TransportCatalogue& primary);
        void GetInfoBus(const json::Node& bus_info);
        void GetInfoStop(const json::Node& stop);
        void GetRequestInfo(const json::Node& request_item);
        void GetMapInfo(const json::Node& map_item);
        void GetRouterInfo(const json::Node& router_item);
        void FillCatalogue();
        MapSettings& GetSetting();
        RoutingSettings& GetRouterSetting();

        void PrintInfoStop(InfoToPrintStop item, std::string request_id);
        void PrintInfoBus(Stats item, std::string request_id);

        std::vector<StatRequest> GetRequestInfo();

    private:
        std::vector<StatRequest> requests_;
        json::Document document_;
        MapSettings map_settings_;
        RoutingSettings router_settings_;
        TransportCatalogue* catalogue_;
    };

}