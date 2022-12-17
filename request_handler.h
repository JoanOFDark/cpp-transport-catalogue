#pragma once

#include "json.h"
#include "domain.h"
#include "transport_catalogue.h"
#include "svg.h"
#include "map_renderer.h"
#include "json_reader.h"
#include "json_builder.h"
#include "transport_router.h"
#include "graph.h"
#include "router.h"

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <iomanip>
#include <fstream>



namespace transport_catalogue {
    class RequestHandler {
    public:
        RequestHandler(renderer::MapRenderer& renderer, JSONReader& requests,
            TransportCatalogue& catalogue, TransportRouter& router);
        void ExecuteRequests();

    private:
        renderer::MapRenderer& renderer_;
        JSONReader& requests_;
        TransportCatalogue& catalogue_;
        TransportRouter& transport_router_;
        graph::Router<double> router_;
    };
}