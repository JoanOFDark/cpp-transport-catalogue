#include "json_reader.h"
#include <string>

namespace TransportsCatalogue {

    void jsonReader::PrepairJSON(std::istream& streamDate)
    {
        Doc = json::Load(streamDate);
        FillCatalogy();
    }

    void jsonReader::GetCatalog(TransportCatalogue& primary)
    {
        Temp = &primary;
    }

    void jsonReader::GetInfoBus(const json::Node& Businf)
    {
        TransportsCatalogue::Bus bus;
        std::list<std::string> revers;
        json::Dict temp = Businf.AsMap();
        bool round_trip = temp.at("is_roundtrip").AsBool();

        bus.name = temp.at("name").AsString();

        for (auto& i : temp.at("stops").AsArray()) {
            std::string t = i.AsString();
            revers.push_front(t);
            bus.busStop.push_back(Temp->FindStop(i.AsString()));
        }
        int i = 0;
        if (!round_trip) {
            for (auto& it : revers) {
                if (i > 0) {
                    bus.busStop.push_back(Temp->FindStop(it));
                }
                i++;
            }

        }

        bus.is_roundtrip = temp.at("is_roundtrip").AsBool();
        Temp->GetBuses()->push_back(bus);

    }

    void jsonReader::GetInfoStop(const json::Node& StopInf)
    {
        TransportsCatalogue::Stop stop;
        json::Dict temp = StopInf.AsMap();

        stop.name = temp.at("name").AsString();
        stop.latitude = temp.at("latitude").AsDouble();
        stop.longitude = temp.at("longitude").AsDouble();

        for (auto& item : temp.at("road_distances").AsMap()) {
            stop.road_distances[item.first] = item.second.AsDouble();
        }
        Temp->GetStops()->push_back(stop);

    }

    void jsonReader::GetReqInf(const json::Node& ReqItem)
    {
        json::Dict temp = ReqItem.AsMap();
        StatRequest item;
        item.id = temp.at("id").AsInt();
        item.type = temp.at("type").AsString();
        if (temp.count("name") != 0)
            item.name = temp.at("name").AsString();
        ReqInf.push_back(item);
    }

    void jsonReader::GetMapInfo(const json::Node& MapItem)
    {
        json::Dict temp = MapItem.AsMap();
        Mapset.width = temp.at("width").AsDouble();
        Mapset.height = temp.at("height").AsDouble();
        Mapset.padding = temp.at("padding").AsDouble();
        Mapset.stop_radius = temp.at("stop_radius").AsDouble();
        Mapset.line_width = temp.at("line_width").AsDouble();
        Mapset.bus_label_font_size = temp.at("bus_label_font_size").AsInt();
        Mapset.stop_label_font_size = temp.at("stop_label_font_size").AsInt();

        {
            int i = 0;
            for (auto& it : temp.at("bus_label_offset").AsArray()) {
                Mapset.bus_label_offset[i] = it.AsDouble();
                ++i;
            }
            i = 0;
            for (auto& it : temp.at("stop_label_offset").AsArray()) {
                Mapset.stop_label_offset[i] = it.AsDouble();
                ++i;
            }
        }

        {
            if (temp.at("underlayer_color").IsArray()) {
                if (temp.at("underlayer_color").AsArray().size() == 4) {
                    std::array<double, 4>te;
                    int i = 0;
                    for (auto& it : temp.at("underlayer_color").AsArray()) {
                        te[i] = it.AsDouble();
                        ++i;
                    }
                    Mapset.underlayer_color = te;

                }
                if (temp.at("underlayer_color").AsArray().size() == 3) {
                    std::array<int, 3>te;
                    int i = 0;
                    for (auto& it : temp.at("underlayer_color").AsArray()) {
                        te[i] = it.AsInt();
                        ++i;
                    }
                    Mapset.underlayer_color = te;
                }
            }

            if (temp.at("underlayer_color").IsString()) {
                Mapset.underlayer_color = temp.at("underlayer_color").AsString();
            }
        }

        Mapset.underlayer_width = temp.at("underlayer_width").AsDouble();

        for (auto& it : temp.at("color_palette").AsArray()) {
            if (it.IsString()) {
                Mapset.color_palette.push_back(it.AsString());
            }
            if (it.IsArray()) {
                if (it.AsArray().size() == 3) {
                    std::array<int, 3>to;
                    int i = 0;
                    for (auto& item : it.AsArray()) {
                        to[i] = item.AsInt();
                        ++i;
                    }
                    Mapset.color_palette.push_back(to);
                }
                if (it.AsArray().size() == 4) {
                    std::array<double, 4>to;
                    int i = 0;
                    for (auto& item : it.AsArray()) {
                        to[i] = item.AsDouble();
                        ++i;
                    }
                    Mapset.color_palette.push_back(to);
                }
            }

        }

    }

