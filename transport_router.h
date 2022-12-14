#pragma once

#include "geo.h"
#include "domain.h"
#include "ranges.h"
#include "graph.h"
#include "router.h"
#include "json_reader.h"
#include "transport_catalogue.h"

#include <iostream>
#include <deque>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <set>
#include <iterator>
#include <memory>

namespace transport_catalogue {

    using VertexId = size_t;
    using EdgeId = size_t;

    class TransportRouter {
    public:
        TransportRouter(TransportCatalogue& tc);
        void SetRouterSettings(JSONReader& router_settings);
        void PrepareGraph();
        graph::DirectedWeightedGraph<double>& ReturnGraph();

        void PrepareEdges(std::vector<graph::Edge<double>>& edges, VertexId from, VertexId to, double weight);
        void Result(size_t from, size_t to, double& total_time, std::vector<InfoToPrintRoute>& res, graph::Router<double>& route);
        void PrepareOneEdgeInfo(const std::string& bus, int span_count, double time, size_t from, size_t to);
        void PrepareStops();

    private:
        JSONReader* router_settings_;
        TransportCatalogue& catalogue_;
        std::map<std::pair<size_t, size_t>, InfoEdge> edges_info_;
        std::map<size_t, std::string> stops_;

        std::vector<graph::Edge<double>> edges_;
        graph::DirectedWeightedGraph<double> graph_;

    };

}