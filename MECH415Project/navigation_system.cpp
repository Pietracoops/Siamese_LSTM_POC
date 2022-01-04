#include "navigation_system.h"

nav_system::nav_system()
{
	last_index = 0;
}

nav_system::~nav_system()
{

}

int nav_system::generate_command_list(float timestamp)
{

	left = 0;
	right = 0;
	up = 0;
	for (unsigned int i = last_index; i < time.size(); i++)
	{
		left += keypress_left[i];
		right += keypress_right[i];
		up += keypress_up[i];		
	}

	last_index = time.size();

	if (left * up != 0 && right * up == 0)
	{
		command_list.push_back(UP_LEFT);
		return UP_LEFT;
	}
	else if (left * up == 0 && right * up != 0)
	{
		command_list.push_back(UP_RIGHT);
		return UP_RIGHT;
	}
	else if (left > right && left > up)
	{
		command_list.push_back(LEFT);
		return LEFT;
	}
	else if (right > left && right > up)
	{
		command_list.push_back(RIGHT);
		return RIGHT;
	}
	else if (up > left && up > right)
	{
		command_list.push_back(UP);
		return UP;
	}
	else
	{
		command_list.push_back(NOTHING);
		return NOTHING;
	}

}