    void jsonReader::FillCatalogy()
    {
        json::NodeJson temp = Doc.GetRoot().GetData();
        json::Dict inf = Doc.GetRoot().AsMap();
        std::vector<json::Node>requestBus;
        for (auto& a : inf) {
            if (a.first == "base_requests")
            {
                if (a.second.IsArray())
                    for (auto& b : a.second.AsArray()) {
                        if (b.IsMap()) {
                            for (auto& infoReq : b.AsMap()) {
                                if (infoReq.first == "type" && infoReq.second.AsString() == "Stop") {
                                    GetInfoStop(b);
                                }
                                if (infoReq.first == "type" && infoReq.second.AsString() == "Bus") {
                                    requestBus.push_back(b);
                                }
                            }
                        }
                    }
            }
            if (a.first == "stat_requests") {
                if (a.second.IsArray()) {
                    for (auto& b : a.second.AsArray()) {
                        if (b.IsMap()) {
                            GetReqInf(b);
                        }
                    }
                }

            }
            if (a.first == "render_settings") {
                if (a.second.IsMap()) {
                    GetMapInfo(a.second);
                }
            }
        }
        Temp->PrepareStops();
        for (auto& item : requestBus) {
            GetInfoBus(item);
        }

        Temp->PrepareBus();
        Temp->PrepareDistance();
        Temp->PrepareStopsWithBus();

    }

    MapSettings& jsonReader::GetSetting()
    {
        return Mapset;
    }

    void jsonReader::PrintInfoStop(InfoToPrintStop item, const std::string& request_id)
    {
        if (item.stop_exist) {
            std::cout << "    {" << std::endl << "        \""
                << "buses" << "\"" << ": " << "[";

            int i = 0;
            for (auto& it : item.buss) {
                if (i > 0) {
                    std::cout << ", \"" << it << "\"";

                }
                else {
                    std::cout << "\"" << it << "\"";
                }
                i++;

            }
            std::cout << "]" << "," << std::endl << "        \""
                << "request_id" << "\"" << ": " << request_id << std::endl << "    }";
        }
        else {
            std::cout << "    {" << std::endl << "        \""
                << "request_id" << "\"" << ": " << request_id << "," << std::endl << "        \""
                << "error_message" << "\"" << ": " << "\"" << "not found" << "\"" << std::endl << "    }";

        }
    }

    void jsonReader::PrintInfoBus(Stats item, const std::string& request_id)
    {
        if (item.route_length == 0 && item.route_length2 == 0 && item.stops == 0 && item.unique_stops == 0) {
            std::cout << "    {" << std::endl << "        \"" << "request_id" << "\"" << ": " << request_id << ","
                << std::endl << "        \"" << "error_message" << "\"" << ": " << "\""
                << "not found" << "\"" << std::endl << "    }";

        }
        else {
            std::cout << "    {" << std::endl << "        \"" << "curvature" << "\"" << ": " << item.route_length2
                << "," << std::endl << "        \"" << "request_id" << "\"" << ": " << request_id
                << "," << std::endl << "        \"" << "route_length" << "\"" << ": "
                << item.route_length << ","
                << std::endl << "        \"" << "stop_count" << "\"" << ": "
                << item.stops << "," << std::endl << "        \""
                << "unique_stop_count" << "\"" << ": " << item.unique_stops << std::endl << "    }";
        }

    }

    std::vector<StatRequest> jsonReader::GetReqInf()
    {
        return ReqInf;
    }
}