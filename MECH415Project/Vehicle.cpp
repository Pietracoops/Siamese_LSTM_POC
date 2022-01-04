#include <iostream>  
#include "Objects.h"
#include <cmath>   
#include <cstdio>  
#include <cstring> 
#include <fstream>   
#include <strstream> 
#include <iomanip>   
#include <windows.h> 
#include "timer.h"  
#include "rotation.h" 
#include "ran.h"
#include <mmsystem.h>

#include <algorithm>

using namespace std;

double sum(std::vector<double> a)
{
	double s = 0;
	for (int i = 0; i < a.size(); i++)
	{
		s += a[i];
	}
	return s;
}

double mean(std::vector<double> a)
{
	return sum(a) / a.size();
}

double sqsum(std::vector<double> a)
{
	double s = 0;
	for (int i = 0; i < a.size(); i++)
	{
		s += pow(a[i], 2);
	}
	return s;
}

double stdev(std::vector<double> nums)
{
	double N = nums.size();
	return pow(sqsum(nums) / N - pow(sum(nums) / N, 2), 0.5);
}

std::vector<double> operator-(std::vector<double> a, double b)
{
	std::vector<double> retvect;
	for (int i = 0; i < a.size(); i++)
	{
		retvect.push_back(a[i] - b);
	}
	return retvect;
}

std::vector<double> operator*(std::vector<double> a, std::vector<double> b)
{
	std::vector<double> retvect;
	for (int i = 0; i < a.size(); i++)
	{
		retvect.push_back(a[i] * b[i]);
	}
	return retvect;
}

double pearsoncoeff(std::vector<double> X, std::vector<double> Y)
{
	return sum((X - mean(X))*(Y - mean(Y))) / (X.size()*stdev(X)* stdev(Y));
}

Deviation::Deviation()
{

}

Deviation::~Deviation()
{

}



Vehicle::Vehicle(char* Name, int &errorcode)
{

	int n = strlen(Name);

	this->Name = new char[n+1];

	if (this->Name == NULL)
	{
		errorcode = 1;
		return;
	}

	strcpy(this->Name, Name);
	VehicleMesh = new mesh(this->Name);

	Pos = new double[3];
	Vel = new double[3];
	Rot = new double[3];

	if (Pos == NULL || Vel == NULL|| VehicleMesh == NULL || Rot == NULL)
	{
		errorcode = 2;
		return;
	}


	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			MeshExtremities[i][j] = 0.0;
		}
	}



	for (int i = 0; i < 3; i++)
	{
		Pos[i] = 0;
		Vel[i] = 0;
		Rot[i] = 0;
	}

	Length = 750;
	Width = 350;
	theta = 0;
	friction = 1;
	dforce = 0.03;
	FRight = 0;
	FLeft = 0;
	Pos[2] = 0.9;

	t0 = high_resolution_time();
	t_sim = 0.0;
	t_sim2 = 0.0;
	current_command = 0;
}

Vehicle::~Vehicle()
{

	if (Name != NULL)
	{
		delete Name;
		Name = NULL;
	}
	if (Pos != NULL)
	{
		delete Pos;
		Pos = NULL;
	}
	if (Vel != NULL)
	{
		delete Vel;
		Vel = NULL;
	}
	if (Rot != NULL)
	{
		delete Rot;
		Rot = NULL;
	}
	if (VehicleMesh != NULL)
	{
		delete VehicleMesh;
		VehicleMesh = NULL;
	}

}


double & Vehicle::get_P(int i)
{
	if (i > 2 || i < 0)	exit(1);

	return Pos[i];
}

double & Vehicle::get_V(int i)
{
	if (i > 2 || i < 0)	exit(1);

	return Vel[i];
}

double & Vehicle::get_R(int i)
{
	if (i > 2 || i < 0)		exit(1);

	return Rot[i];
}


double & Vehicle::get_Length()
{
	
	return Length;
}


double & Vehicle::get_Width()
{

	return Width;
}

void Vehicle::set_Scale(double Scale)
{
	//Set the Scale of the mesh
	Length *= Scale;
	Width *= Scale;

	VehicleMesh->Scale = Scale;

}

