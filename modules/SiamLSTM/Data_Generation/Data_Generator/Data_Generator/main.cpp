#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>       /* time */
#include <ctime>

#define NO_ERROR 0


#define NOTHING -1
#define LEFT 0
#define RIGHT 1
#define UP 2
#define UP_LEFT 3
#define UP_RIGHT 4

int Parse_File(std::string filename, std::vector<int> &sequence)
{
	// Grab sequence from file
	std::ifstream fin;
	fin.open(filename);
	
	if (!fin)
	{
		std::cout << "File not found: " << filename << " - Be sure to include full filepath. Exiting..." << std::endl;
		return 1;
	}
	else
	{
		std::string file_contents;
		while (fin >> file_contents)
		{
			try
			{
				sequence.push_back(std::stoi(file_contents));
			}
			catch (std::exception e)
			{
				std::cout << "Exception occured and sequence not processed: " << e.what() << std::endl;
				return 1;
			}
		}
	}
	fin.close();

	return 0;
}



int Generate_Sequence(const std::vector<int> ref_seq, std::vector<int> &gen_seq)
{

	int random_number = 0;

	for (unsigned int j = 0; j < ref_seq.size(); j++)
	{
		// Loop for sequence

		// Generate random number
		random_number = rand() % 5 + 1;

		switch (random_number)
		{
		case 1:
			// Keep the same
			gen_seq.push_back(ref_seq[j]);
			break;
		case 2:
			// Keep the same
			gen_seq.push_back(ref_seq[j]);
			break;
		case 3:
			// Duplicate of same value
			gen_seq.push_back(ref_seq[j]);
			gen_seq.push_back(ref_seq[j]);
			break;
		case 4:
			// Missing value
			break;
		case 5:
			// Replace left with up left or right with up right
			if (ref_seq[j] == LEFT)
			{
				gen_seq.push_back(UP_LEFT);
			}
			else if (ref_seq[j] == RIGHT)
			{
				gen_seq.push_back(UP_RIGHT);
			}
		}

	}


	return 0;
}


int Output_Sequence(const std::vector<int> gen_seq, std::string filename)
{

	std::string gen_filename = "generated_" + std::to_string(std::time(0)) + "_" + filename + ".txt";

	std::ofstream fout;
	fout.open(gen_filename);

	for (unsigned int i = 0; i < gen_seq.size(); i++)
	{
		fout << gen_seq[i] << std::endl;
	}
	
	fout.close();

	return 0;
}

int main(int argc, char* argv[])
{
	// Input file
	std::string filename = argv[1];
	std::string numberOfGenerations = argv[2];
	std::vector<int> sequence;


	if (Parse_File(filename, sequence) != NO_ERROR)
	{
		return 1;
	}

	// Random Seed
	srand(time(NULL));
	

	std::vector<int> generated_sequence;
	for (unsigned int i = 0; i < std::stoi(numberOfGenerations); i++)
	{
		// Loop for number of files
		std::cout << "Generating " << i << " of " << numberOfGenerations << std::endl;
		Generate_Sequence(sequence, generated_sequence);
		Output_Sequence(generated_sequence, std::to_string(i));
		generated_sequence.clear();
	}

	std::cout << "COMPLETE" << std::endl;

	return 0;
}