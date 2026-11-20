#include "Grid.hpp"
#include "h.hpp"

// Дополнительные проходы раскрытия макросов
#define DUMP_EXPAND(...)  DUMP_EXPAND4(DUMP_EXPAND4(DUMP_EXPAND4(DUMP_EXPAND4(__VA_ARGS__))))
#define DUMP_EXPAND4(...) DUMP_EXPAND3(DUMP_EXPAND3(DUMP_EXPAND3(DUMP_EXPAND3(__VA_ARGS__))))
#define DUMP_EXPAND3(...) DUMP_EXPAND2(DUMP_EXPAND2(DUMP_EXPAND2(DUMP_EXPAND2(__VA_ARGS__))))
#define DUMP_EXPAND2(...) DUMP_EXPAND1(DUMP_EXPAND1(DUMP_EXPAND1(DUMP_EXPAND1(__VA_ARGS__))))
#define DUMP_EXPAND1(...) __VA_ARGS__

#define DUMP_PARENS ()

#define DUMP_ONE(x)                                    \
    dump_internal_stream_ << (dump_internal_first_ ? "" : ", ") \
                          << #x << ": " << (x);        \
    dump_internal_first_ = false;

#define DUMP_IMPL(x, ...)                              \
    DUMP_ONE(x)                                        \
    __VA_OPT__(DUMP_AGAIN DUMP_PARENS (__VA_ARGS__))

#define DUMP_AGAIN() DUMP_IMPL

#define DUMP(...)                                      \
    ([&]() -> std::string {                            \
        std::ostringstream dump_internal_stream_;      \
        [[maybe_unused]] bool dump_internal_first_ = true; \
        __VA_OPT__(DUMP_EXPAND(DUMP_IMPL(__VA_ARGS__))) \
        return dump_internal_stream_.str();            \
    }())


Grid::Grid(int n)
{
	s_ = sqrt((x_max-x_min)*(y_max-y_min)) / n;
	c = 0;

	n_ = n;
	start = get_grid_coords(x_min, y_min);
	end = get_grid_coords(x_max, y_max);

	// std::cout << "start: " << start.first << ", " << start.second << '\n'
	// 		  << "end: " << end.first << ", " << end.second << '\n';
}


std::pair<int, int> Grid::get_grid_coords(const Dot & d)
{
	float x_s, y_s;
	if (d.x >= x_max && d.x <= x_max)
	{
		x_s = x_max-1.e-5;
	}
	else if (d.x >= x_min && d.x <= x_min)
	{
		x_s = x_min+1.e-5;
	}
	else
	{
		x_s = d.x;
	}

	if (d.y >= y_max && d.y <= y_max)
	{
		y_s = y_max-1.e-5;
	}
	else if (d.y >= y_min && d.y <= y_min)
	{
		y_s = y_min+1.e-5;
	}
	else
	{
		y_s = d.y;
	}
	return std::pair<int, int>(static_cast<int>(std::floor(x_s/s_)), static_cast<int>(std::floor(y_s/s_)));
}


std::pair<int, int> Grid::get_grid_coords(float x, float y)
{
		// std::cout << "x_s: " << x_s << ", x_max: " << x_max << '\n';
	float x_s, y_s;
	if (x >= x_max && x <= x_max)
	{
		x_s = x_max-1.e-5;
	}
	else if (x >= x_min && x <= x_min)
	{
		x_s = x_min+1.e-5;
	}
	else
	{
		x_s = x;
	}

	if (y >= y_max && y <= y_max)
	{
		y_s = y_max-1.e-5;
	}
	else if (y >= y_min && y <= y_min)
	{
		y_s = y_min+1.e-5;
	}
	else
	{
		y_s = y;
	}

	return std::pair<int, int>(static_cast<int>(std::floor(x_s/s_)), static_cast<int>(std::floor(y_s/s_)));
}

void Grid::add_els(const Dot & d)
{
	std::pair<int, int> cell = get_grid_coords(d);
	mp[cell].push_back(d);
	c++;
}

void Grid::add_els(float x, float y)
{
	std::pair<int, int> cell = get_grid_coords(x, y);
	mp[cell].push_back({x, y});
	c++;
}


