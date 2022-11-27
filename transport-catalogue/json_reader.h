#pragma once

#include "json.h"
#include "domain.h"
#include "transport_catalogue.h"

#include <iostream>
#include <array>
#include <list>

namespace TransportsCatalogue {
    class JSONReader {
    public:
        JSONReader() {
        }
        void PrepareJSON(std::istream& streamDate);
        void GetCatalogue(TransportCatalogue& primary);
        void GetInfoBus(const json::Node& Businf);
        void GetInfoStop(const json::Node& Stop);
        void GetReqInf(const json::Node& ReqItem);
        void GetMapInfo(const json::Node& MapItem);
        void FillCatalogue();
        MapSettings& GetSetting();

        void PrintInfoStop(InfoToPrintStop item, const std::string& request_id);
        void PrintInfoBus(Stats item, const std::string& request_id);

        std::vector<StatRequest> GetReqInf();

    private:
        std::vector<StatRequest> ReqInf;
        json::Document Doc;
        MapSettings Mapset;
        TransportCatalogue* Temp;

    };

}