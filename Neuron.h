#pragma once
#include <algorithm>
#include <cmath>
#include <random>

struct Neuron {
	unsigned short amountOfInputs;
	float* weights;
	float offset;
};

void init(Neuron* nr, unsigned short inputs) {
	nr->amountOfInputs = inputs;
	nr->weights = new float(nr->amountOfInputs);
	for (int i = 0; i < inputs; i++) {
		nr->weights[i] = 0;
	}
	nr->offset = 0;
}

void setRandomWeights(Neuron* nr, float maxWeight) {
	for (int i = 0; i < nr->amountOfInputs; i++) {
		nr->weights[i] = (rand() - RAND_MAX / 2) * 2 / RAND_MAX * maxWeight;
	}
	nr->offset = (rand() - RAND_MAX / 2) * 2 / RAND_MAX * maxWeight;
}

//output is sigma function from nr->amountOfInputs inputs (x_i) with given weights (w_i) and offsets (b_i);
float calculate(Neuron* nr, unsigned short inputSize, float* inputs) {
	float functionInput = nr->offset;
	for (int i = 0; i < std::min(nr->amountOfInputs, inputSize), i++) {
		functionInput += inputs[i] * nr->weights[i];
	}
	float functionResult = 1 / (1 + std::exp(-1 * functionInput));
	return functionResult;
}

bool mutate(float mutationChance) {
	return (rand() / RAND_MAX < mutationChance);
}
float getMutation(float mutationCoefficient) {
	return (((rand()) - RAND_MAX / 2) / RAND_MAX * 2 * mutationCoefficient);
}

void mutateNeuron(Neuron* nr, float mutationChance, float mutationCoefficient) {
	for (int i = 0; i < nr->amountOfInputs; i++) {
		if (mutate(mutationChance)) {
			nr->weights[i] += getMutation(mutationCoefficient);
		}
	}
}