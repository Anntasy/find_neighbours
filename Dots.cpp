#include "Dots.hpp"
#include "h.hpp"


Dots::Dots()
{
	grid = new Grid();
};

Dots::Dots(int n_s)
{
	grid = new Grid(n_s);
};

void Dots::parse(std::string file_name)
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

	// std::vector<Dot> dots;
	// std::vector<float> distances;
	// std::vector<int> neighbour_i;
	// {1, 2}; {4, 2}; {9, 1}


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
	// for (int i=0; i<=grid->c; i++)
	// {
	// 	int min_i = i ? 0 : 1; // ~ if (i) {return 0} {return 1}
	// 	float cur_min_dist = 10000000;
	// 	float cur_dist_2 = 0;
	// 	for (int j=0; j<dots.size(); j++)
	// 	{
	// 		if (i == j)
	// 		{
	// 			continue;
	// 		}
	// 		cur_dist_2 = (dots[j].x-dots[i].x)*(dots[j].x-dots[i].x) + (dots[j].y-dots[i].y)*(dots[j].y-dots[i].y);
	// 		if (cur_min_dist>cur_dist_2)
	// 		{
	// 			cur_min_dist = cur_dist_2;
	// 			min_i = j;
	// 		}
	// 	}
	// 	distances[i] = cur_min_dist;
	// 	neighbour_i[i] = min_i;
	// }
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
	delete grid;
};
