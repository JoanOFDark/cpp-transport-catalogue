#pragma once

#include "json.h"
#include "domain.h"
#include "transport_catalogue.h"
#include "svg.h"
#include "map_renderer.h"
#include "iostream"
#include "json_reader.h"

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <iomanip>
#include <fstream>


namespace TransportsCatalogue {
    class RequestHandler {
    public:
        RequestHandler(renderer::MapRenderer& Renderer, JSONReader& Requests, TransportCatalogue& Catalogue_);
        void ExecuteRequests(std::ostream& output);

    private:
        renderer::MapRenderer& Renderer_;
        JSONReader& Requests_;
        TransportCatalogue& Catalogue_;
    };
}