double Vehicle::get_theta()
{
	return theta;
}

void Vehicle::get_highscore(int &errorcode)
{
	static char file_name[50] = "highscore.txt";
	static ifstream fin;
	static ofstream fout;

	fin.open(file_name);

	// if file doesn't exist start a new count
	if (!fin) {
		//file doesn't exist create file for first time
		highscore = 9.99E300;
		fout.open(file_name);
		if (!fout) errorcode = 5;
		fout << highscore;
		fout.close();
	}
	else {
		fin >> highscore;
	}
	fin.close();
}

void Vehicle::store_highscore(double highscore, double Restart, int &errorcode)
{
	static char file_name[50] = "highscore.txt";
	static ofstream fout;

	if (Restart == 2)//Pickup chest
	{
		fout.open(file_name);

		if (!fout){
			errorcode = 6;
		}
		else {
			fout << highscore;
		}
		fout.close();

	}

	static char input_history[50] = "input.txt";
	static std::string input_history_str;

	
	if (Restart == 1 || Restart == 2)//crashed
	{
		input_history_str = "Inputs/inputs_" + std::to_string(std::time(0)) + "_" + std::to_string((int)Restart) + ".txt";
		fout.open(input_history_str);
		if (!fout){
			errorcode = 7;
		}
		else {
			fout << "Course Completed," << (int)Restart << endl;
			for (unsigned int i = 0; i < nav_system_obj.command_list.size(); i++)
			{
				fout << nav_system_obj.command_list[i] << endl;
				//fout << nav_system_obj.time[i] << "," << nav_system_obj.keypress_left[i] << "," << nav_system_obj.keypress_up[i] << "," << nav_system_obj.keypress_right[i] << "," << nav_system_obj.Pos_x[i] << "," << nav_system_obj.Pos_y[i] << "," << nav_system_obj.Vel_x[i] << "," << nav_system_obj.Vel_y[i] << endl;
			}

		}
		fout.close();
		//Clear vars
		nav_system_obj.time.clear();
		nav_system_obj.keypress_left.clear();
		nav_system_obj.keypress_right.clear();
		nav_system_obj.keypress_up.clear();
		nav_system_obj.Pos_x.clear();
		nav_system_obj.Pos_y.clear();
		nav_system_obj.Vel_x.clear();
		nav_system_obj.Vel_y.clear();

		nav_system_obj.command_list.clear();
	}

}


void Vehicle::draw()
{
	update_meshextremities();
	VehicleMesh->draw(Pos[0], Pos[1], Pos[2], Rot[0], Rot[1], Rot[2]);

}

double & Vehicle::get_friction()
{
	return friction;
}


