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
}


void Dots::count_distance_base()
{
	// th_f (dots), len(dots): th_n ; start_i end_i
	for (int i=0; i<dots.size(); i++)
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
		distances.push_back(cur_min_dist);
		neighbour_i.push_back(min_i);
	}

}


// std::vector<Dot> dots;
// std::vector<float> distances;
// std::vector<int> neighbour_i;
// {1, 2}; {4, 2}; {9, 1}



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
