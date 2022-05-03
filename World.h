#pragma once
#include "Creature.h"
#include <cmath>
#include <algorithm>
#include <iostream>

class World {
private:
	int width;
	int height;
	Creature*** map;
	float** energyDistribution;
	float maxEnergy;
	float mutationChance;
	float mutationCoefficient;
	float maxWeight;
	int getX(int x) {
		if (x < 0) {
			return width + x;
		}
		if (x >= width) {
			return x - width;
		}
		return x;
	}
	int getY(int y) {
		if (y < 0) {
			return height + y;
		}
		if (y >= height) {
			return y - height;
		}
		return y;
	}
	void changeEnergyDistribution(int timer) {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				energyDistribution[i][j] = 1.65 + 1.0 * std::cos((timer * 0.1 + i) * 2 * std::acos(-1) / width);
				if ((timer % 1000 > 500)&&(timer > 1000)) {
					energyDistribution[i][j] -= 1.3 * std::pow(std::sin((j) * std::acos(-1) / 200), 5);
				}
				//std::cout << "a" << std::endl;
				/*if (timer >= 100) {
					energyDistribution[i][j] = (float)-1;
					std::cout << " ada" << std::endl;
				}*/
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
		else {
			*birth = false;
		}
	}
	void takeOutput(float* output, int x, int y, int timer) {
		float* inputs = new float[19];
		gatherInputs(inputs, x, y, timer);
		map[x][y]->calculateOutput(inputs, 19, output);
		delete[] inputs;
	}
	void gatherInputs(float* inputs, int x, int y, int timer) {
		int counter = 0;
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				inputs[counter] = energyDistribution[getX(x + i)][getY(y + j)] / maxEnergy;
				counter++;
			}
		}
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				if ((j != 0) || (i != 0)) {
					if (map[getX(x + i)][getY(y + j)] != NULL) {
						inputs[counter] = map[getX(x + i)][getY(y + j)]->getSeed() - map[x][y]->getSeed();
					}
					else {
						inputs[counter] = 0;
					}
					counter++;
				}
			}
		}
		inputs[17] = std::sin(timer * (map[x][y]->getOscillatorPeriod()) + (map[x][y]->getOscillatorOffset()));
		inputs[18] = map[x][y]->getEnergy();
	}
public:
	World(int widthInput, int heightInput, float maxEnergyInput, float mutationChanceInput, float mutationCoefficientInput,
		float maxWeightInput) {
		width = widthInput;
		height = heightInput;
		map = new Creature * *[width];
		energyDistribution = new float* [width];
		for (int i = 0; i < width; i++) {
			map[i] = new Creature * [height];
			energyDistribution[i] = new float[height];
			for (int j = 0; j < width; j++) {
				map[i][j] = NULL;
				energyDistribution[i][j] = 0;
			}
		}
		maxEnergy = maxEnergyInput;
		mutationChance = mutationChanceInput;
		mutationCoefficient = mutationCoefficientInput;
		maxWeight = maxWeightInput;
	}
	void placeCreature(Creature* c, int x, int y) {
		map[x][y] = c;
	}
	Creature*** getMap() {
		return map;
	}
	float getEnergy(int x, int y) {
		return energyDistribution[x][y];
	}
	
	
	void step(int timer) {
		changeEnergyDistribution(timer);
		int* dx = new int;
		int* dy = new int;
		bool* birth = new bool;
		*dx = 0;
		*dy = 0;
		*birth = false;
		
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if ((map[i][j] != NULL)&&(map[i][j]!=nullptr)) {
					if (map[i][j]->getActionTimer()) {
						continue;
					}
					map[i][j]->setEnergy(map[i][j]->getEnergy() - 1);
					map[i][j]->setEnergy(energyDistribution[i][j] + map[i][j]->getEnergy());
					if (map[i][j]->getEnergy() <= 0) {
						map[i][j]->deleteCreature();
						delete map[i][j];
						map[i][j] = NULL;
						continue;
					}
					float* output = new float[3];
					takeOrder(dx, dy, birth, output, i, j, timer);
					
					if ((*dx != 0) || (*dy != 0)) {
						if ((birth)&& (map[getX(i + *dx)][getY(j + *dy)] == NULL)) {
							map[getX(i + *dx)][getY(j + *dy)] = new Creature(maxEnergy, map[i][j]->getFirstLayerSize(),
								map[i][j]->getInternalLayerAmount(), map[i][j]->getInternalLayerSize(), map[i][j]->getFinalLayerSize(),
								map[i][j]->getFirstLayer()[0]->amountOfInputs, maxWeight);
							map[getX(i + *dx)][getY(j + *dy)]->becomeChild(map[i][j], mutationChance, mutationCoefficient);
							map[i][j]->setEnergy(map[i][j]->getEnergy() / 2 * 0.9 - 1);
							map[getX(i + *dx)][getY(j + *dy)]->setEnergy(map[i][j]->getEnergy());
							map[getX(i + *dx)][getY(j + *dy)]->setActionTimer(true);
							if (map[i][j]->getEnergy() <= 0) {
								map[i][j]->deleteCreature();
								delete map[i][j];
								map[i][j] = NULL;
								continue;
							}
						}
						else {
							if (map[getX(i + *dx)][getY(j + *dy)] == NULL) {
								map[getX(i + *dx)][getY(j + *dy)] = map[i][j];
								map[i][j] = NULL;
								map[getX(i + *dx)][getY(j + *dy)]->setActionTimer(true);
							}
							else {
								map[i][j]->setEnergy(map[getX(i + *dx)][getY(j + *dy)]->getEnergy() / 2 * 0.8 + map[i][j]->getEnergy());
								map[getX(i + *dx)][getY(j + *dy)]->setEnergy(map[getX(i + *dx)][getY(j + *dy)]->getEnergy() / 2 - 0.5);
								if (map[getX(i + *dx)][getY(j + *dy)]->getEnergy() <= 0) {
									map[getX(i + *dx)][getY(j + *dy)]->deleteCreature();
									delete map[getX(i + *dx)][getY(j + *dy)];
									map[getX(i + *dx)][getY(j + *dy)] = NULL;
								}
							}
							if (map[i][j]->getEnergy() <= 0) {
								map[i][j]->deleteCreature();
								delete map[i][j];
								map[i][j] = NULL;
								continue;
							}
						}
					}
					delete[] output;
					map[i][j]->setEnergy(std::min(maxEnergy, map[i][j]->getEnergy()));
				}
			}
		}
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if ((map[i][j] != NULL)&&(map[i][j]!=nullptr)) {
					map[i][j]->setActionTimer(false);
				}
			}
		}
		

	}
	
	
};