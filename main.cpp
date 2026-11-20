#include "h.hpp"
# define DATA_NAME "data/data100.txt"


// GITHUB
// dots grid n_size; задавать оптимальное значение автоматически
// dots + threads
// 

void test1()
{
	Dots dots(Mode::Grid, 1000000);
	dots.parse(DATA_NAME);
	// dots.grid->print_mp();
	std::cout << "_____" << "\n";
	dots.count_distance();
	// dots.print();
}


void test4()
{
	Dots dots(Mode::Grid_Thread, 100);
	dots.parse(DATA_NAME);
	// dots.grid->print_mp();
	std::cout << "_____" << "\n";
	// dots.grid->debug_print("log.log");
	dots.count_distance();
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
		OurTimer t("count_distance_base_grid_threads");
		test4();
	}
}


// int main()
// {
// 	std::cout << boost::thread::hardware_concurrency(); 
// 	// The number of physical cores available on the current system
// 	// GITHUB!!!!!!!!!!!!!!
// }
