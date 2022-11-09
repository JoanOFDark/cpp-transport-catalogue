#pragma once
#include "transport_catalogue.h"    // структуры данных

#include <utility>          // for std::pair<>
#include <string>
#include <string_view>      // for string manipulation via sv
#include <istream>          // for istream
#include <iostream>         // for cout  (in input_reader)

namespace transport_catalogue::input_reader
{

	// Производит построчное чтение исходных данных согласно заданной схеме
	// и формирует список запросов на обновление каталога
	void ProcessInput(TransportCatalogue&, std::istream&);

}
