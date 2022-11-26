#include "request_handler.h"

using namespace std;


TransportsCatalogue::RequestHandler::RequestHandler(renderer::MapRenderer& Renderer, TransportsCatalogue::jsonReader& Requests, TransportCatalogue& Catalogue)
    :Renderer_(Renderer), Requests_(Requests), Catalogue_(Catalogue)
{

}

void TransportsCatalogue::RequestHandler::OutputResult()
{

    int i = 0;
    if (Requests_.GetReqInf().size() != 0)
    {
        std::cout << "[" << std::endl;

        for (auto& item : Requests_.GetReqInf())
        {
            if (i > 0) { std::cout << "," << std::endl;; }
            ++i;
            if (item.type == "Map") {
                std::cout << "    {" << endl;

                std::cout << "        \"" << "map" << "\": " << "\"";
                Renderer_.GetMap();
                std::cout << "\"," << std::endl;
                std::cout << "        \"" << "request_id" << "\": " << std::to_string(item.id) << endl;
                std::cout << "    }";

            }

            if (item.type == "Stop") {
                Requests_.PrintInfoStop(Catalogue_.GetStopInfo(item.name), std::to_string(item.id));
            }
            if (item.type == "Bus") {
                Requests_.PrintInfoBus(Catalogue_.GetBusInfo(item.name), std::to_string((item.id)));
            }
        }
        std::cout << std::endl << "]";
    }
}