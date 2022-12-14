#include "json_reader.h"

#include <string>

namespace transport_catalogue {

    void JSONReader::PrepareJson(std::istream& stream)
    {
        document_ = json::Load(stream);
        FillCatalogue();
    }

    void JSONReader::GetCatalogue(TransportCatalogue& primary)
    {
        catalogue_ = &primary;
    }

    void JSONReader::GetInfoBus(const json::Node& bus_info)
    {
        transport_catalogue::Bus bus;
        std::list<std::string> revers;
        json::Dict temp = bus_info.AsMap();
        bool round_trip = temp.at("is_roundtrip").AsBool();

        bus.name = temp.at("name").AsString();

        for (auto& i : temp.at("stops").AsArray()) {
            std::string t = i.AsString();
            revers.push_front(t);
            bus.bus_stop.push_back(catalogue_->FindStop(i.AsString()));
        }
        int i = 0;
        if (!round_trip) {
            for (auto& it : revers) {
                if (i > 0) {
                    bus.bus_stop.push_back(catalogue_->FindStop(it));
                }
                i++;
            }

        }

        bus.is_roundtrip = temp.at("is_roundtrip").AsBool();
        catalogue_->GetBuses()->push_back(bus);

    }

    void JSONReader::GetInfoStop(const json::Node& stop_info)
    {
        transport_catalogue::Stop stop;
        json::Dict temp = stop_info.AsMap();

        stop.name = temp.at("name").AsString();
        stop.latitude = temp.at("latitude").AsDouble();
        stop.longitude = temp.at("longitude").AsDouble();

        for (auto& item : temp.at("road_distances").AsMap()) {
            stop.road_distances[item.first] = item.second.AsDouble();
        }
        catalogue_->GetStops()->push_back(stop);

    }

    void JSONReader::GetRequestInfo(const json::Node& request_item)
    {
        json::Dict temp = request_item.AsMap();
        StatRequest item;
        item.id = temp.at("id").AsInt();
        item.type = temp.at("type").AsString();
        if (temp.count("name") != 0)
            item.name = temp.at("name").AsString();

        if (temp.at("type").AsString() == "Route") {
            item.from = temp.at("from").AsString();
            item.to = temp.at("to").AsString();
        }
        requests_.push_back(item);
    }

    void JSONReader::GetMapInfo(const json::Node& map_item)
    {
        json::Dict temp = map_item.AsMap();
        map_settings_.width = temp.at("width").AsDouble();
        map_settings_.height = temp.at("height").AsDouble();
        map_settings_.padding = temp.at("padding").AsDouble();
        map_settings_.stop_radius = temp.at("stop_radius").AsDouble();
        map_settings_.line_width = temp.at("line_width").AsDouble();
        map_settings_.bus_label_font_size = temp.at("bus_label_font_size").AsInt();
        map_settings_.stop_label_font_size = temp.at("stop_label_font_size").AsInt();

        {
            int i = 0;
            for (auto& it : temp.at("bus_label_offset").AsArray()) {
                map_settings_.bus_label_offset[i] = it.AsDouble();
                ++i;
            }
            i = 0;
            for (auto& it : temp.at("stop_label_offset").AsArray()) {
                map_settings_.stop_label_offset[i] = it.AsDouble();
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
                    map_settings_.underlayer_color = te;

                }
                if (temp.at("underlayer_color").AsArray().size() == 3) {
                    std::array<int, 3>te;
                    int i = 0;
                    for (auto& it : temp.at("underlayer_color").AsArray()) {
                        te[i] = it.AsInt();
                        ++i;
                    }
                    map_settings_.underlayer_color = te;
                }
            }

            if (temp.at("underlayer_color").IsString()) {
                map_settings_.underlayer_color = temp.at("underlayer_color").AsString();
            }
        }

        map_settings_.underlayer_width = temp.at("underlayer_width").AsDouble();

        for (auto& it : temp.at("color_palette").AsArray()) {
            if (it.IsString()) {
                map_settings_.color_palette.push_back(it.AsString());
            }
            if (it.IsArray()) {
                if (it.AsArray().size() == 3) {
                    std::array<int, 3>to;
                    int i = 0;
                    for (auto& item : it.AsArray()) {
                        to[i] = item.AsInt();
                        ++i;
                    }
                    map_settings_.color_palette.push_back(to);
                }
                if (it.AsArray().size() == 4) {
                    std::array<double, 4>to;
                    int i = 0;
                    for (auto& item : it.AsArray()) {
                        to[i] = item.AsDouble();
                        ++i;
                    }
                    map_settings_.color_palette.push_back(to);
                }
            }

        }

    }

    void JSONReader::GetRouterInfo(const json::Node& router_item)
    {
        json::Dict temp = router_item.AsMap();
        router_settings_.bus_velocity = temp.at("bus_velocity").AsDouble();
        router_settings_.bus_wait_time = temp.at("bus_wait_time").AsDouble();
    }

    void JSONReader::FillCatalogue()
    {
        // json::Node::Value temp= document_.GetRoot().GetValue();
        json::Dict inf = document_.GetRoot().AsMap();
        std::vector<json::Node>requestBus;
        for (auto& a : inf) {
            if (a.first == "base_requests")
            {
                if (a.second.IsArray())
                    for (auto& b : a.second.AsArray()) {
                        if (b.IsMap()) {
                            for (auto& request : b.AsMap()) {
                                if (request.first == "type" && request.second.AsString() == "Stop") {
                                    GetInfoStop(b);
                                }
                                if (request.first == "type" && request.second.AsString() == "Bus") {
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
                            GetRequestInfo(b);
                        }
                    }
                }

            }
            if (a.first == "render_settings") {
                if (a.second.IsMap()) {
                    GetMapInfo(a.second);
                }
            }
            if (a.first == "routing_settings") {
                if (a.second.IsMap()) {
                    GetRouterInfo(a.second);
                }
            }
        }
        catalogue_->PrepareStops();
        for (auto& item : requestBus) {
            GetInfoBus(item);
        }

        catalogue_->PrepareBus();
        catalogue_->PrepareDistance();
        catalogue_->PrepareStopsWithBus();

    }

    MapSettings& JSONReader::GetSetting()
    {
        return map_settings_;
    }

    RoutingSettings& JSONReader::GetRouterSetting()
    {
        return router_settings_;
    }

    void JSONReader::PrintInfoStop(InfoToPrintStop item, std::string request_id)
    {
        if (item.stop_exist) {
            std::cout << "    {" << std::endl << "        \""
                << "buses" << "\"" << ": " << "[" << std::endl;

            int i = 0;
            std::cout << "            ";
            for (auto& it : item.buss) {
                if (i > 0) {
                    std::cout << ", \"" << it << "\"";

                }
                else {
                    std::cout << "\"" << it << "\"";
                }
                i++;

            }
            std::cout << std::endl;
            std::cout << "        ]" << "," << std::endl << "        \""
                << "request_id" << "\"" << ": " << request_id << std::endl << "    }";
        }
        else {
            std::cout << "    {" << std::endl << "        \""
                << "request_id" << "\"" << ": " << request_id << "," << std::endl << "        \""
                << "error_message" << "\"" << ": " << "\"" << "not found" << "\"" << std::endl << "    }";

        }
    }

    void JSONReader::PrintInfoBus(Stats item, std::string request_id)
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

    std::vector<StatRequest> JSONReader::GetRequestInfo()
    {
        return requests_;
    }
}