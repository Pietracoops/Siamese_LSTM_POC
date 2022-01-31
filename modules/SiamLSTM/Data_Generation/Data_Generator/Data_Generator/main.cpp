#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>       /* time */
#include <ctime>
#include <filesystem>

namespace fs = std::filesystem;

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


int File_Generator(std::string filename, std::string numberOfGenerations)
{

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

	return 0;
}


int Get_Files(std::string &path, std::vector<std::vector<std::filesystem::path>> &allFolders)
{

	std::vector<std::filesystem::path> rootFoldersVector;
	std::vector<std::filesystem::path> FilesVector;

	for (const auto& entry : fs::directory_iterator(path))
	{
		std::cout << entry.path() << std::endl;
		// We are in folder now
		rootFoldersVector.push_back(entry.path());
	}


	for (unsigned int i = 0; i < rootFoldersVector.size(); i++)
	{

		std::string tmpstring = rootFoldersVector[i].u8string();

		std::size_t found = tmpstring.find("READ");
		if (found != std::string::npos) continue;

		for (const auto& subentry : fs::directory_iterator(tmpstring))
		{
			FilesVector.push_back(subentry.path());
		}

		allFolders.push_back(FilesVector);
		FilesVector.clear();
	}

	return 0;
}


int Make_Pairs(std::vector<std::vector<std::filesystem::path>> allFolders, int folder1, int folder2, int same, int Filesize, std::string outputName, bool varying_lengths)
{

	std::vector<int> file1;
	std::vector<int> file2;
	std::string str1;
	std::string str2;

	std::ofstream fout;
	int random_number;


	int maxSize = 0;


	fout.open(outputName, std::ios_base::app); // append instead of overwrite

	for (unsigned int i = 0; i < Filesize; i++)
	{
		if (i == 100)
		{
			std::cout << "hello" << std::endl;
		}

		std::cout << "Iteration: " << i << "  of " << Filesize << std::endl;

		// Parse file 1
		Parse_File(allFolders[folder1][i].u8string(), file1);

		// Parse file 2
		Parse_File(allFolders[folder2][i].u8string(), file2);

		// Get Max size
		if (file1.size() > file2.size())
		{
			maxSize = file1.size();
		}
		else
		{
			maxSize = file2.size();
		}

		// convert to string values
		for (unsigned int j = 0; j < file1.size(); j++)
		{
			str1 = str1 + std::to_string(file1[j]) + " ";
		}


		if (varying_lengths)
		{
			// Generate random number
			random_number = rand() % file2.size() + 1;
			if (random_number < 5)
			{
				random_number = 5;
			}
		}
		else
		{
			random_number = file2.size();
		}


		for (unsigned int j = 0; j < random_number; j++)
		{
			str2 = str2 + std::to_string(file2[j]) + " ";
		}


		// Append it to the current file
		//std::cout << str1 << std::endl;
		//std::cout << str2 << std::endl;


		fout << str1 << "," << str2 << "," << maxSize << "," << same << std::endl;
		
		file1.clear();
		file2.clear();

		str1.clear();
		str2.clear();
	}


	fout.close();

	return 0;
}


int database_pairs_creator()
{
	
	int Filesize = 1000;
	std::string outputName = "Database.csv";
	std::string path = "H:\\MassFiles\\Masters\\AIPilot\\Massimo\\Siamese_LSTM_POC\\modules\\SiamLSTM\\Data_Generation\\Data_Generator\\Inputs\\Generated";

	std::vector<std::vector<std::filesystem::path>> allFolders;


	Get_Files(path, allFolders);

	Make_Pairs(allFolders, 0, 1, 1, Filesize, outputName, true);  // Good
	Make_Pairs(allFolders, 2, 5, 1, Filesize, outputName, false); // Good
	Make_Pairs(allFolders, 2, 3, 0, Filesize, outputName, false); // Bad
	Make_Pairs(allFolders, 5, 4, 0, Filesize, outputName, false); // Bad
	Make_Pairs(allFolders, 0, 3, 0, Filesize, outputName, false); // Bad
	Make_Pairs(allFolders, 1, 4, 0, Filesize, outputName, false); // Bad


	std::cout << "Done" << std::endl;

	return 0;
}



int main(int argc, char* argv[])
{
	// Input file

	std::string option;
	std::string filename;
	std::string numberOfGenerations;

	if (argc != 1)
	{
		option = argv[1];

		if (std::stoi(option) == 0)
		{
			filename = argv[2];
			numberOfGenerations = argv[3];
		}
	}
	else
	{
		option = "1";
	}
	



	if (std::stoi(option) == 0)
	{
		File_Generator(filename, numberOfGenerations);
	}
	else if (std::stoi(option) == 1)
	{
		database_pairs_creator();
	}
	else
	{
		std::cout << "Invalid options." << std::endl;
	}
	


	std::cout << "COMPLETE" << std::endl;

	return 0;
}