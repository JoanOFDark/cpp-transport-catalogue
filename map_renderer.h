#pragma once

#include "geo.h"
#include "svg.h"
#include "transport_catalogue.h"
#include "json_reader.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>
#include <limits>
#include <iomanip>
#include <fstream>

namespace transport_catalogue {
    namespace Plane {
        inline const double EPSILON = 1e-6;

        inline bool IsZero(double value) {
            return std::abs(value) < EPSILON;
        }

        class SphereProjector {
        public:
            template <typename PointInputIt>
            SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                double max_width, double max_height, double padding)
                : padding_(padding) //
            {
                // Если точки поверхности сферы не заданы, вычислять нечего
                if (points_begin == points_end) {
                    return;
                }

                // Находим точки с минимальной и максимальной долготой
                const auto [left_it, right_it] = std::minmax_element(
                    points_begin, points_end,
                    [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
                min_lon_ = left_it->lng;
                const double max_lon = right_it->lng;

                // Находим точки с минимальной и максимальной широтой
                const auto [bottom_it, top_it] = std::minmax_element(
                    points_begin, points_end,
                    [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
                const double min_lat = bottom_it->lat;
                max_lat_ = top_it->lat;

                // Вычисляем коэффициент масштабирования вдоль координаты x
                std::optional<double> width_zoom;
                if (!IsZero(max_lon - min_lon_)) {
                    width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
                }

                // Вычисляем коэффициент масштабирования вдоль координаты y
                std::optional<double> height_zoom;
                if (!IsZero(max_lat_ - min_lat)) {
                    height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
                }

                if (width_zoom && height_zoom) {
                    // Коэффициенты масштабирования по ширине и высоте ненулевые,
                    // берём минимальный из них
                    zoom_coeff_ = std::min(*width_zoom, *height_zoom);
                }
                else if (width_zoom) {
                    // Коэффициент масштабирования по ширине ненулевой, используем его
                    zoom_coeff_ = *width_zoom;
                }
                else if (height_zoom) {
                    // Коэффициент масштабирования по высоте ненулевой, используем его
                    zoom_coeff_ = *height_zoom;
                }
            }

            // Проецирует широту и долготу в координаты внутри SVG-изображения
            svg::Point operator()(transport_catalogue::detail::distant::Coordinates coords) const {
                return {
                    (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                    (max_lat_ - coords.lat) * zoom_coeff_ + padding_
                };
            }

        private:
            double padding_;
            double min_lon_ = 0;
            double max_lat_ = 0;
            double zoom_coeff_ = 0;
        };
    }
    namespace renderer {

        class MapRenderer {
        public:
            MapRenderer(TransportCatalogue& db);
            void SetMapSetting(JSONReader& json_reader);
            void FillPolyline(svg::Polyline& route, JSONReader* MapSet, int colorPaletIndex, std::string& color);
            std::string GetMap();
            void FillText(svg::Document& doc, std::deque<Bus>& orderBus, const transport_catalogue::Plane::SphereProjector& proj);
            void FillCircle(std::vector<Stop>& stops, svg::Document& doc, const Plane::SphereProjector& proj) const;
            std::string ColorToText(const MapSettings& settings);
            void  PrepareText(svg::Text& temp, bool zaliv, const std::string& busName, const std::string& color, const MapSettings& tempSetting);
            void  PrepareTextCoordinatsRing(svg::Text& temp, const Bus& bus, const Plane::SphereProjector& proj);
            bool  FindDuplicate(Stop item, std::vector<Stop>& stops)const;
            void  PrepareTextCoordinatsNotRing(svg::Text& temp, const Bus& bus, const Plane::SphereProjector& proj, int lastStop, int stop_num);
            void  MainPrepareText(svg::Text& temp, svg::Document& doc, bool zalivka, bool ring, const Bus& bus, const Plane::SphereProjector& proj, const MapSettings& tempSetting);
            void  FillTextStop(std::vector<Stop>& stops, svg::Document& doc, const Plane::SphereProjector& proj);
            void  PrepareTextStop(svg::Text& temp, bool zaliv, std::string busName, std::string color, const MapSettings& tempSetting, const Stop stop, const Plane::SphereProjector& proj);
            std::string PrintMap(std::string& str);

        private:
            TransportCatalogue& catalogue_;
            JSONReader* MapSet;
            std::map<std::string, std::string> busWithColor;
        };

    }
}