void Grid::parse(std::string file_name)
{
	std::ifstream file(file_name);
	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			Dot dot;
			std::vector<std::string> result;
			boost::split(result, line, boost::is_any_of(" "));
			dot.x = std::stof(result[0]);
			dot.y = std::stof(result[1]);
			add_els(dot);
		}
	}
	file.close();

	for (int n_y=start.second; n_y<=end.second; n_y++)
	{
		for (int n_x=start.first; n_x<=end.first; n_x++)
		{
			if (!n_el_s_row.size())
			{
				n_el_s_row.push_back(mp[{n_x, n_y}].size());
			}
			else
			{
				n_el_s_row.push_back(mp[{n_x, n_y}].size()+n_el_s_row[n_el_s_row.size()-1]);
			}
		}

	}

	// distances.resize(dots.size());
	// neighbour_i.resize(dots.size());
}

// 	std::map<std::pair<int, int>, std::vector<Dot>> mp;
// 	int n_;
//  float s;
// 	const float x_min = -10;
// 	const float y_min = -10;
// 	const float x_max = 10;
// 	const float y_max = 10;


void Grid::print_mp()
// 	std::map<std::pair<int, int>, std::vector<Dot>> mp;
{
	for (const auto & mp_pair: mp)
	{
		std::cout << "grid pairs[" << mp_pair.first.first << "][" 
				  << mp_pair.first.second << "]\n";

		std::cout << "size: " << mp_pair.second.size() << "\n";
		std::cout << "Dot x & y: \n";
		for (const auto & v_el: mp_pair.second)
		{
			std::cout << v_el.x << ", " << v_el.y << '\n';
		}
		std::cout << "_________\n";
	}
}



std::vector<Dot> &Grid::get_dots(int i_x, int i_y)
{
	return mp[std::pair<int, int>(i_x, i_y)];	
}


void Grid::j_to_i(int n_x, int n_y, int j, int &i)
{
	int s = (n_)*(n_y-start.second)+(n_x-start.first); // end.first-start.first+1
	if (s > 0)
		i = n_el_s_row[s-1]+j;
	else
		i = j;
}

void Grid::i_to_j(int &n_x, int &n_y, int &j, int i)
{
	int k=0;
	for (;i>=n_el_s_row[k];k++)
		;
	// i=k;
	n_y = start.second+k/n_;
	n_x = start.first+k%n_;

	if (n_y > end.second)
		n_y = end.second;

	if (n_x > end.first)
		n_x = end.first;

	if (k > 0)
		j = i-n_el_s_row[k-1];
	else
		j =i;
}


void Grid::debug_print(std::string file_name)
{
	std::ofstream file(file_name);
	if (file.is_open())
	//	std::map<std::pair<int, int>, std::vector<Dot>> mp;
	{
		for (const auto & [key, value]: mp)
		{
			file << key.first << ", " << key.second << ": " << value.size() << '\n';
		}

		file << "_____________\n";
		for (int i=0; i<n_el_s_row.size(); i++)
		{
			file << i << ": " << n_el_s_row[i] <<  '\n';
		}
		file << "_____________\n";
	    file << DUMP(start.first, start.second) << '\n';
	    file << DUMP(end.first, end.second);
		file << "_____________\n";

		unsigned core_n = boost::thread::hardware_concurrency();
		int block_l = c / core_n + 1;
	    int start_i, end_i;

		int n_x_start, n_y_start, j_start, n_x_end, n_y_end, j_end;

		for (int i=0; i<core_n; i++)
		{
		    start_i = i * block_l;
		    if (start_i + block_l >= c)
		    	end_i = c - 1;
		    else
		    	end_i = start_i + block_l - 1;
	    	file << DUMP(start_i, end_i) << '\n';


	    	i_to_j(n_x_start, n_y_start, j_start, start_i);
			i_to_j(n_x_end, n_y_end, j_end, end_i);

			file << DUMP(n_x_start, n_y_start, j_start) << '\n';
			file << DUMP(n_x_end, n_y_end, j_end) << '\n';

		}

	}
	file.close();
}


Grid::~Grid()
{
}
