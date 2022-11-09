#pragma once
#include "transport_catalogue.h"    // ��������� ������
#include "input_reader.h"           // ������ ������ �� ��������

#include <utility>          // for std::pair<>
#include <string>
#include <string_view>      // for string manipulation via sv
#include <sstream>          // ��� stringstream
#include <iomanip>         // ��� ���������� ������� (������������)
#include <istream>          // for istream
#include <iostream>         // for cout

namespace transport_catalogue::stat_reader
{

    std::ostream& operator<<(std::ostream&, const Stop*);
    std::ostream& operator<<(std::ostream& os, const Route* route);

    std::ostream& ProcessRequests(std::ostream&, TransportCatalogue&, std::istream&);   // ���������� ������ � ������ �������� � �����������
    std::ostream& ExecuteRequest(std::ostream&, TransportCatalogue&, RequestQuery&);    // ���������� ������ � ������������ ����� ������quest(TransportCatalogue&, RequestQuery&);           // ���������� ������ � ������������ ����� ������

}