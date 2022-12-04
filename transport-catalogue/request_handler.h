#pragma once

#include "json.h"
#include "domain.h"
#include "transport_catalogue.h"
#include "svg.h"
#include "map_renderer.h"
#include "iostream"
#include "json_reader.h"
#include "json_builder.h"

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <iomanip>
#include <fstream>


namespace TransportsCatalogue {
    class RequestHandler {
    public:
        RequestHandler(renderer::MapRenderer& renderer, JSONReader& requests, TransportCatalogue& catalogue_);
        void ExecuteRequests();

    private:
        renderer::MapRenderer& renderer_;
        JSONReader& requests_;
        TransportCatalogue& catalogue_;
    };
}