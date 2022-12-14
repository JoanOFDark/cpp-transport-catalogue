#include "geo.h"
#include "json_reader.h"
#include "request_handler.h"
#include "domain.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "router.h"
#include "graph.h"

#include <iostream>
#include <deque>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <string>


using namespace std;

int main() {

    transport_catalogue::JSONReader reader;
    transport_catalogue::TransportCatalogue catalogue;
    transport_catalogue::renderer::MapRenderer map_renderer(catalogue);
    map_renderer.SetMapSetting(reader);
    reader.GetCatalogue(catalogue);
    reader.PrepareJson(std::cin);

    transport_catalogue::TransportRouter router(catalogue);
    router.SetRouterSettings(reader);
    router.PrepareGraph();
    transport_catalogue::RequestHandler manager(map_renderer, reader, catalogue, router);
    manager.ExecuteRequests();
    return 0;
}