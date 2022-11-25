#include "geo.h" 

bool TransportsCatalogue::detail::distant::Coordinates:: operator==(const Coordinates& other) const {
    return lat == other.lat && lng == other.lng;
}

bool  TransportsCatalogue::detail::distant::Coordinates::operator!=(const Coordinates& other) const {
    return !(*this == other);
}
