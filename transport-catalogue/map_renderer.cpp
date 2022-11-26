#include "map_renderer.h"

#include <sstream>

using namespace std;

namespace TransportsCatalogue {

    namespace renderer {

        MapRenderer::MapRenderer(TransportCatalogue& db) :Temp(db)
        {

        }

        void MapRenderer::SetMapSetting(jsonReader& MapSet_)
        {
            MapSet = &MapSet_;
        }

        void MapRenderer::FillPolyline(svg::Polyline& route, TransportsCatalogue::jsonReader* MapSet, int colorPaletIndex, string& color)
        {
            MapSetting tempSet = MapSet->GetSetting();
            route.GetFill() = "none";
            if (std::holds_alternative<std::string>(MapSet->GetSetting().color_palette[colorPaletIndex])) {
                route.GetStroke() = std::get<std::string>(MapSet->GetSetting().color_palette[colorPaletIndex]);
            }

            if (std::holds_alternative<std::array<int, 3>>(MapSet->GetSetting().color_palette[colorPaletIndex])) {
                std::string str = "rgb(";
                int i = 0;
                for (auto& color : std::get<std::array<int, 3>>(MapSet->GetSetting().color_palette[colorPaletIndex])) {
                    if (i != 0) {
                        str += ",";
                    }
                    ++i;
                    str += std::to_string(color);
                }
                str += ")";
                route.GetStroke() = str;
            }
            if (std::holds_alternative<std::array<double, 4>>(MapSet->GetSetting().color_palette[colorPaletIndex])) {
                std::string str = "rgba(";
                int i = 0;
                for (auto& color : std::get<std::array<double, 4>>(MapSet->GetSetting().color_palette[colorPaletIndex])) {
                    if (i != 0) {
                        str += ",";
                    }
                    ++i;

                    std::ostringstream strs;
                    strs << color;
                    str += strs.str();

                }
                str += ")";
                route.GetStroke() = str;
            }
            color = route.GetStroke();
            std::ostringstream strs;
            strs << MapSet->GetSetting().line_width;
            route.GetStroke_width() = strs.str();
            route.GetStroke_linecap() = "round";
            route.GetStroke_linejoin() = "round";

        }

        svg::Document MapRenderer::GetMap()
        {
            MapSetting tempSetting = MapSet->GetSetting();
            const double WIDTH = tempSetting.width;
            const double HEIGHT = tempSetting.height;
            const double PADDING = tempSetting.padding;


            svg::Document doc;

            // Точки, подлежащие проецированию
            vector<TransportsCatalogue::detail::distant::Coordinates> geo_coords;
            vector<int> countStopInTrafic;
            vector<int> countPoint;
            std::deque<Bus> orderBus;
            orderBus = *Temp.GetBuses();

            sort(orderBus.begin(), orderBus.end(), [](Bus first, Bus second) {
                return std::lexicographical_compare(first.name.begin(), first.name.end(), second.name.begin(), second.name.end()); });


            std::vector<Stop> stops;
            for (auto& bus : orderBus) {
                for (auto& stop : bus.busStop) {
                    if (!FindDuplicate(*stop, stops)) {
                        stops.push_back(*stop);
                    }
                }
            }

            sort(stops.begin(), stops.end(), [](Stop first, Stop second) {
                return std::lexicographical_compare(first.name.begin(), first.name.end(), second.name.begin(), second.name.end()); });


            for (auto& item : orderBus) {
                int count = 0;
                for (auto& stopFromBus : item.busStop) {
                    geo_coords.push_back(TransportsCatalogue::detail::distant::Coordinates(stopFromBus->latitude, stopFromBus->longitude));
                    ++count;
                }

                countStopInTrafic.push_back(count);
            }

            // Создаём проектор сферических координат на карту
            const TransportsCatalogue::Plane::SphereProjector proj{
                geo_coords.begin(), geo_coords.end(), WIDTH, HEIGHT, PADDING
            };

            int clolorPaletIndex = 0;
            for (auto& bus : orderBus) {
                svg::Polyline Marshrut;
                string color;
                FillPolyline(Marshrut, MapSet, clolorPaletIndex % tempSetting.color_palette.size(), color);
                busWithColor[bus.name] = color;
                bool dontAdd = true;
                for (auto& stop : bus.busStop) {
                    dontAdd = false;
                    const svg::Point screen_coord = proj(TransportsCatalogue::detail::distant::Coordinates(stop->latitude, stop->longitude));
                    Marshrut.AddPoint(screen_coord);
                }
                if (dontAdd) {
                    continue;
                }
                doc.Add(Marshrut);
                ++clolorPaletIndex;
            }

            FillText(doc, orderBus, proj);
            FillCircle(stops, doc, proj);
            FillTextStop(stops, doc, proj);

            geo_coords.clear();

            std::string str;
            std::ostringstream strs;

            doc.Render(strs);
            str += strs.str();
            PrintMap(str);

            return doc;
        }

