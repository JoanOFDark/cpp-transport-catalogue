#include "transport_router.h"

using namespace std;

namespace transport_catalogue {
    template <typename Weight>
    struct RouteInfo {
        Weight weight;
        std::vector<graph::EdgeId> edges;
    };

    TransportRouter::TransportRouter(TransportCatalogue& tc) : catalogue_(tc) {}

    void TransportRouter::SetRouterSettings(JSONReader& reader)
    {
        router_settings_ = &reader;
    }

    void TransportRouter::PrepareEdges(vector<graph::Edge<double>>& edges, VertexId from, VertexId to, double weight) {
        graph::Edge<double> temp;
        temp.from = from;
        temp.to = to;
        temp.weight = weight;
        edges.push_back(temp);
    }

    void TransportRouter::Result(size_t from, size_t to, double& total_time, std::vector<InfoToPrintRoute>& res, graph::Router<double>& route)
    {
        if (route.BuildRoute(from, to).has_value()) {
            total_time = route.BuildRoute(from, to).value().weight;
            auto vect = route.BuildRoute(from, to).value().edges;

            for (size_t i = 0; i < vect.size(); ++i)
            {
                size_t from, to;
                from = graph_.GetEdge(vect[i]).from;
                to = graph_.GetEdge(vect[i]).to;

                InfoToPrintRoute tempW;
                tempW.type = "Wait";
                tempW.stop_name = stops_.at(from);
                tempW.time = router_settings_->GetRouterSetting().bus_wait_time;
                res.push_back(tempW);
                InfoToPrintRoute tempB;
                tempB.bus = edges_info_.at({ from,to }).name_bus;
                tempB.span_count = edges_info_.at({ from,to }).span_count;
                tempB.time = edges_info_.at({ from,to }).time;
                tempB.type = "Bus";
                res.push_back(tempB);
            }
        }
    }

    void TransportRouter::PrepareOneEdgeInfo(const string& bus, int span_count, double time, size_t from, size_t to) {
        InfoEdge edge;
        edge.name_bus = bus;
        edge.span_count = span_count + 1;
        edge.time = time;
        edges_info_[{from, to}] = edge;
    }

    void TransportRouter::PrepareStops() {
        size_t id_stop = 0;
        for (auto& stop : *catalogue_.GetStops()) {
            stop.id = id_stop;
            stops_[stop.id] = stop.name;
            ++id_stop;
        }
    }

    void TransportRouter::PrepareGraph()
    {

        double bus_velocity = router_settings_->GetRouterSetting().bus_velocity;
        double bus_wait_time = router_settings_->GetRouterSetting().bus_wait_time;
        double speedMetrs = bus_velocity * 1000 / 60;
        PrepareStops();

        for (const auto& bus : *catalogue_.GetBuses()) {

            if (bus.is_roundtrip) {
                for (size_t i = 0; i < bus.bus_stop.size() - 1; ++i) {
                    double distance = 0;
                    size_t k = i;
                    size_t spaunCount = 0;
                    for (size_t j = i + 1; j < bus.bus_stop.size(); ++j) {

                        size_t from, to;
                        from = bus.bus_stop[i]->id;
                        to = bus.bus_stop[j]->id;
                        if (from != to) {
                            distance += catalogue_.GetDistance(bus.bus_stop[k], bus.bus_stop[k + 1]);
                            PrepareEdges(edges_, from, to, distance / speedMetrs + bus_wait_time);
                            PrepareOneEdgeInfo(bus.name, spaunCount, distance / speedMetrs, from, to);
                            ++spaunCount;
                        }
                        ++k;
                    }
                }
            }
            else {

                for (size_t i = 0; i < bus.bus_stop.size() / 2; ++i) {
                    double distance = 0;
                    size_t k = i;
                    size_t spaunCount = 0;
                    for (size_t j = i + 1; j <= bus.bus_stop.size() / 2; ++j) {
                        size_t from, to;
                        from = bus.bus_stop[i]->id;
                        to = bus.bus_stop[j]->id;
                        if (from != to) {
                            distance += catalogue_.GetDistance(bus.bus_stop[k], bus.bus_stop[k + 1]);
                            PrepareEdges(edges_, bus.bus_stop[i]->id, bus.bus_stop[j]->id, distance / speedMetrs + bus_wait_time);
                            PrepareOneEdgeInfo(bus.name, spaunCount, distance / speedMetrs, from, to);
                            ++spaunCount;
                        }
                        ++k;
                    }
                }

                for (size_t i = bus.bus_stop.size() / 2; i < bus.bus_stop.size(); ++i) {
                    double distance = 0;
                    size_t k = i;
                    size_t spaunCount = 0;
                    for (size_t j = i + 1; j < bus.bus_stop.size(); ++j) {
                        size_t from, to;
                        from = bus.bus_stop[i]->id;
                        to = bus.bus_stop[j]->id;
                        if (from != to) {
                            distance += catalogue_.GetDistance(bus.bus_stop[k], bus.bus_stop[k + 1]);
                            PrepareEdges(edges_, bus.bus_stop[i]->id, bus.bus_stop[j]->id, distance / speedMetrs + bus_wait_time);
                            PrepareOneEdgeInfo(bus.name, spaunCount, distance / speedMetrs, from, to);
                            ++spaunCount;
                        }
                        ++k;
                    }
                }
            }
        }
        graph::DirectedWeightedGraph<double> Graf(catalogue_.GetStops()->size());
        for (auto& it : edges_) {
            Graf.AddEdge(it);
        }
        graph_ = std::move(Graf);

    }

    graph::DirectedWeightedGraph<double>& TransportRouter::ReturnGraph()
    {
        return graph_;
    }
}