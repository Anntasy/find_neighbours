#include "Dots.hpp"
#include "h.hpp"


Dots::Dots()
{
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
};
