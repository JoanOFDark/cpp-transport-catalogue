#include "request_handler.h"

using namespace std;


TransportsCatalogue::RequestHandler::RequestHandler(renderer::MapRenderer& renderer, TransportsCatalogue::JSONReader& requests, TransportCatalogue& catalogue)
    :renderer_(renderer), requests_(requests), catalogue_(catalogue)
{

}

void TransportsCatalogue::RequestHandler::ExecuteRequests()
{
    json::Array arr;
    if (requests_.GetReqInf().size() != 0)
    {
        for (auto& item : requests_.GetReqInf())
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
        }
        json::Print(
            json::Document{
                // Форматирование не имеет формального значения:
                // это просто цепочка вызовов методов
                json::Builder{}.Value(arr).Build()
            },
            cout
        );

    }

}