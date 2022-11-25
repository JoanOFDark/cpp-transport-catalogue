#pragma once 
#include <cmath> 

namespace TransportsCatalogue {
    namespace detail {
        namespace distant {

            struct Coordinates {
                Coordinates() {

                };
                Coordinates(double lat_, double lng_) :lat(lat_),
                    lng(lng_) {

                };
                double lat;
                double lng;
                bool operator==(const Coordinates& other) const;
                bool operator!=(const Coordinates& other) const;

            };

            inline double ComputeDistance(Coordinates from, Coordinates to) {
                using namespace std;
                if (from == to) {
                    return 0;
                }
                int RadiusZemli = 6371000;
                static const double Radian = 3.1415926535 / 180.;
                return acos(sin(from.lat * Radian) * sin(to.lat * Radian) + cos(from.lat * Radian) * cos(to.lat * Radian) * cos(abs(from.lng - to.lng) * Radian)) * RadiusZemli;
            }
        }
    }
}