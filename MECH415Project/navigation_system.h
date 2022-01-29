#include <vector>
#include <fstream>
#define LEFT 0
#define RIGHT 1
#define UP 2
#define UP_LEFT 3
#define UP_RIGHT 4
#define NOTHING -1

class nav_system
{

	int last_index;
	float left = 0;
	float right = 0;
	float up = 0;
public:

	std::vector<float> time;
	std::vector<int> keypress_left;
	std::vector<int> keypress_up;
	std::vector<int> keypress_right;
	std::vector<float> Pos_x;
	std::vector<float> Pos_y;
	std::vector<float> Vel_x;
	std::vector<float> Vel_y;

	std::vector<int> command_list;
	std::vector<int> command_list_size;

	int generate_command_list(float timestamp);

	nav_system();
	~nav_system();


};