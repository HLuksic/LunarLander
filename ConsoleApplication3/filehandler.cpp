#include <fstream>
#include <iostream>
#include <string>
#include "filehandler.h"

int FileHandler::ReadOrCreateFile()
{
	file.open("highscore.txt", std::ios::in);

	if (file) {
		std::getline(file, highScore);
		file.close();
		return std::atoi(highScore.c_str());
	}
	file.close();
	return -1;
}

void FileHandler::OverwriteScore(int score)
{
	char* scoreString = new char[20];
	_itoa_s(score, scoreString, 20, 10);

	file.open("highscore.txt", std::ios::out | std::ios::trunc);
	file << scoreString;

	file.close();
}
