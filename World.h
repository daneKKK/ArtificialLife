#pragma once
#include "Creature.h"

class World {
private:
	int width;
	int height;
	Creature*** oldMap;
	Creature*** newMap;
	float** energyDistribution;
public:
	World(int widthInput, int heightInput) {
		width = widthInput;
		height = heightInput;
		oldMap = new Creature * *[height];
		newMap = new Creature * *[height];
		energyDistribution = new float* [height];
		for (int i = 0; i < height; i++) {
			oldMap[i] = new Creature * [width];
			newMap[i] = new Creature * [width];
			energyDistribution[i] = new float[width];
			for (int j = 0; j < width; j++) {
				oldMap[i][j] = NULL;
				newMap[i][j] = NULL;
				energyDistribution[i][j] = 0;
			}
		}
	}
};