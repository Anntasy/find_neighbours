#ifndef DOTS_HPP
#define DOTS_HPP


#include "h.hpp"


class OurTimer
{
public:
	OurTimer(std::string name): name_(std::move(name)), start_(std::chrono::steady_clock::now()) {}
	~OurTimer() {
		const auto end = std::chrono::steady_clock::now();
		const auto res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);
		std::cout << name_ << ": " << res << '\n';
	}
	std::string name_;
	std::chrono::steady_clock::time_point start_;
};

struct Dot
 {
 	float x;
 	float y;
 };

class Grid;
class Dots
{
public:
	std::vector<Dot> dots;
	std::vector<float> distances;
	std::vector<int> neighbour_i;
	Grid *grid; // grid * grid (dynamic)

	Dots();
	Dots(int n_s);
	~Dots();

	void parse(std::string file_name);
	void parse_grid(std::string file_name);

	void count_distance_base();
	void count_distance_threads();
	void count_distance_base_grid();
	void get_neighbours(int start_i, int end_i);
	void print();

	// S neighbours vector int
	// method parsing (file format: 1 2.2 /n 3 4.0)
	// print
	// solve

	// O(N^2); check
	// ||;
	// #
	// || + #
	// [_] (+ ||) ??
};

#endif