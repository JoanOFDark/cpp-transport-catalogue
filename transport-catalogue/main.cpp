#include "geo.h"
#include "input_reader.h"
#include "transport_catalogue.h"
#include "stat_reader.h"

#include <sstream>

int main()
{
    transport_catalogue::TransportCatalogue tc;

    transport_catalogue::input_reader::ProcessInput(tc, std::cin);
    transport_catalogue::stat_reader::ProcessRequest(tc, std::cin);

    return 0;
}
