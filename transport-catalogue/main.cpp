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

    TransportsCatalogue::jsonReader GetInf;
    TransportsCatalogue::TransportCatalogue A;
    TransportsCatalogue::renderer::MapRenderer Map(A);
    Map.SetMapSetting(GetInf);
    GetInf.GetCatalog(A);
    GetInf.PrepairJSON(std::cin);
    TransportsCatalogue::RequestHandler Manager(Map, GetInf, A);
    Manager.ExecuteRequests(std::cout);

    return 0;
}