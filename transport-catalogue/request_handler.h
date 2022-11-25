#pragma once

#include "json.h"
#include "domain.h"
#include "transport_catalogue.h"
#include "svg.h"
#include "map_renderer.h"
#include "iostream"
#include "json_reader.h"

#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <limits>
#include <iomanip>
#include <fstream>

 // Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
 // с другими подсистемами приложения.
 // См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
namespace TransportsCatalogue {
    class RequestHandler {
    public:
        RequestHandler(TransportsCatalogue::TransportCatalogue& db);
        void SetMapSetting(JsonReader& MapSer_);
        void FillingPolyline(svg::Polyline& Marshrut, JsonReader* MapSet, int clolorPaletIndex, std::string& color);
        svg::Document GetTraec();
        void FillingText(svg::Document& doc, std::deque<Bus>& orderBus, const TransportsCatalogue::Plane::SphereProjector& proj);
        void FillingCircl(std::vector<Stop>& stops, svg::Document& doc, const Plane::SphereProjector& proj) const;
        std::string GetColor(MapSetting settings);
        void  PrepareText(svg::Text& temp, bool zaliv, std::string busName, std::string color, const MapSetting& tempSetting);
        void  PrepareTextCoordinatsRing(svg::Text& temp, const Bus& bus, const Plane::SphereProjector& proj);
        bool  FindDublicate(Stop item, std::vector<Stop>& stops)const;
        void  PrepareTextCoordinatsNotRing(svg::Text& temp, const Bus& bus, const Plane::SphereProjector& proj, int lastStop, int stop_num);
        void  MainPrepareText(svg::Text& temp, svg::Document& doc, bool zalivka, bool ring, const Bus& bus, const Plane::SphereProjector& proj, const MapSetting& tempSetting);
        void  FillingTextStop(std::vector<Stop>& stops, svg::Document& doc, const Plane::SphereProjector& proj);
        void  PrepareTextStop(svg::Text& temp, bool zaliv, std::string busName, std::string color, const MapSetting& tempSetting, const Stop stop, const Plane::SphereProjector& proj);
        void  ScreeningMap(std::string& str);

    private:
        TransportCatalogue& Temp;
        JsonReader* MapSet;
        std::map<std::string, std::string> busWithColor;
    };
}