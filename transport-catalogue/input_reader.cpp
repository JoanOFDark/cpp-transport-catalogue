#include "input_reader.h"
// �������� ������� � ����
// ��� ��������� � ���� git-�����������

// � ������� �������� ����� ������ ����������� ��������� � ��������� ���� ���� utility.h/.cpp
namespace detail
{
	std::pair<std::string_view, std::string_view> Split(std::string_view line, char by, int count)
	{
		size_t pos = line.find(by);
		for (int i = 1; (i < count) && (pos != line.npos); ++i)
		{
			pos = line.find(by, pos + 1);
		}
		// Substr ����� ��������� �� ���� ����� �������� ������, ������� npos (������� ���� ������)
		std::string_view left = line.substr(0, pos);

		// ���� ������-����������� ��� ������...
		if (pos < line.size() && pos + 1 < line.size())
		{
			// ...���������� ��������� ��� �����������
			return { left, line.substr(pos + 1) };
		}
		else
		{
			// ...����� ��� ���������� � ������ ������ � ������ ������
			return { left, std::string_view() };
		}
	}

	std::string_view Lstrip(std::string_view line)
	{
		// ���� ������ �� ����� � ������ ������ �� ������...
		while (!line.empty() && std::isspace(line[0]))
		{
			// ...������� ������ ������
			line.remove_prefix(1);
		}
		return line;
	}

	std::string_view Rstrip(std::string_view line)
	{
		// ��� ��������� �������� ����� ������ � ��������� ����������
		size_t line_size = line.size();
		// ���� ������ �� ����� � ��������� ������ �� ������...
		while (!line.empty() && std::isspace(line[line_size - 1]))
		{
			// ...������� ��������� ������
			line.remove_suffix(1);
			--line_size;
		}
		return line;
	}

	std::string_view TrimString(std::string_view line)
	{
		return Rstrip(Lstrip(line));
	}
}


namespace transport_catalogue::input_reader
{

	void ProcessInput(TransportCatalogue& tc, std::istream& is)
	{
		// ������ ��������. ������� ���������� ������� �� ��������� �������
		std::vector<InputQuery> queries;

		// ����� ��� �������� ������� ����������� ������
		std::string line;
		std::getline(is, line);
		// ������ ������ - ���������� �������� �� ���������� ������
		size_t request_num = static_cast<size_t>(std::stoul(line));
		for (size_t i = 0; i < request_num; ++i)
		{
			using namespace std::literals;
			std::getline(is, line, '\n');
			// ������ ������: "Stop Tolstopaltsevo: 55.611087, 37.208290"
			// ������ ������: "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka"
			// ������ ������: "Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya"

			// �������� ������� �� �� ����������
			auto tmp = detail::Split(line, ' ');
			// ������ �������
			tmp.first = detail::TrimString(tmp.first);
			tmp.second = detail::TrimString(tmp.second);

			// ���� tmp.second ����, �� ��� ������������ ����, ����������
			// TODO � ������� ����� ��������� ������� ��� ����������
			if (tmp.second.empty())
			{
				continue;
			}

			InputQuery query;
			if (tmp.first == "Stop"sv)
			{
				// ������ ������: "Stop Tolstopaltsevo: 55.611087, 37.208290"
				// ������ ������: "Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya"
				query.type = InputQueryType::AddStop;
				// ����� �������� 2 �������� ��� tmp.second: ���� ������ ����������, ���� ���������� + ������ ����������
				// ���� ������ �������
				tmp = detail::Split(tmp.second, ',', 2);
				// ������ �������
				tmp.first = detail::Rstrip(tmp.first);
				tmp.second = detail::Lstrip(tmp.second);
				if (tmp.second.size() != 0)
				{
					// ���� ������ ����������. ��������� ������ �� 2 ���������
					//tmp.first �������� "Tolstopaltsevo: 55.611087, 37.208290"
					//tmp.second �������� " 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya"
					// ��� ����������� ���������� ���������� ����� ��������� ������������ ��������� ��� ������� �������.
					auto tmp_stop_name = detail::Split(tmp.first, ':');
					tmp_stop_name.first = detail::TrimString(tmp_stop_name.first);
					// ������� ������ � ������������
					query.query = std::string(tmp.first);
					queries.push_back(std::move(query));
					// ������ ������ � �����������
					query = {};
					query.type = InputQueryType::AddStopsDistance;
					query.query = std::string(tmp_stop_name.first) + ":"s + std::string(tmp.second);
					queries.push_back(std::move(query));
				}
				else
				{
					// � ������� ������ ����������
					//tmp.first �������� "Tolstopaltsevo: 55.611087, 37.208290"
					//tmp.second �������� ""
					query.query = std::string(tmp.first);
					queries.push_back(std::move(query));
				}
			}
			else if (tmp.first == "Bus"sv)
			{
				// ������ ������: "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka"
				query.type = InputQueryType::AddRoute;
				query.query = std::string(tmp.second);
				queries.push_back(std::move(query));
			}
			else
			{
				query.type = InputQueryType::NoOp;
				// ���������� ����� �������, ���� ���� ��� ��� NoOp
				query.query = std::string(tmp.second);
				queries.push_back(std::move(query));
			}
		}

		// ������ ����� � ���������� �� ������� ���������. ������������ �������
		ProcessInputQueries(tc, queries);
	}

