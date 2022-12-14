#pragma once

#include "geo.h"
#include "svg.h"

#include <iostream>
#include <deque>
#include <unordered_map>
#include <vector>
#include <array>
#include <unordered_set>
#include <set>
#include <map>
#include <variant>

namespace transport_catalogue {

    using color = std::variant<std::string, std::array<int, 3>, std::array<double, 4>>;

    struct InfoEdge {
        std::string name_bus;
        int span_count;
        double time;
    };

    struct RoutingSettings {
        int bus_wait_time;
        double bus_velocity;
    };
    struct Stop {
        size_t id;
        std::string name;
        double latitude;
        double longitude;
        svg::Point screen_coord;
        std::map<std::string, double> road_distances;
    };



    struct InfoToPrintStop {
        std::vector<std::string_view> buss;
        bool stop_exist = false;
    };

    struct StopToStop {
        StopToStop(Stop* A, Stop* B)
        {
            AtoB.first = A;
            AtoB.second = B;
        }

        bool operator==(StopToStop A)const {
            return AtoB == A.AtoB;
        }

        bool operator!=(StopToStop A)const {
            return AtoB != A.AtoB;
        }
        std::pair<Stop*, Stop*>AtoB;
    };

    struct Stats {
        Stats() = default;
        int stops{};
        int unique_stops{};
        double route_length{};
        double route_length2{};
    };

    struct Requests {
        std::vector<std::pair<std::string, bool>> requests;
    };

    struct StatRequest {
        int id;
        std::string type;
        std::string name;
        std::string from;
        std::string to;
    };

    struct InfoToPrintRoute {
        int span_count;
        double time;
        std::string type;
        std::string stop_name;
        std::string bus;

    };

    struct Bus {
        std::string name;
        std::vector<Stop*>bus_stop;
        bool is_roundtrip;
    };
    struct MapSettings {

        double width;
        double height;
        double padding;
        double line_width;
        double stop_radius;
        int bus_label_font_size;
        std::array<double, 2> bus_label_offset;
        int stop_label_font_size;
        std::array<double, 2> stop_label_offset;
        color underlayer_color;
        double underlayer_width;
        std::vector<color> color_palette;
    };
}