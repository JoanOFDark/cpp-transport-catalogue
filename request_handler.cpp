#include "request_handler.h"

using namespace std;

transport_catalogue::RequestHandler::RequestHandler(renderer::MapRenderer& renderer, transport_catalogue::JSONReader& requests, TransportCatalogue& catalogue, TransportRouter& router)
    :renderer_(renderer), requests_(requests), catalogue_(catalogue), transport_router_(router), router_(transport_router_.ReturnGraph())
{}

void transport_catalogue::RequestHandler::ExecuteRequests()
{
    json::Array arr;
    if (requests_.GetRequestInfo().size() != 0)
    {
        for (auto& item : requests_.GetRequestInfo())
        {
            if (item.type == "Map") {
                json::Dict tem;
                tem["map"] = renderer_.GetMap();
                tem["request_id"] = item.id;
                arr.push_back(tem);
            }
            if (item.type == "Stop") {
                InfoToPrintStop temp_info = catalogue_.GetStopInfo(item.name);
                if (temp_info.stop_exist) {
                    json::Dict tem;
                    json::Array busss;
                    for (auto& it : temp_info.buss) {
                        busss.push_back(string(it));
                    }
                    tem["buses"] = busss;
                    tem["request_id"] = item.id;
                    arr.push_back(tem);

                }
                else {
                    json::Dict tem;
                    tem["request_id"] = item.id;
                    tem["error_message"] = "not found";
                    arr.push_back(tem);
                }

            }
            if (item.type == "Bus") {
                Stats temp_info = catalogue_.GetBusInfo(item.name);
                if (temp_info.route_length == 0 && temp_info.route_length2 == 0 && temp_info.stops == 0 && temp_info.unique_stops == 0) {
                    json::Dict tem;
                    tem["request_id"] = item.id;
                    tem["error_message"] = "not found";
                    arr.push_back(tem);

                }
                else {
                    json::Dict tem;
                    tem["curvature"] = temp_info.route_length2;
                    tem["request_id"] = item.id;
                    tem["route_length"] = temp_info.route_length;
                    tem["stop_count"] = temp_info.stops;
                    tem["unique_stop_count"] = temp_info.unique_stops;
                    arr.push_back(tem);

                }

            }
            if (item.type == "Route") {
                json::Dict items;
                json::Array itemsIntoItems;
                std::vector<InfoToPrintRoute> res;
                size_t from, to;

                from = catalogue_.GetStopNameToStop()->at(item.from)->id;
                to = catalogue_.GetStopNameToStop()->at(item.to)->id;
                double weit_total = 0;

                transport_router_.SetRouter(router_);
                transport_router_.Result(from, to, weit_total, res);

                if (from == to) {
                    items["items"] = itemsIntoItems;
                    items["request_id"] = item.id;
                    items["total_time"] = weit_total;
                    arr.push_back(items);

                }
                else if (res.size() == 0) {
                    std::string str;
                    str = "not found";
                    items["error_message"] = str;
                    items["request_id"] = item.id;
                    arr.push_back(items);
                }
                else {
                    for (auto& infoIt : res) {
                        if (infoIt.type == "Wait") {
                            json::Dict info;
                            info["stop_name"] = infoIt.stop_name;
                            info["time"] = infoIt.time;
                            info["type"] = infoIt.type;
                            itemsIntoItems.push_back(info);

                        }
                        if (infoIt.type == "Bus") {
                            json::Dict info;
                            info["bus"] = infoIt.bus;
                            info["span_count"] = infoIt.span_count;
                            info["time"] = infoIt.time;
                            info["type"] = infoIt.type;
                            itemsIntoItems.push_back(info);
                        }
                    }

                    items["items"] = itemsIntoItems;
                    items["request_id"] = item.id;
                    items["total_time"] = weit_total;
                    arr.push_back(items);
                }
            }
        }
        json::Print(
            json::Document{
                json::Builder{}.Value(arr).Build()
            },
            cout
        );

    }
}