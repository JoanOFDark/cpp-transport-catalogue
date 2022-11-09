#pragma once
#include <cmath>
namespace transport_catalogue::geo
{

    // Радиус Земли, метров
    const int EARTH_RADIUS = 6371000;

    // Структура, определяющая положение точки в пространстве
    // и методы работы с ней
    struct Coordinates
    {
        double lat;  // Широта
        double lng;  // Долгота
        bool operator==(const Coordinates& other) const
        {
            return lat == other.lat && lng == other.lng;
        }
        bool operator!=(const Coordinates& other) const
        {
            return !(*this == other);
        }
    };

    inline double ComputeDistance(Coordinates from, Coordinates to)

    {
        using namespace std;
        if (from == to)
        {
            return 0;
        }
        static const double dr = 3.1415926535 / 180.;
        return acos(sin(from.lat * dr) * sin(to.lat * dr)
            + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
            * EARTH_RADIUS;
    }

}
