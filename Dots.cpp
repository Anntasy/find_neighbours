#include "Dots.hpp"
#include "h.hpp"



#define DUMP_EXPAND(...)  DUMP_EXPAND4(DUMP_EXPAND4(DUMP_EXPAND4(DUMP_EXPAND4(__VA_ARGS__))))
#define DUMP_EXPAND4(...) DUMP_EXPAND3(DUMP_EXPAND3(DUMP_EXPAND3(DUMP_EXPAND3(__VA_ARGS__))))
#define DUMP_EXPAND3(...) DUMP_EXPAND2(DUMP_EXPAND2(DUMP_EXPAND2(DUMP_EXPAND2(__VA_ARGS__))))
#define DUMP_EXPAND2(...) DUMP_EXPAND1(DUMP_EXPAND1(DUMP_EXPAND1(DUMP_EXPAND1(__VA_ARGS__))))
#define DUMP_EXPAND1(...) __VA_ARGS__

#define DUMP_PARENS ()

#define DUMP_ONE(x)                                    \
    std::cout << (dump_first_ ? "" : ", ")             \
              << #x << ": " << (x);                    \
    dump_first_ = false;

#define DUMP_IMPL(x, ...)                              \
    DUMP_ONE(x)                                        \
    __VA_OPT__(DUMP_AGAIN DUMP_PARENS (__VA_ARGS__))

#define DUMP_AGAIN() DUMP_IMPL

#define DUMP(...)                                      \
    do {                                               \
        bool dump_first_ = true;                       \
        __VA_OPT__(DUMP_EXPAND(DUMP_IMPL(__VA_ARGS__)))\
        std::cout << '\n';                             \
    } while (false)



Dots::Dots()
{
	grid = new Grid();
}

Dots::Dots(int n_s)
{
	grid = new Grid(n_s);
}

Dots::Dots(Mode mode, int dots_n): mode_(mode) {
	if (mode_ == Mode::Grid || mode_ == Mode::Grid_Thread)
	{
		int n_s = static_cast<int>(std::floor(sqrt(dots_n/3)));
		if (n_s < 3)
			n_s = 3;
		grid = new Grid(n_s);
	}
}

void Dots::parse(std::string file_name)
{
	if (mode_ == Mode::Grid || mode_ == Mode::Grid_Thread)
	{
		parse_grid(file_name);
	}
	else if (mode_ == Mode::Base || mode_ == Mode::Base_Thread)
	{
		parse_base(file_name);
	}
}


void Dots::parse_base(std::string file_name)
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
			dots.push_back(dot);
		}
	}
	file.close();
	distances.resize(dots.size());
	neighbour_i.resize(dots.size());
}


void Dots::parse_grid(std::string file_name)
{
	grid->parse(file_name);
	distances.resize(grid->c);
	neighbour_i.resize(grid->c);
}


void Dots::count_distance()
{
	switch (mode_)
	{
	case Mode::Base:
		count_distance_base();
		break;
	case Mode::Base_Thread:
		count_distance_threads();
		break;
	case Mode::Grid:
		count_distance_base_grid();
		break;
	case Mode::Grid_Thread:
		count_distance_grid_threads();
		break;
	}
}


void Dots::count_distance_base()
{
	get_neighbours(0, dots.size()-1);
}



void Dots::get_neighbours(int start_i, int end_i)
{
	for (int i=start_i; i<=end_i; i++)
	{
		int min_i = i ? 0 : 1; // ~ if (i) {return 0} {return 1}
		float cur_min_dist = 10000000;
		float cur_dist_2 = 0;
		for (int j=0; j<dots.size(); j++)
		{
			if (i == j)
			{
				continue;
			}
			cur_dist_2 = (dots[j].x-dots[i].x)*(dots[j].x-dots[i].x) + (dots[j].y-dots[i].y)*(dots[j].y-dots[i].y);
			if (cur_min_dist>cur_dist_2)
			{
				cur_min_dist = cur_dist_2;
				min_i = j;
			}
		}
		distances[i] = cur_min_dist;
		neighbour_i[i] = min_i;
	}

}

void Dots::get_neighbours_grid(int start_i, int end_i)
{
	int i = start_i;
	int n_x_start, n_y_start, j_start, n_x_end, n_y_end, j_end;
	// void Grid::i_to_j(int &n_x, int &n_y, int &j, int i)
	// void Grid::j_to_i(int n_x, int n_y, int j, int &i)

	grid->i_to_j(n_x_start, n_y_start, j_start, start_i);
	grid->i_to_j(n_x_end, n_y_end, j_end, end_i);

	debug.lock();
	DUMP(n_x_start, n_y_start, j_start);
	DUMP(n_x_end, n_y_end, j_end);
	debug.unlock();
	for (int n_y=n_y_start; n_y<=n_y_end; n_y++)
	{
		for (int n_x=n_x_start; n_x<=n_x_end; n_x++)
		{	
			for (int j=((n_y==n_y_start && n_x==n_x_start) ? j_start : 0); j<((n_y==n_y_end && n_x==n_x_end) ? j_end+1 : grid->mp[{n_x, n_y}].size()); j++)
			{
				Dot d = grid->mp[{n_x, n_y}][j];
				// ##############
				float cur_min_dist = 10000000;
				float cur_dist_2 = 0;
				int min_i = 0;
				for (int n_y2=n_y-1; n_y2<=n_y+1; n_y2++)
				{
					if (n_y2 > grid->end.second || n_y2 < grid->start.second)
					{
						continue;
					}
					for (int n_x2=n_x-1; n_x2<=n_x+1; n_x2++)
					{
						if (n_x2 > grid->end.first || n_x2 < grid->start.first)
						{
							continue;
						}
						int i2;
						grid->j_to_i(n_x2, n_y2, 0, i2); // n_x; n_y; j -> i2 ; n_el_s_row[.][.][.]
						for (int j2=0; j2<grid->mp[{n_x2, n_y2}].size(); j2++)
						{
							if (n_x2==n_x && n_y2==n_y && j == j2)
							{
								i2++;
								continue;
							}
							Dot d2 = grid->mp[{n_x2, n_y2}][j2];

							cur_dist_2 = (d2.x-d.x)*(d2.x-d.x) + (d2.y-d.y)*(d2.y-d.y);
							if (cur_min_dist>cur_dist_2)
							{
								cur_min_dist = cur_dist_2;
								min_i = i2;
							}
							i2++;
						}
					}
				}
				// std::cout << "cur_min_dist: " << cur_min_dist << ", min_i: " << min_i << ", i: " << i << "\n";
				distances[i] = cur_min_dist;
				neighbour_i[i] = min_i;
				i++;
			}
		}

	}
}

	// std::vector<Dot> dots;
	// std::vector<float> distances;
	// std::vector<int> neighbour_i;
	// {1, 2}; {4, 2}; {9, 1}