        void MapRenderer::FillTextStop(std::vector<Stop>& stops, svg::Document& doc, const Plane::SphereProjector& proj)
        {
            svg::Text text_1, text_2;
            for (auto& stop : stops) {
                svg::Text text_1;
                svg::Text text_2;
                PrepareTextStop(text_1, true, stop.name, ColorToText(MapSet->GetSetting()), MapSet->GetSetting(), stop, proj);
                PrepareTextStop(text_2, false, stop.name, "black", MapSet->GetSetting(), stop, proj);
                doc.Add(text_1);
                doc.Add(text_2);
            }

        }
        void MapRenderer::FillText(svg::Document& doc, std::deque<Bus>& orderBus, const Plane::SphereProjector& proj)
        {
            MapSetting tempSetting = MapSet->GetSetting();
            for (auto& bus : orderBus) {
                if (bus.is_roundtrip == true) {
                    svg::Text text_1, text_2;
                    MainPrepareText(text_1, doc, true, true, bus, proj, tempSetting);
                    MainPrepareText(text_2, doc, false, true, bus, proj, tempSetting);

                }
                else {
                    if (bus.busStop.operator[](0)->name != bus.busStop.operator[](bus.busStop.size() / 2)->name) {
                        svg::Text text_1, text_2, text_3, text_4;
                        MainPrepareText(text_1, doc, true, true, bus, proj, tempSetting);
                        MainPrepareText(text_2, doc, false, true, bus, proj, tempSetting);
                        MainPrepareText(text_3, doc, true, false, bus, proj, tempSetting);
                        MainPrepareText(text_4, doc, false, false, bus, proj, tempSetting);
                    }
                    else {
                        svg::Text text_1, text_2;
                        MainPrepareText(text_1, doc, true, true, bus, proj, tempSetting);
                        MainPrepareText(text_2, doc, false, true, bus, proj, tempSetting);
                    }
                }
            }
        }
        void MapRenderer::FillCircle(std::vector<Stop>& stops, svg::Document& doc, const Plane::SphereProjector& proj)const
        {
            MapSetting tempSetting = MapSet->GetSetting();
            for (auto& stop : stops) {
                svg::Circle Point;
                Point.SetRadius(tempSetting.stop_radius);
                Point.SetFill("white");
                svg::Point screen_coord = proj(TransportsCatalogue::detail::distant::Coordinates(stop.latitude, stop.longitude));
                Point.SetCenter(screen_coord);
                doc.Add(Point);
            }

        }
       
        void MapRenderer::PrintMap(string& str)
        {
            for (auto c : str) {
                if (c == '\"') {
                    cout << '\\';
                    std::cout << c;
                }
                else
                    if (c == '\\') {
                        cout << '\\';
                        cout << '\\';
                        std::cout << c;
                    }
                    else
                        if (c == '\n') {
                            std::cout << '\\' << 'n';
                        }
                        else {
                            std::cout << c;
                        }
            }
        }

        string MapRenderer::ColorToText(const TransportsCatalogue::MapSetting& settings)
        {
            std::string str1;
            if (std::holds_alternative<std::string>(settings.underlayer_color)) {
                return std::get<std::string>(settings.underlayer_color);
            }

            if (std::holds_alternative<std::array<int, 3>>(settings.underlayer_color)) {
                std::string str = "rgb(";
                int i = 0;
                for (auto& color : std::get<std::array<int, 3>>(settings.underlayer_color)) {
                    if (i != 0) {
                        str += ",";
                    }
                    ++i;
                    str += std::to_string(color);
                }
                str += ")";
                return str;
            }
            if (std::holds_alternative<std::array<double, 4>>(settings.underlayer_color)) {
                std::string str = "rgba(";
                int i = 0;
                for (auto& color : std::get<std::array<double, 4>>(settings.underlayer_color)) {
                    if (i != 0) {
                        str += ",";
                    }
                    ++i;
                    std::ostringstream strs;
                    strs << color;
                    str += strs.str();

                }
                str += ")";
                return str;
            }
            return str1;
        }

