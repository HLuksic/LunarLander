#pragma once

#include <fstream>
#include <iostream>

class FileHandler
{
public:
	std::fstream file;
	std::string  highScore;

public:
	int ReadOrCreateFile();
	void OverwriteScore(uint32_t score);
};