void Dots::count_distance_grid_threads()
{
	// th_f (dots), len(dots): th_n ; start_i end_i
	unsigned core_n = boost::thread::hardware_concurrency();
	int block_l = grid->c / core_n + 1;
	std::vector<boost::thread *> threads;
    int start_i, end_i;

    DUMP(grid->start.first, grid->start.second);
    DUMP(grid->end.first, grid->end.second);

    // std::cout << "grid->c: " << grid->c << ", block_l: " << block_l << ", core_n: " << core_n << '\n';
	for (int i=0; i<core_n; i++)
	{
	    start_i = i * block_l;
	    if (start_i + block_l >= grid->c)
	    	end_i = grid->c - 1;
	    else
	    	end_i = start_i + block_l - 1;
    	// std::cout << "start_i: " << start_i << ", end_i: " << end_i << '\n';
	    threads.push_back(new boost::thread(&Dots::get_neighbours_grid, this, start_i, end_i));
	}

    for (auto t: threads)
	{
		t->join();
		delete t;
	}
}


void Dots::count_distance_base_grid()
{
	int i = 0;
	for (int n_y=grid->start.second; n_y<=grid->end.second; n_y++)
	{
		for (int n_x=grid->start.first; n_x<=grid->end.first; n_x++)
		{	
			for (int j=0; j<grid->mp[{n_x, n_y}].size(); j++)
			{
				Dot d = grid->mp[{n_x, n_y}][j];
				// ##############
				float cur_min_dist = 10000000;
				float cur_dist_2 = 0;
				int min_i = 0;
				for (int n_y2=n_y-1; n_y2<=n_y+1; n_y2++)
				{
					if (n_y2 > grid->end.second || n_y2 < grid->start.second)
					{
						continue;
					}
					for (int n_x2=n_x-1; n_x2<=n_x+1; n_x2++)
					{
						if (n_x2 > grid->end.first || n_x2 < grid->start.first)
						{
							continue;
						}
						int i2;
						grid->j_to_i(n_x2, n_y2, 0, i2); // n_x; n_y; j -> i2 ; n_el_s_row[.][.][.]
						for (int j2=0; j2<grid->mp[{n_x2, n_y2}].size(); j2++)
						{
							if (n_x2==n_x && n_y2==n_y && j == j2)
							{
								i2++;
								continue;
							}
							Dot d2 = grid->mp[{n_x2, n_y2}][j2];

							cur_dist_2 = (d2.x-d.x)*(d2.x-d.x) + (d2.y-d.y)*(d2.y-d.y);
							if (cur_min_dist>cur_dist_2)
							{
								cur_min_dist = cur_dist_2;
								min_i = i2;
							}
							i2++;
						}
					}
				}
				// std::cout << "cur_min_dist: " << cur_min_dist << ", min_i: " << min_i << ", i: " << i << "\n";
				distances[i] = cur_min_dist;
				neighbour_i[i] = min_i;
				i++;
			}
		}

	}
}


void Dots::count_distance_threads()
{
	// th_f (dots), len(dots): th_n ; start_i end_i
	unsigned core_n = boost::thread::hardware_concurrency();
	int block_l = dots.size() / core_n + 1;
	std::vector<boost::thread *> threads;
    int start_i, end_i;


	for (int i=0; i<core_n; i++) 
	{
	    start_i = i * block_l;
	    if (start_i + block_l >= dots.size())
	    	end_i = dots.size() - 1;
	    else
	    	end_i = start_i + block_l - 1;
	    threads.push_back(new boost::thread(&Dots::get_neighbours, this, start_i, end_i));
	}

    for (auto t: threads)
	{
		t->join();
		delete t;
	}
}



void Dots::print()
{
	for (int i=0; i<neighbour_i.size(); i++)
	{
		std::cout << "dot" << i << "'s neighbour: " << "dot" << neighbour_i[i] << '\n';
		std::cout << "S: " << sqrt(distances[i]) << "\n";
	} 
}


Dots::~Dots()
{
	if (mode_ == Mode::Grid || mode_ == Mode::Grid_Thread)
	{
		delete grid;
	}
};
