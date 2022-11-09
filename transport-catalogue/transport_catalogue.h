#pragma once
#include "geo.h"           // ��� ������ � ������������ ���������

#include <deque>
#include <string>
#include <string_view>
#include <ostream>         // ��� ���������� ��������� ������
#include <sstream>         // ��� stringstream
#include <iomanip>         // ��� ���������� ������� (������������)
#include <unordered_set>
#include <unordered_map>
#include <algorithm>       // ��� uniq(), find()
#include <utility>         // for std::pair<>
#include <cctype>          // for isspace() function
#include <functional>      // ��� ������� hash<>

namespace transport_catalogue
{

	// ��� �������� �� ���������� ���������� � ���� ������. ���������� ������ ������� ������.
	enum class InputQueryType
	{
		NoOp,            // �������� ��-���������, ��� ��������
		AddStop,         // �������� ���������� �� ��������� (��� � ����������) ����-1
		AddRoute,        // �������� ���������� � �������� (��� ��������� ������ ������������) ����-3
		AddStopsDistance,// �������� ���������� �� ��������� (������� ���������� �� �������� ���������) ����-2
	};

	// ��������� ������� �� ���������� ����������.
	// ������� ������ ������� ������ ��������� ������� ������������ ������� (���, ������� ���� �������), �.�. 
	// ������ �� ����� ������ ������ ��������� ������ � ������ ����������� ����������� �������� � ����������.
	// ������ ������� ������ ������������� ����������� ���������� ���������� ������ ���� �������.
	struct InputQuery
	{
		InputQueryType type = InputQueryType::NoOp;
		std::string query;
	};

	// ��� �������� �� ���������� ���������� � ���� ������. ���������� ������ ������� ������.
	enum class RequestQueryType
	{
		NoOp,            // �������� ��-���������, ��� ��������
		GetRouteByName,  // �������� ������� �� �����
		GetBusesForStop, // �������� �������� ��� ���������
	};

	// ��������� ������� �� ����� ����������
	struct RequestQuery
	{
		RequestQueryType type{ RequestQueryType::NoOp };
		std::string_view params;
	};

	// ��� ���������� ���������� ������� � ���� ������.
	enum class RequestResultType
	{
		Ok,                  // �������� ��-���������
		NoBuses,             // ���������: �������� �� �������
		StopNotExists,       // ���������: ��������� �� �������
		RouteNotExists,      // ���������: ������� �� ������
	};

	// ���������, �������� ���������� �� ��������� � ������������
	// ������ ������ � ���
	struct Stop
	{
	public:
		std::string name;
		geo::Coordinates coords{ 0L,0L };   // ����������

	};

	// ���������, �������� ���������� � �������� (��������) � ������������
	// ������ ������ � ���
	struct Route
	{
		std::string bus_number;            // ����� �������� (��������)
		std::vector<const Stop*> stops;    // ��������� ���������� �� ��������� ��������
		size_t unique_stops_qty = 0U;      // ���������� ���������� ��������� �� �������� (��������, �.�. ���������� ������ ��� ������������ ��������)
		double geo_route_length = 0L;      // ����� �������� �� ������ ����� ������������ (��������, �.�. ���������� ������ ��� ������������ ��������)
		size_t meters_route_length = 0U;   // ����� �������� � ������ �������� ���������� ����� ������� (�����) (��������, �.�. ���������� ������ ��� ������������ ��������)
		double curvature = 0L;             // ������������ �������� = meters_route_length / geo_route_length. >1 ��� ������ ��������, ����� ����������
	};

	struct RequestResult
	{
		RequestResultType code = RequestResultType::Ok;   // ��� ���������� (���� ��� ��������� ����������� ������)
		std::vector<std::string> vector_str;              // ������ �����. ������� ������������� ��������� ������������ ���������� �������.
		const Stop* s_ptr = nullptr;               // ��������� �� ��������� ���������. ������� ������������� ������������ ���������� �������.
		const Route* r_ptr = nullptr;              // ��������� �� ��������� ������� (�������). ������� ������������� ������������ ���������� �������.
	};

	class PairPointersHasher
	{
	public:
		std::size_t operator()(const std::pair<const Stop*, const Stop*> pair_of_pointers) const noexcept
		{
			auto ptr1 = static_cast<const void*>(pair_of_pointers.first);
			auto ptr2 = static_cast<const void*>(pair_of_pointers.second);
			return hasher_(ptr1) * 37 + hasher_(ptr2);
		}

	private:
		std::hash<const void*> hasher_;
	};


	class TransportCatalogue
	{
	public:
		TransportCatalogue();
		~TransportCatalogue();

		void AddStop(Stop&& stop);              // ��������� ��������� � ������� ���� ���������
		void AddRoute(Route&& bus);            // ��������� ������� � ������� ���� ���������
		void AddDistance(const Stop* stop_from, const Stop* stop_to, size_t dist);    // ��������� ���������� ����� ����� ����������� � �������
		size_t GetDistance(const Stop* stop1, const Stop* stop2);          // ���������� ���������� (size_t �����) ����� ����� ����������� � ������������� ����
		size_t GetDistanceDirectly(const Stop* stop1, const Stop* stop2);  // ���������� ���������� (size_t �����) ����� ����� ����������� ��� ������������ ����

		const Stop* GetStopByName(std::string_view stop_name);    // ���������� ��������� �� ��������� ��������� �� �� �����
		Route* GetRouteByName(std::string_view bus_name);        // ���������� ��������� �� ��������� �������� �� ��� �����

		RequestResult GetRouteInfo(std::string_view);         // ���������� ������ � ����������� � �������� � ������� �� sv
		RequestResult GetBusesForStop(std::string_view);      // ���������� ������ � ����������� �� ��������� ��� �������� �� sv

	private:
		std::deque<Stop> all_stops_data_;                                     // ��� � ����������� ��� ���� ���������� (�������� ������, �� ���������)
		std::unordered_map<std::string_view, const Stop*> all_stops_map_;     // ������� ��������� (������� � ���-��������)
		
		std::deque<Route> all_buses_data_;                                    // ��� � ����������� ��� ���� ���������
		std::unordered_map<std::string_view, Route*> all_buses_map_;          // ������� ��������� (���������) (������� � ���-��������)
		
		std::unordered_map<std::pair<const Stop*, const Stop*>, size_t, PairPointersHasher> distances_map_; // ������� ���������� ����� �����������


		// ���������� string_view � ������ ��������� �� ��������� �� ��������� ��������� Stop
		std::string_view GetStopName(const Stop* stop_ptr);
		// ���������� string_view � ������ ��������� ��� ���������� ��������� Stop
		std::string_view GetStopName(const Stop stop);

		// ���������� string_view � ������� �������� �� ��������� �� ��������� ��������� Route
		std::string_view GetBusName(const Route* route_ptr);
		// ���������� string_view � ������� �������� ��� ���������� ��������� Route
		std::string_view GetBusName(const Route route);
	};
}

