#ifndef OBJECTS_H // header guards
#define OBJECTS_H

#include "3D_graphics.h"
#include "navigation_system.h"
#include <cmath>
#include <vector>
#include <ctime>
#include <string>
#include "../modules/Named_Pipes_Multithread/named_pipes/named_pipes/pipes.h"		   // IPC commands to commmunicate with python

#include "../Reading_Program_Shared_Memory/program_B/shared_memory.h"	
#include "../Reading_Program_Shared_Memory/program_B/SharedMemory.h"	

static Pipes_Server Ipcserver("\\\\.\\pipe\\Foo");
const double PI = 4 * atan(1.0);

static SharedMemory SM("shared_memory1", 100, 0, 2);			              // Block of shared memory (size = n, 0 = queue number, 2 = number of programs in queue (from 0 to 1))

class Vehicle
{

	char* Name;

	int nav_size;

	double dt;
	double* Pos;
	double* Rot;
	double* Vel;
	double Length;
	double Width;
	double  t, t0, t_sim, t_sim2;
	double MeshExtremities[8][2];
	double friction;
	double theta;
	double eye_point_1[3 + 1], lookat_point_1[3], up_dir_1[3 + 1];
	double eye_point_g[3 + 1], lookat_point_g[3], up_dir_g[3 + 1];

	mesh * VehicleMesh;

	nav_system nav_system_obj;

public:
	double FRight, FLeft;
	double dforce;
	double highscore;

	int current_command;

	Vehicle(char * Name, int &errorcode);
	~Vehicle();
	
	double & get_R(int i);
	double & get_P(int i);
	double & get_V(int i);
	double & get_Length();
	double & get_Width();
	double get_theta();
	double & get_friction();
	double & get_meshextremities(int i, int j);

	void set_starting_pos();
	void set_theta(double input);
	void set_music(char* wavfile);
	void set_Scale(double Scale);
	void draw();
	void sim_step(double dt,double t, int &restart);
	void Euler(double F1, double F2,double dt);
	void set_camera_view(int &i);
	void update_meshextremities();
	void get_highscore(int &errorcode);
	void store_highscore(double highscore, double Restart,int &errorcode);
};



class Walls
{

	char* Name;

	bool Vertical;

	double* Pos;
	double* Rot;
	double Length;
	double Thickness;
	double MeshExtremities[2][2];

	mesh * WallMesh;

public:

	Walls(char * Name, int &errorcode);
	~Walls();

	double & get_R(int i);
	double & get_P(int i);
	double & get_Length();
	double & get_Width();
	double & get_meshextremities(int i, int j);

	bool & Walls::set_Vertical();

	void set_Scale(double Scale);
	void draw();
	void update_meshextremities();
};


class Deviation
{


public:

	Deviation();
	~Deviation();


};


#endif