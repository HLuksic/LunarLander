#include <fstream>
#include <string>
#include "filehandler.h"

std::string FileHandler::ReadOrCreateFile()
{
	file.open("highscore.txt", std::ios::in);

	if (file) 
	{
		std::getline(file, highScore);
		file.close();
		file.clear();

		return highScore;
	}

	file.close();
	file.clear();

	return "0";
}

void FileHandler::OverwriteScore(uint32_t score)
{
	file.open("highscore.txt", std::ios::out | std::ios::trunc);
	file << std::to_string(score);

	file.close();
	file.clear();
}
