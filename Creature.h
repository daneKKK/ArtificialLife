#pragma once
#include "Neuron.h"
#include <random>

class Creature {
private:
	float energy;
	unsigned short firstLayerSize;
	Neuron** firstLayer;
	unsigned short internalLayerAmount;
	unsigned short internalLayerSize;
	unsigned short finalLayerSize;
	Neuron*** internalLayers;
	Neuron** finalLayer;
	float oscillatorPeriod = 100;
	float oscillatorOffset = 10;
	float seed;
	float* outputs;
	float countSeed() {
		float* inputs = new float[firstLayerSize];
		for (int i = 0; i < finalLayerSize; i++) {
			inputs[i] = 0.5;
		}
		calculateOutput(inputs, firstLayerSize);
		float seedCalculated = 0;
		for (int i = 0; i < finalLayerSize; i++) {
			seedCalculated += outputs[i];
		}
		delete[] inputs;
		return (seedCalculated / 2) + 1 / 2;
	}
public:
	void setEnergy(float energyInput) {
		energy = energyInput;
	}
	float getEnergy() {
		return energy;
	}
	float getSeed() {
		return seed;
	}
	Neuron** getFirstLayer() {
		return firstLayer;
	}
	Neuron*** getInternalLayer() {
		return internalLayers;
	}
	Neuron** getFinalLayer() {
		return finalLayer;
	}
	unsigned short getFirstLayerSize() {
		return firstLayerSize;
	}
	unsigned short getInternalLayerAmount() {
		return internalLayerAmount;
	}
	unsigned short getInternalLayerSize() {
		return internalLayerSize;
	}
	unsigned short getFinalLayerSize() {
		return finalLayerSize;
	}
	unsigned short getInputSize() {
		return firstLayer[0]->amountOfInputs;
	}
	float getOscillatorPeriod() {
		return oscillatorPeriod;
	}
	float getOscillatorOffset() {
		return oscillatorOffset;
	}
	float* getOutputs() {
		return outputs;
	}
	void setLayerSizes(unsigned short firstLayerSizeInput, unsigned short internalLayerAmountInput, unsigned short internalLayerSizeInput,
		unsigned short finalLayerSizeInput) {
		firstLayerSize = firstLayerSizeInput;
		internalLayerAmount = internalLayerAmountInput;
		internalLayerSize = internalLayerSizeInput;
		finalLayerSize = finalLayerSizeInput;
	}
	void setNeuronArrays(unsigned short totalInputsAmount, float maxWeight) {
		firstLayer = new Neuron*[firstLayerSize];
		internalLayers = new Neuron** [internalLayerAmount];
		for (int i = 0; i < internalLayerAmount; i++) {
			internalLayers[i] = new Neuron * [internalLayerSize];
		}
		for (int i = 0; i < firstLayerSize; i++) {
			init(firstLayer[i], totalInputsAmount);
			setRandomWeights(firstLayer[i], maxWeight);
		}
		for (int i = 0; i < internalLayerSize; i++) {
			init(internalLayers[0][i], firstLayerSize);
			setRandomWeights(internalLayers[0][i], maxWeight);
		}
		for (int i = 1; i < internalLayerAmount; i++) {
			for (int j = 0; i < internalLayerSize; j++) {
				init(internalLayers[i][j], internalLayerSize);
				setRandomWeights(internalLayers[i][j], maxWeight);
			}
		}
		for (int i = 0; i < finalLayerSize; i++) {
			init(finalLayer[i], internalLayerSize);
			setRandomWeights(finalLayer[i], maxWeight);
		}
	}
	Creature(float maxEnergy, unsigned short firstLayerSizeInput, unsigned short internalLayerAmountInput, unsigned short internalLayerSizeInput,
		unsigned short finalLayerSizeInput, unsigned short totalInputsAmount, float maxWeight) {
		setEnergy(maxEnergy);
		setLayerSizes(firstLayerSizeInput, internalLayerAmountInput, internalLayerSizeInput, finalLayerSizeInput);
		setNeuronArrays(totalInputsAmount, maxWeight);
		seed = countSeed();
	}
	void mutateBrain(float mutationChance, float mutationCoefficient) {
		if (mutate(mutationChance)) {
			oscillatorOffset *= (1 + getMutation(mutationCoefficient));
		}
		if (mutate(mutationChance)) {
			oscillatorPeriod *= (1 + getMutation(mutationCoefficient));
		}
		for (int i = 0; i < firstLayerSize; i++) {
			mutateNeuron(firstLayer[i], mutationChance, mutationCoefficient);
		}
		for (int i = 0; i < internalLayerAmount; i++) {
			for (int j = 0; j < internalLayerSize; j++) {
				mutateNeuron(internalLayers[i][j], mutationChance, mutationCoefficient);
			}
		}
		seed = countSeed();
	}
	void becomeChild(Creature* p, float mutationChance, float mutationCoefficient) {
		for (int i = 0; i < firstLayerSize; i++) {
			for (int j = 0; j < p->getInputSize(); j++) {
				firstLayer[i]->weights[j] = p->getFirstLayer()[i]->weights[j];
			}
			firstLayer[i]->offset = p->getFirstLayer()[i]->offset;
		}
		for (int i = 0; i < internalLayerSize; i++) {
			for (int j = 0; j < p->getFirstLayerSize(); j++) {
				internalLayers[0][i]->weights[j] = p->getInternalLayer()[0][i]->weights[j];
			}
			internalLayers[0][i]->offset = p->getInternalLayer()[0][i]->offset;
		}
		for (int i = 1; i < internalLayerAmount; i++) {
			for (int j = 0; i < internalLayerSize; j++) {
				for (int k = 0; j < internalLayerSize; k++) {
					internalLayers[i][j]->weights[k] = p->getInternalLayer()[i][j]->weights[k];
				}
				internalLayers[i][j]->offset = p->getInternalLayer()[i][j]->offset;
			}
		}
		for (int i = 0; i < finalLayerSize; i++) {
			for (int j = 0; j < p->getInternalLayerSize(); j++) {
				finalLayer[i]->weights[j] = p->getFinalLayer()[i]->weights[j];
			}
			finalLayer[i]->offset = p->getFinalLayer()[i]->offset;
		}
		mutateBrain(mutationChance, mutationCoefficient);
	}
	void calculateOutput(float* inputs, unsigned short inputSize) {
		float* firstOutputs = new float[firstLayerSize];
		float* innerOutputs = new float[internalLayerSize];
		float* innerOutputsReplacer = new float[internalLayerSize];
		float* finalOutputs = new float[finalLayerSize];
		for (int i = 0; i < firstLayerSize; i++) {
			firstOutputs[i] = calculate(firstLayer[i], inputSize, inputs);
		}
		for (int i = 0; i < internalLayerSize; i++) {
			innerOutputs[i] = calculate(internalLayers[0][i], (unsigned short)firstLayerSize, firstOutputs);
			innerOutputsReplacer[i] = innerOutputs[i];
		}
		for (int i = 1; i < internalLayerAmount; i++) {
			for (int j = 0; j < internalLayerSize; j++) {
				innerOutputs[j] = calculate(internalLayers[i][j], (unsigned short)internalLayerSize, innerOutputsReplacer);
			}
			for (int j = 0; j < internalLayerSize; j++) {
				innerOutputsReplacer[j] = innerOutputs[j];
			}
		}
		for (int i = 0; i < finalLayerSize; i++) {
			finalOutputs[i] = calculate(finalLayer[i], (unsigned short)internalLayerSize, innerOutputs);
		}
		delete[] firstOutputs;
		delete[] innerOutputs;
		delete[] innerOutputsReplacer;
		outputs = finalOutputs;
	}
	void deleteCreature() {
		for (int i = 0; i < firstLayerSize; i++) {
			delete[] firstLayer[i]->weights;
			delete firstLayer[i];
		}
		delete[] firstLayer;
		for (int i = 0; i < internalLayerAmount; i++) {
			for (int j = 0; i < internalLayerSize; j++) {
				delete[] internalLayers[i][j]->weights;
				delete internalLayers[i][j];
			}
			delete[] internalLayers[i];
		}
		delete[] internalLayers;
		for (int i = 0; i < finalLayerSize; i++) {
			delete[] finalLayer[i]->weights;
			delete finalLayer[i];
		}
		delete[] finalLayer;
	}
};