	void ProcessInputQueries(TransportCatalogue& tc, std::vector<InputQuery>& queries)
	{
		std::vector<InputQuery> delayed;

		// ������� ������������ ������� �� ���������� ���������
		for (auto& element : queries)
		{
			if (element.type == InputQueryType::AddStop)
			{
				tc.AddStop(ProcessQueryAddStop(element.query));
			}
			else
			{
				delayed.push_back(std::move(element));
			}
		}

		// �.�. ������� ����������� �����, �� ������ � �� sv ������ �� �����, ��������� ������ swap
		queries.swap(delayed);
		delayed.clear();

		// ������������ ������� �� ���������� ����������
		for (auto& element : queries)
		{
			if (element.type == InputQueryType::AddStopsDistance)
			{
				// ������� ���������, ��� ��������� �������� tc ������ �������
				ProcessQueryAddStopsDistance(tc, element.query);
			}
			else
			{
				delayed.push_back(std::move(element));
			}
		}

		// ������������ ��� ���������
		for (auto& element : delayed)
		{
			if (element.type == InputQueryType::AddRoute)
			{
				// ������� ���������, ��� ��������� �������� tc ������ �������
				tc.AddRoute(std::move(ProcessQueryAddRoute(tc, element.query)));
			}

			// ��������� �������, ��������, InputQueryType::NoOp, ����������
		}
	}


	Stop ProcessQueryAddStop(std::string& query)
	{
		// ������ ������� ������ "Rasskazovka: 55.632761, 37.333324"
		Stop new_stop;

		// �������� �������� �� ����� ���������
		auto tmp = detail::Split(query, ':');
		// ������ �������
		tmp.first = detail::TrimString(tmp.first);
		tmp.second = detail::TrimString(tmp.second);

		// ��������� string � ���������
		new_stop.name = std::string(tmp.first);

		// ��������� ����������
		tmp = detail::Split(tmp.second, ' ');
		// ������ ������� � ������ ����� ��������� ����� ����� ������
		tmp.first = detail::Rstrip(tmp.first);
		tmp.second = detail::Lstrip(tmp.second);

		// ������������ � ��������� ����������
		new_stop.coords.lat = std::stod(std::string(tmp.first));
		new_stop.coords.lng = std::stod(std::string(tmp.second));

		// ���������� �������������� ���������
		return new_stop;
	}


	void ProcessQueryAddStopsDistance(TransportCatalogue& tc, std::string& query)
	{
		// ������ ������� ������ "Tolstopaltsevo: 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya"

		// �������� �������� �� ����� ����������
		auto tmp = detail::Split(query, ':');
		// ������ �������
		tmp.first = detail::TrimString(tmp.first);
		tmp.second = detail::TrimString(tmp.second);

		// �������� ��������� �� ��������� ��� ������� ��������� ����������
		const Stop* stop_from = tc.GetStopByName(tmp.first);
		if (stop_from == nullptr)
		{
			// ��������� �� ����������. �������.
			return;
		}

		size_t dist = 0U;
		const Stop* stop_to = nullptr;

		while (tmp.second.size() != 0)
		{
			// �������� ����������
			tmp = detail::Split(tmp.second, 'm');
			tmp.first = detail::TrimString(tmp.first);
			tmp.second = detail::Lstrip(tmp.second);
			dist = std::stoul(std::string(tmp.first));

			// �������� �������� ������ ���������
			tmp = detail::Split(tmp.second, ' ');
			tmp = detail::Split(tmp.second, ',');
			tmp.first = detail::TrimString(tmp.first);
			tmp.second = detail::Lstrip(tmp.second);
			stop_to = tc.GetStopByName(tmp.first);
			if (stop_to == nullptr)
			{
				// ��������� �� ����������. �������.
				return;
			}

			// ������ ������ � ������� ����������
			tc.AddDistance(stop_from, stop_to, dist);
		}
	}


	Route ProcessQueryAddRoute(TransportCatalogue& tc, std::string& query)
	{
		// ������ ������� ������ "750: Tolstopaltsevo - Marushkino - Rasskazovka"
		// ������ ������� ������ "256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye"

		Route new_route;

		// �������� �������� �������� �� ����� ���������
		auto tmp = detail::Split(query, ':');
		// ������ �������
		tmp.first = detail::TrimString(tmp.first);
		tmp.second = detail::TrimString(tmp.second);

		// ��������� string � ���������
		new_route.bus_number = std::string(tmp.first);

		// ���� � ������ ��� ����������� > (��������� �������), �� ������������� ��-��������� ����������� '-'
		char delim = (tmp.second.find('>') == tmp.second.npos ? '-' : '>');

		std::vector<std::string_view> stops_list;

		// ���� tmp.second �� ����� (������� �� ����� ������� ����� � ��� ���� ������, ���� ������ ���)
		while (tmp.second.size() != 0)
		{
			// �������� ������ �� ���������

			// �������� ��������� ��������� � tmp.first
			tmp = detail::Split(tmp.second, delim);
			// ������ ������� � ������ ����� ��������� ����� ����� ������
			tmp.first = detail::Rstrip(tmp.first);
			tmp.second = detail::Lstrip(tmp.second);

			// ���������� ��������
			stops_list.push_back(tmp.first);
		}

		if ((delim == '-') && (stops_list.size() > 1))
		{
			// ����������� �������� ������� ��� ������������ ��������
			for (int i = stops_list.size() - 2; i >= 0; --i)
			{
				// ��������� � ����� ������� ��������� � �������� �����������, ����� �������� stops_list[size() - 1]
				stops_list.push_back(stops_list[i]);
			}
		}

		// ��������� string_view � ���������� � ��������� �� ��������� �� ���� � ����������
		for (auto& element : stops_list)
		{
			// �������� ���� ���������, ���� nullptr
			const Stop* tmp_ptr = tc.GetStopByName(element);
			if (tmp_ptr != nullptr)
			{
				// ��������� ������ ������������ ��������� (� ������ ������)
				new_route.stops.push_back(tmp_ptr);
			}
		}

		// ���������� �������������� ���������
		return new_route;
	}

}