        void MapRenderer::PrepareText(svg::Text& temp, bool zaliv, const std::string& busName, const std::string& color, const MapSetting& tempSetting)
        {
            temp.zaliv = zaliv;
            temp.SetData(busName);
            temp.SetFontFamily("Verdana");
            temp.SetFontWeight("bold");
            temp.SetFill(color);
            temp.SetStroke(color);
            temp.SetFontSize(tempSetting.bus_label_font_size);
            std::ostringstream strs1;
            strs1 << tempSetting.underlayer_width;
            temp.SetStroke_width(strs1.str());
            temp.SetOffset(svg::Point(tempSetting.bus_label_offset[0], tempSetting.bus_label_offset[1]));

        }
        void MapRenderer::PrepareTextStop(svg::Text& temp, bool zaliv, std::string busName, std::string color, const MapSetting& tempSetting, const Stop stop, const Plane::SphereProjector& proj)
        {
            temp.zaliv = zaliv;
            temp.SetData(busName);
            temp.SetFontFamily("Verdana");
            temp.SetFill(color);
            temp.SetStroke(color);
            temp.SetFontSize(tempSetting.stop_label_font_size);
            std::ostringstream strs1;
            strs1 << tempSetting.underlayer_width;
            temp.SetStroke_width(strs1.str());
            temp.SetOffset(svg::Point(tempSetting.stop_label_offset[0], tempSetting.stop_label_offset[1]));
            const svg::Point screen_coord = proj(TransportsCatalogue::detail::distant::Coordinates(stop.latitude, stop.longitude));
            temp.SetPosition(screen_coord);


        }
        void MapRenderer::PrepareTextCoordinatsNotRing(svg::Text& temp, const Bus& bus, const Plane::SphereProjector& proj, int lastStop, int stop_num)
        {

            for (auto& stop : bus.busStop) {
                if (stop_num == lastStop) {
                    const svg::Point screen_coord = proj(TransportsCatalogue::detail::distant::Coordinates(stop->latitude, stop->longitude));
                    temp.SetPosition(screen_coord);
                    break;
                }
                ++stop_num;
            }
        }
        
        void MapRenderer::MainPrepareText(svg::Text& temp, svg::Document& doc, bool zalivka, bool ring, const Bus& bus, const Plane::SphereProjector& proj, const MapSetting& tempSetting)
        {
            if (ring) {
                if (zalivka) {
                    PrepareText(temp, zalivka, bus.name, ColorToText(tempSetting), tempSetting); // true и false обозначает разницу подложка это или  надпись
                }
                else {
                    PrepareText(temp, zalivka, bus.name, busWithColor[bus.name], tempSetting);
                }
                PrepareTextCoordinatsRing(temp, bus, proj);

            }
            else {
                if (zalivka) {
                    PrepareText(temp, zalivka, bus.name, ColorToText(tempSetting), tempSetting);
                }
                else {
                    PrepareText(temp, zalivka, bus.name, busWithColor[bus.name], tempSetting);
                }
                int lastStop = bus.busStop.size() / 2;
                PrepareTextCoordinatsNotRing(temp, bus, proj, lastStop, 0);

            }
            doc.Add(temp);
        }

        void MapRenderer::PrepareTextCoordinatsRing(svg::Text& temp, const Bus& bus, const Plane::SphereProjector& proj)
        {
            for (auto& stop : bus.busStop) {
                svg::Point screen_coord = proj(TransportsCatalogue::detail::distant::Coordinates(stop->latitude, stop->longitude));
                temp.SetPosition(screen_coord);
                break;
            }
        }
        
        bool MapRenderer::FindDuplicate(Stop item, std::vector<Stop>& stops)const
        {
            for (auto& it : stops) {
                if (item.name == it.name) {
                    return true;
                }
            }
            return false;
        }
    }
}