void Vehicle::set_music(char* wavfile)
{

	PlaySound(TEXT(wavfile), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
}


void Vehicle::sim_step(double dt,double t, int &restart)
{

	if (restart == 1 || restart == 2)
	{
		t_sim = 0;
		t_sim2 = 0;
	}

	if (t - t_sim2 >= 0.5)
	{
		current_command = nav_system_obj.generate_command_list(t_sim);
		nav_system_obj.command_list.push_back(current_command);
		t_sim2 += 0.5;
	}


	if (t - t_sim >= dt)
	{	

		// Snapshot of statistics for Data collection
		nav_system_obj.Pos_x.push_back(Pos[0]);
		nav_system_obj.Pos_y.push_back(Pos[1]);
		nav_system_obj.Vel_x.push_back(Vel[0]);
		nav_system_obj.Vel_y.push_back(Vel[1]);
		nav_system_obj.time.push_back(t_sim);

		if (KEY(VK_LEFT))
		{
			nav_system_obj.keypress_left.push_back(1);
			nav_system_obj.keypress_right.push_back(0);
			nav_system_obj.keypress_up.push_back(0);
			if (FRight < 50) FRight += 4*dforce;//increase
			if (FLeft > 0) FLeft -= friction*dforce;//decreasing the power of the other one
			if (FLeft < 0) FLeft = 0;//do not exceed 0
		}
		else if (KEY(VK_RIGHT))
		{
			nav_system_obj.keypress_left.push_back(0);
			nav_system_obj.keypress_right.push_back(1);
			nav_system_obj.keypress_up.push_back(0);
			if (FLeft < 50) FLeft += 4 * dforce;
			if (FRight > 0) FRight -= friction*dforce;
			if (FRight < 0) FRight = 0;
		}
		else if (KEY(VK_UP))
		{
			nav_system_obj.keypress_left.push_back(0);
			nav_system_obj.keypress_right.push_back(0);
			nav_system_obj.keypress_up.push_back(1);
			//Both engines at same time
			if (FRight <= FLeft)
				if (FRight < 50) FRight += 4 * dforce;
			
			if (FLeft <= FRight)
				if (FLeft < 50) FLeft += 4 * dforce;
		}
		else
		{
			nav_system_obj.keypress_left.push_back(0);
			nav_system_obj.keypress_right.push_back(0);
			nav_system_obj.keypress_up.push_back(0);
			//Decrement until reaches 0
			if (FRight > 0)	FRight -= friction*dforce;
			if (FLeft > 0)	FLeft -= friction*dforce;
			if (FRight < 0) FRight = 0;
			if (FLeft < 0) FLeft = 0;
		}


		Euler(FLeft, FRight, dt);//Call Euler Functions
		
		t_sim += dt;//Increment simulation time
	}

}

void Vehicle::Euler(double F1, double F2, double dt)
{

	static double c1 = 0.0, c2 = 0.0, c3 = 0.0;
	static double M = 0.085;//grams
	static double u = 0, v = 0, udot = 0, vdot = 0;
	static double deltatheta = 0, rdot = 0, r = 0;
	static double J = 0.05;//moment of inertia (kg*m^2)
	static double theta = 0.0;
	static double sim_time = 0;
	static double length = 5;


		udot = ((F1 + F2 - c1*u) / M) + v*r;
		vdot = ((-c2*v) / M) - u*r;
		rdot = ((length / 2)*(F1 - F2) - c3*r) / J;

		u = udot*dt;
		v = vdot*dt;
		r = rdot*dt;

		//Conversion to Global Coordinates
		theta += r*dt;
		Vel[0] = cos(theta)*u - sin(theta)*v;
		Vel[1] = sin(theta)*u + cos(theta)*v;

		this->theta = theta;

		//Velocity to Position
		Pos[0] += Vel[0]*dt;
		Pos[1] += Vel[1]*dt;

}




void Vehicle::set_camera_view(int &i)
{
	static double time0 = high_resolution_time();
	static double timenow;
	static double stalltime = 0.5;
	static double LastPressTime;
	timenow = high_resolution_time() - time0;

	if (timenow - LastPressTime > stalltime)
	{
		if (KEY('C'))
		{
			i++;
			LastPressTime = high_resolution_time() - time0;
		}
	}


	if (i > 3) i = 1;//total of 2 camera views

	if (i == 1)
	{
		//first person

		eye_point_g[1] = Pos[0];
		eye_point_g[2] = Pos[1];
		eye_point_g[3] = 0.9;

		lookat_point_g[1] = (10 * cos(theta)) + Pos[0];
		lookat_point_g[2] = (10 * sin(theta)) + Pos[1];
		lookat_point_g[3] = 0.0;

		up_dir_g[1] = 0.0;
		up_dir_g[2] = 0.0;
		up_dir_g[3] = 1.5;

		set_view(eye_point_g, lookat_point_g, up_dir_g);
	}
	else if (i == 2)
	{
		//3rd person


		eye_point_g[1] = Pos[0] + (-6 * cos(-theta));
		eye_point_g[2] = Pos[1] + (6 * sin(-theta));
		eye_point_g[3] = 3;

		lookat_point_g[1] = 0.0 + Pos[0];
		lookat_point_g[2] = 0.0 + Pos[1];
		lookat_point_g[3] = 2.0;
		
		up_dir_g[1] = 0.0;
		up_dir_g[2] = 0.0;
		up_dir_g[3] = 1.0;


		set_view(eye_point_g, lookat_point_g, up_dir_g);
	}
	else if (i == 3)
	{
		//controllable view to see top view of maze
		//Q = +X    |   A = -X
		//W = +Y    |   S = -Y
		//E = +Z    |   D = -Z
		//R = +Dia  |   F = -Dia
		set_view();
	}

}

void Vehicle::update_meshextremities()
{

	//Front of Hovercraft
	MeshExtremities[0][0] = Pos[0] + (Length / 2)*cos(theta);
	MeshExtremities[0][1] = Pos[1] + (Length / 2)*sin(theta);	

	//Top Right Hovercraft
	MeshExtremities[1][0] = Pos[0] + (Length / 2 - (Length* 0.3733))*sin(theta);
	MeshExtremities[1][1] = Pos[1] - (((0.8*Width) / 2))*cos(theta);

	//Bottom Right Hovercraft
	MeshExtremities[2][0] = Pos[0] - sqrt(((Length / 2)*(Length / 2) + (Width / 2)*(Width / 2)))*cos(theta + 2*PI/14.4);
	MeshExtremities[2][1] = Pos[1] - sqrt(((Length / 2)*(Length / 2) + (Width / 2)*(Width / 2)))*sin(theta + 2*PI / 14.4);

	//Bottom Left Hovercraft
	MeshExtremities[3][0] = Pos[0] - sqrt(((Length / 2)*(Length / 2) + (Width / 2)*(Width / 2)))*sin((theta + 2*PI / 5.538));
	MeshExtremities[3][1] = Pos[1] + sqrt(((Length / 2)*(Length / 2) + (Width / 2)*(Width / 2)))*cos((theta + 2 * PI / 5.538));

	//Top Left Hovercraft
	MeshExtremities[4][0] = Pos[0] + (Length / 2 - (Length* 0.3733))*sin(-theta);
	MeshExtremities[4][1] = Pos[1] + (((0.8*Width) / 2))*cos(-theta);

}

double & Vehicle::get_meshextremities(int i, int j)
{

	if (i > 4 || i < 0) exit(1);
	if (j > 2 || j < 0) exit(1);

	return MeshExtremities[i][j];
}


int collision(Vehicle & Hovercraft, Walls* Wall[],int NumberOfWalls)
{


	for (int i = 0; i < NumberOfWalls - 1; i++)
	{
		

		//Point 1 
		if (Hovercraft.get_meshextremities(0, 0) > Wall[i]->get_meshextremities(0, 0) && Hovercraft.get_meshextremities(0, 0) < Wall[i]->get_meshextremities(0, 1) && \
			Hovercraft.get_meshextremities(0, 1) > Wall[i]->get_meshextremities(1, 0) && Hovercraft.get_meshextremities(0, 1) < Wall[i]->get_meshextremities(1, 1))
		{
			//Point 1 is within restricted area
			Hovercraft.FLeft = 0;
			Hovercraft.FRight = 0;
			

			Hovercraft.get_P(0) = 0;
			Hovercraft.get_P(1) = 0;
			return 1;
			
		}

		
		//Point 2 
		if (Hovercraft.get_meshextremities(1, 0) > Wall[i]->get_meshextremities(0, 0) && Hovercraft.get_meshextremities(1, 0) < Wall[i]->get_meshextremities(0, 1) && \
			Hovercraft.get_meshextremities(1, 1) > Wall[i]->get_meshextremities(1, 0) && Hovercraft.get_meshextremities(1, 1) < Wall[i]->get_meshextremities(1, 1))
		{
			//Point 2 is within restricted area
			Hovercraft.FLeft = 0;
			Hovercraft.FRight = 0;

			Hovercraft.get_P(0) = 0;
			Hovercraft.get_P(1) = 0;
			return 1;
		}


		//Point 3 
		if (Hovercraft.get_meshextremities(2, 0) > Wall[i]->get_meshextremities(0, 0) && Hovercraft.get_meshextremities(2, 0) < Wall[i]->get_meshextremities(0, 1) && \
			Hovercraft.get_meshextremities(2, 1) > Wall[i]->get_meshextremities(1, 0) && Hovercraft.get_meshextremities(2, 1) < Wall[i]->get_meshextremities(1, 1))
		{
			//Point 3 is within restricted area
			Hovercraft.FLeft = 0;
			Hovercraft.FRight = 0;

			Hovercraft.get_P(0) = 0;
			Hovercraft.get_P(1) = 0;
			return 1;
		}


		//Point 4 
		if (Hovercraft.get_meshextremities(3, 0) > Wall[i]->get_meshextremities(0, 0) && Hovercraft.get_meshextremities(3, 0) < Wall[i]->get_meshextremities(0, 1) && \
			Hovercraft.get_meshextremities(3, 1) > Wall[i]->get_meshextremities(1, 0) && Hovercraft.get_meshextremities(3, 1) < Wall[i]->get_meshextremities(1, 1))
		{
			//Point 4 is within restricted area
			Hovercraft.FLeft = 0;
			Hovercraft.FRight = 0;

			Hovercraft.get_P(0) = 0;
			Hovercraft.get_P(1) = 0;
			return 1;
		}

		//Point 5 
		if (Hovercraft.get_meshextremities(4, 0) > Wall[i]->get_meshextremities(0, 0) && Hovercraft.get_meshextremities(4, 0) < Wall[i]->get_meshextremities(0, 1) && \
			Hovercraft.get_meshextremities(4, 1) > Wall[i]->get_meshextremities(1, 0) && Hovercraft.get_meshextremities(4, 1) < Wall[i]->get_meshextremities(1, 1))
		{
			//Point 5 is within restricted area
			Hovercraft.FLeft = 0;
			Hovercraft.FRight = 0;

			Hovercraft.get_P(0) = 0;
			Hovercraft.get_P(1) = 0;
			return 1;
		}

	}

	//CHECKING FOR CHEST

	//Point 1 
	if (Hovercraft.get_meshextremities(0, 0) > Wall[30]->get_meshextremities(0, 0) && Hovercraft.get_meshextremities(0, 0) < Wall[30]->get_meshextremities(0, 1) && \
		Hovercraft.get_meshextremities(0, 1) > Wall[30]->get_meshextremities(1, 0) && Hovercraft.get_meshextremities(0, 1) < Wall[30]->get_meshextremities(1, 1))
	{
		//Point 1 is within restricted area
		Hovercraft.FLeft = 0;
		Hovercraft.FRight = 0;

		Hovercraft.get_P(0) = 0;
		Hovercraft.get_P(1) = 0;

		return 2;

	}


	//Point 2 
	if (Hovercraft.get_meshextremities(1, 0) > Wall[30]->get_meshextremities(0, 0) && Hovercraft.get_meshextremities(1, 0) < Wall[30]->get_meshextremities(0, 1) && \
		Hovercraft.get_meshextremities(1, 1) > Wall[30]->get_meshextremities(1, 0) && Hovercraft.get_meshextremities(1, 1) < Wall[30]->get_meshextremities(1, 1))
	{
		//Point 2 is within restricted area
		Hovercraft.FLeft = 0;
		Hovercraft.FRight = 0;

		Hovercraft.get_P(0) = 0;
		Hovercraft.get_P(1) = 0;

		return 2;
	}


	//Point 3 
	if (Hovercraft.get_meshextremities(2, 0) > Wall[30]->get_meshextremities(0, 0) && Hovercraft.get_meshextremities(2, 0) < Wall[30]->get_meshextremities(0, 1) && \
		Hovercraft.get_meshextremities(2, 1) > Wall[30]->get_meshextremities(1, 0) && Hovercraft.get_meshextremities(2, 1) < Wall[30]->get_meshextremities(1, 1))
	{
		//Point 3 is within restricted area
		Hovercraft.FLeft = 0;
		Hovercraft.FRight = 0;

		Hovercraft.get_P(0) = 0;
		Hovercraft.get_P(1) = 0;

		return 2;
	}


	//Point 4 
	if (Hovercraft.get_meshextremities(3, 0) > Wall[30]->get_meshextremities(0, 0) && Hovercraft.get_meshextremities(3, 0) < Wall[30]->get_meshextremities(0, 1) && \
		Hovercraft.get_meshextremities(3, 1) > Wall[30]->get_meshextremities(1, 0) && Hovercraft.get_meshextremities(3, 1) < Wall[30]->get_meshextremities(1, 1))
	{
		//Point 4 is within restricted area
		Hovercraft.FLeft = 0;
		Hovercraft.FRight = 0;

		Hovercraft.get_P(0) = 0;
		Hovercraft.get_P(1) = 0;

		return 2;
	}

	//Point 5 
	if (Hovercraft.get_meshextremities(4, 0) > Wall[30]->get_meshextremities(0, 0) && Hovercraft.get_meshextremities(4, 0) < Wall[30]->get_meshextremities(0, 1) && \
		Hovercraft.get_meshextremities(4, 1) > Wall[30]->get_meshextremities(1, 0) && Hovercraft.get_meshextremities(4, 1) < Wall[30]->get_meshextremities(1, 1))
	{
		//Point 5 is within restricted area
		Hovercraft.FLeft = 0;
		Hovercraft.FRight = 0;

		Hovercraft.get_P(0) = 0;
		Hovercraft.get_P(1) = 0;

		return 2;
	}



	return 0;
}


void Display_Instructions(double t, int &Restart, double &tfinal, int &current_command)
{

	//Restart 0 = No Collision  |  Restart 1 = Wall Collision  |  Restart 3 = Chest Collision
	if (Restart == 1 || Restart == 0)
	{
		if (t < 5.0)
		{
			text_xy("Complete the Maze as fast as possible!", 650, 500, 30);
		}
		if (t > 5.0 && t < 10.0)
		{
			text_xy("Don't touch the walls or you will have to restart", 650, 500, 30);
		}

		text_xy("Best Time:" , 900, 100, 20);

		if (tfinal == 9.99E300)
		{
			text_xy(tfinal, 900, 150, 20);
		}
		else
		{
			text_xy(tfinal, 900, 150, 20);
		}


		if (current_command == 0)
		{
			text_xy("LEFT", 650, 600, 30);
		}
		else if (current_command == 1)
		{
			text_xy("RIGHT", 650, 600, 30);
		}
		else if (current_command == 2)
		{
			text_xy("UP", 650, 600, 30);
		}
		else if (current_command == 3)
		{
			text_xy("UP_LEFT", 650, 600, 30);
		}
		else if (current_command == 4)
		{
			text_xy("UP_RIGHT", 650, 600, 30);
		}
		else
		{
			text_xy("NOTHING", 650, 600, 30);
		}
		
		Restart = 0;
	}
	if (Restart == 2)
	{
		if (tfinal == 9.99E300)
		{
			text_xy(0.0, 900, 150, 20);
		}
		else
		{
			text_xy(tfinal, 900, 150, 20);
		}

		Restart = 0;
	}

	text_xy(t, 100, 100, 30);   // Display Time
}

void Display_Error(int ErrorCode)
{

	//Displaying Error Message to Screen

	if (ErrorCode == 1)
	{
		text_xy("Dynamic Memory error for Vehicle Name", 100, 900, 30);
	}
	else if (ErrorCode == 2)
	{
		text_xy("Dynamic Memory error for Vehicle Position, Velocity, Rotation, and/or Mesh", 100, 900, 30);
	}
	else if (ErrorCode == 3)
	{
		text_xy("Dynamic Memory error for Wall Name", 100, 900, 30);
	}
	else if (ErrorCode == 4)
	{
		text_xy("Dynamic Memory error for Wall Position, Velocity, Rotation, and/or Mesh", 100, 900, 30);
	}
	else if (ErrorCode == 5)
	{
		text_xy("Get_highscore: Unable to create highscore databse - FileIO error", 100, 900, 30);
	}
	else if (ErrorCode == 6)
	{
		text_xy("Store_highscore: Unable to store highscore into databse - FileIO error", 100, 900, 30);
	}
	else if (ErrorCode == 7)
	{
		text_xy("Store_highscore: Unable to store input into databse - FileIO error", 100, 900, 30);
	}

}
