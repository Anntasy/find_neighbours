#include "h.hpp"
# define DATA_NAME "data/data100_000.txt"


// GITHUB
// dots grid n_size; задавать оптимальное значение автоматически
// dots + threads
// 

void test1()
{
	Dots dots(100);
	dots.parse_grid(DATA_NAME);
	// dots.grid->print_mp();
	std::cout << "_____" << "\n";
	dots.count_distance_base_grid();
	// dots.print();
}


void test2()
{
	Dots dots;
	dots.parse(DATA_NAME);
	std::cout << "_____" << "\n";
	dots.count_distance_base();
	// dots.print();
}


void test3()
{
	Dots dots;
	dots.parse(DATA_NAME);
	std::cout << "_____" << "\n";
	dots.count_distance_threads();
	// dots.print();
}


int main()
{
	// std::cout << "_____" << "\n";
	// {
	// 	OurTimer t("count_distance_threads");
	// 	test3();
	// }

	std::cout << "_____" << "\n";
	{
		OurTimer t("count_distance_base_grid");
		test1();
	}
}


// int main()
// {
// 	std::cout << boost::thread::hardware_concurrency(); 
// 	// The number of physical cores available on the current system
// 	// GITHUB!!!!!!!!!!!!!!
// }
