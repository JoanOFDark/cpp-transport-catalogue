#include "geo.h" 

bool transport_catalogue::detail::distant::Coordinates:: operator==(const Coordinates& other) const {
    return lat == other.lat && lng == other.lng;
}

bool  transport_catalogue::detail::distant::Coordinates::operator!=(const Coordinates& other) const {
    return !(*this == other);
}
