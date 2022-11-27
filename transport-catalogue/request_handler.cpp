#include "request_handler.h"

using namespace std;


TransportsCatalogue::RequestHandler::RequestHandler(renderer::MapRenderer& Renderer, TransportsCatalogue::JSONReader& Requests, TransportCatalogue& Catalogue)
    :Renderer_(Renderer), Requests_(Requests), Catalogue_(Catalogue)
{

}

void TransportsCatalogue::RequestHandler::ExecuteRequests(std::ostream& output)
{

    int i = 0;
    if (Requests_.GetReqInf().size() != 0)
    {
        output << "[" << std::endl;

        for (auto& item : Requests_.GetReqInf())
        {
            if (i > 0) { output << "," << std::endl;; }
            ++i;
            if (item.type == "Map") {
                output << "    {" << endl;

                output << "        \"" << "map" << "\": " << "\"";
                Renderer_.GetMap();
                output << "\"," << std::endl;
                output << "        \"" << "request_id" << "\": " << std::to_string(item.id) << endl;
                output << "    }";

            }

            if (item.type == "Stop") {
                Requests_.PrintInfoStop(Catalogue_.GetStopInfo(item.name), std::to_string(item.id));
            }
            if (item.type == "Bus") {
                Requests_.PrintInfoBus(Catalogue_.GetBusInfo(item.name), std::to_string((item.id)));
            }
        }
        output << std::endl << "]";
    }
}