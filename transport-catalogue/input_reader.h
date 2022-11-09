#pragma once
#include "transport_catalogue.h"    // ��������� ������

#include <utility>          // for std::pair<>
#include <string>
#include <string_view>      // for string manipulation via sv
#include <istream>          // for istream
#include <iostream>         // for cout  (in input_reader)
#include <vector>

// � ������� �������� ����� ������ ����������� ��������� � ��������� ���� ���� utility.h/.cpp
namespace detail
{
	// ����������� 1 ������ � 2, ����������� count �� ������� ��������-������������
	std::pair<std::string_view, std::string_view> Split(std::string_view, char, int count = 1);
	// ������� ������� (�������� �����, ��������� � �.�.) �� ������ ������
	std::string_view Lstrip(std::string_view);
	// ������� ������� (�������� �����, ��������� � �.�.) � ����� ������
	std::string_view Rstrip(std::string_view);
	// ������� ������� (�������� �����, ��������� � �.�.) �� ���� ������ ������
	std::string_view TrimString(std::string_view);
}

namespace transport_catalogue::input_reader
{

	void ProcessInput(TransportCatalogue&, std::istream&);     // ���������� ���������� ������ �������� ������ � ������������ ������� ��������
	void ProcessInputQueries(TransportCatalogue&, std::vector<InputQuery>&);    // ������������ ������� � ������������ � �����������

	Stop ProcessQueryAddStop(std::string&);           // ������������ ������ �� ���������� ���������
	void ProcessQueryAddStopsDistance(TransportCatalogue&, std::string&);      // ������������ ������ �� ���������� ����������
	Route ProcessQueryAddRoute(TransportCatalogue&, std::string&);             // ������������ ������ �� ���������� ��������
}