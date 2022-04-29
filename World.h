#pragma once
#include "Creature.h"
#include <cmath>
#include <algorithm>

class World {
private:
	int width;
	int height;
	Creature*** oldMap;
	float** energyDistribution;
	float maxEnergy;
	float mutationChance;
	float mutationCoefficient;
public:
	World(int widthInput, int heightInput, float maxEnergyInput, float mutationChanceInput, float mutationCoefficientInput) {
		width = widthInput;
		height = heightInput;
		oldMap = new Creature * *[width];
		energyDistribution = new float* [width];
		for (int i = 0; i < width; i++) {
			oldMap[i] = new Creature * [height];
			energyDistribution[i] = new float[height];
			for (int j = 0; j < width; j++) {
				oldMap[i][j] = NULL;
				energyDistribution[i][j] = 0;
			}
		}
		maxEnergy = maxEnergyInput;
		mutationChance = mutationChanceInput;
		mutationCoefficient = mutationCoefficientInput;
	}
	void placeCreature(Creature* c, int x, int y) {
		oldMap[x][y] = c;
	}
	void changeEnergyDistribution(int timer) {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				energyDistribution[i][j] = 1.5 * pow(std::cos((timer + i) * std::acos(-1) / width), 2);
			}
		}
	}
	void takeOrder(int* dx, int* dy, bool* birth, float* output, int x, int y, int timer) {
		takeOutput(output, x, y, timer);
		*dx = 0;
		if (output[0] <= 0.33) {
			*dx = -1;
		}
		if (output[0] >= 0.66) {
			*dx = 1;
		}
		*dy = 0;
		if (output[1] <= 0.33) {
			*dy = -1;
		}
		if (output[1] >= 0.66) {
			*dy = 1;
		}
		if (output[2] >= 0.5) {
			*birth = true;
		}
	}
	void step(int timer) {
		int* dx;
		int* dy;
		bool* birth;
		float* output = new float[3];
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if (oldMap[i][j] != NULL) {
					if (oldMap[i][j]->getActionTimer()) {
						continue;
					}
					oldMap[i][j]->changeEnergy(-1);
					if (oldMap[i][j]->getEnergy() <= 0) {
						oldMap[i][j]->deleteCreature();
						delete oldMap[i][j];
						oldMap[i][j] = NULL;
					}
					takeOrder(dx, dy, birth, output, i, j, timer);
					oldMap[i][j]->changeEnergy(energyDistribution[i][j]);
					if ((dx != 0) || (dy != 0)) {
						if (birth) {
							oldMap[(i + *dx) % width][(j + *dy) % height] = new Creature(maxEnergy, oldMap[i][j]->getFirstLayerSize(),
								oldMap[i][j]->getInternalLayerAmount(), oldMap[i][j]->getInternalLayerSize(), oldMap[i][j]->getFinalLayerSize(),
								19, oldMap[i][j]->getFirstLayer()[0]->amountOfInputs);
							oldMap[(i + *dx) % width][(j + *dy) % height]->becomeChild(oldMap[i][j], mutationChance, mutationCoefficient);
							oldMap[i][j]->setEnergy(oldMap[i][j]->getEnergy() / 2);
							oldMap[(i + *dx) % width][(j + *dy) % height]->setEnergy(oldMap[i][j]->getEnergy());
						}
						else {
							if (oldMap[(i + *dx) % width][(j + *dy) % height] == NULL) {
								oldMap[(i + *dx) % width][(j + *dy) % height] = oldMap[i][j];
								oldMap[i][j] = NULL;
								oldMap[(i + *dx) % width][(j + *dy) % height]->setActionTimer(true);
							}
							else {
								oldMap[i][j]->changeEnergy(std::min(energyDistribution[(i + *dx) % width][(j + *dy) % height],
									oldMap[(i + *dx) % width][(j + *dy) % height]->getEnergy()));
								oldMap[(i + *dx) % width][(j + *dy) % height]->setEnergy(std::max(oldMap[(i + *dx) % width][(j + *dy) % height]->getEnergy() - energyDistribution[(i + *dx) % width][(j + *dy) % height], (float)0));

							}
						}
					}
					oldMap[i][j]->setEnergy(std::max(maxEnergy, oldMap[i][j]->getEnergy()));
				}
			}
		}
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if (oldMap[i][j] != NULL) {
					oldMap[i][j]->setActionTimer(false);
				}
			}
		}
		changeEnergyDistribution(timer);

	}
	void gatherInputs(float* inputs, int x, int y, int timer) {
		int counter = 0;
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				inputs[counter] = energyDistribution[(x + i) % width][(y + j) % height];
				counter++;
			}
		}
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				if ((j != 0) || (i != 0)) {
					if (oldMap[(x + i) % width][(y + j) % height] != NULL) {
						inputs[counter] = oldMap[(x + i) % width][(y + j) % height]->getSeed();
					}
					else {
						inputs[counter] = 0;
					}
					counter++;
				}
			}
		}
		inputs[17] = std::sin(timer * oldMap[(x) % width][(y) % height]->getOscillatorPeriod() + oldMap[(x) % width][(y) % height]->getOscillatorOffset());
		inputs[18] = oldMap[(x) % width][(y) % height]->getEnergy();
	}
	void takeOutput(float* output, int x, int y, int timer) {
		float* inputs = new float[19];
		gatherInputs(inputs, x, y, timer);
		oldMap[x][y]->calculateOutput(inputs, 19, output);
	}
};