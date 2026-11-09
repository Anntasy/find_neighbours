#ifndef GRID_HPP
#define GRID_HPP


#include "h.hpp"

class Dot;
class Grid
{
public:
	Grid(int n=10);
	~Grid();

	void parse(std::string file_name);

	std::pair<int, int> get_grid_coords(const Dot & d);
	std::pair<int, int> get_grid_coords(float x, float y);
	void add_els(const Dot & d);
	void add_els(float x, float y);
	void j_to_i(int n_x, int n_y, int j, int &i);
	void i_to_j(int &n_x, int &n_y, int &j, int i); // void i_to_j(int *nx, int *ny, int *j, int i);

	void print_mp();

	std::vector<Dot> &get_dots(int i_x, int i_y);

	std::vector<int> n_el_s_row;
	std::map<std::pair<int, int>, std::vector<Dot>> mp;
	int n_;
	float s_;
	int c;
	const float x_min = -10;
	const float y_min = -10;
	const float x_max = 10;
	const float y_max = 10;

	std::pair<int, int> start;
	std::pair<int, int> end;

};

#endif