#pragma once
#include "transport_catalogue.h"    // структуры данных

#include <utility>          // for std::pair<>
#include <string>
#include <string_view>      // for string manipulation via sv
#include <istream>          // for istream
#include <iostream>         // for cout

namespace transport_catalogue::stat_reader
{

	// Производит чтение запросов к справочнику
	void ProcessRequest(TransportCatalogue&, std::istream&);

}
