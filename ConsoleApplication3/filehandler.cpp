#include <fstream>
#include <string>
#include "filehandler.h"

int FileHandler::ReadOrCreateFile()
{
	file.open("highscore.txt", std::ios::in);

	if (file) 
	{
		std::getline(file, highScore);
		file.close();
		file.clear();

		return std::atoi(highScore.c_str());
	}

	file.close();
	file.clear();

	return -1;
}

void FileHandler::OverwriteScore(uint32_t score)
{
	file.open("highscore.txt", std::ios::out | std::ios::trunc);
	file << std::to_string(score);

	file.close();
	file.clear();
}
