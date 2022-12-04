﻿#include "geo.h"
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

    TransportsCatalogue::JSONReader GetInf;
    TransportsCatalogue::TransportCatalogue A;
    TransportsCatalogue::renderer::MapRenderer Map(A);
    Map.SetMapSetting(GetInf);
    GetInf.GetCatalogue(A);
    GetInf.PrepareJSON(std::cin);
    TransportsCatalogue::RequestHandler Manager(Map, GetInf, A);
    Manager.ExecuteRequests();

    return 0;
}