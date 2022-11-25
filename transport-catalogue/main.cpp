#include "geo.h"
#include "json_reader.h"
#include "request_handler.h"
#include "domain.h"
#include "map_renderer.h"

#include <iostream>
#include <deque>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <string>

using namespace std;

int main() {

    TransportsCatalogue::JsonReader GetInf;
    TransportsCatalogue::TransportCatalogue A;
    TransportsCatalogue::RequestHandler Manager(A);

    GetInf.GetCatalog(A);
    GetInf.PrepairJson(std::cin);
    Manager.SetMapSetting(GetInf);

    int i = 0;
    if (GetInf.GetReqInf().size() != 0)
    {
        std::cout << "[" << std::endl;

        for (auto& item : GetInf.GetReqInf())
        {
            if (i > 0) { std::cout << "," << std::endl;; }
            ++i;
            if (item.type == "Map") {
                std::cout << "    {" << endl;

                std::cout << "        \"" << "map" << "\": " << "\"";
                Manager.GetTraec();
                std::cout << "\"," << std::endl;
                std::cout << "        \"" << "request_id" << "\": " << std::to_string(item.id) << endl;
                std::cout << "    }";

            }

            if (item.type == "Stop") {
                GetInf.PrintInfoStop(A.GetStopInfo(item.name), std::to_string(item.id));
            }
            if (item.type == "Bus") {
                GetInf.PrintInfoBus(A.GetBusInfo(item.name), std::to_string((item.id)));
            }
        }
        std::cout << std::endl << "]";
    }
}