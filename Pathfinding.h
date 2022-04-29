#pragma once
#include <string>
#include <Classes.h>
#include <Image.h>
#include <iostream>

pixel** imgToNodes(std::string mapname, pixel** mapInfo, int32* width, int32* height) {
	byte** pixels;
	int32 bytesPerPixel;

	byte* waterPixels;
	byte* roadPixels;
	byte* urbanPixels;

	std::string waterString = "maps/" + mapname + "/water.bmp";
	std::string roadString = "maps/" + mapname + "/road.bmp";
	std::string urbanString = "maps/" + mapname + "/urban.bmp";

	char* waterPath = new char[waterString.size() + 1];
	char* roadPath = new char[roadString.size() + 1];
	char* urbanPath = new char[urbanString.size() + 1];

	strcpy(waterPath, waterString.c_str());
	strcpy(roadPath, roadString.c_str());
	strcpy(urbanPath, urbanString.c_str());

	/*const char* visualPath = ("maps/" + mapname + "/visuallayer.").c_str();
	const char* waterPath = ("maps/" + mapname + "/water.bmp").c_str();
	const char* roadPath = ("maps/" + mapname + "/road.bmp").c_str();
	const char* urbanPath = ("maps/" + mapname + "/urban.bmp").c_str();*/
	
	std::cout << waterPath << std::endl;
	ReadImage(waterPath, &waterPixels, width, height, &bytesPerPixel);
	ReadImage(roadPath, &roadPixels, width, height, &bytesPerPixel);
	ReadImage(urbanPath, &urbanPixels, width, height, &bytesPerPixel);
	
	delete[] waterPath;
	delete[] roadPath;
	delete[] urbanPath;

	mapInfo = new pixel * [*height];
	for (int i = 0; i < *height; i++) {
		mapInfo[i] = new pixel[*width];
		for (int j = 0; j < *width; j++) {
			mapInfo[i][j].type = 'p';
			if ((int)*(waterPixels + 2 * (i * (*width) + j)) <= 1) {
				mapInfo[i][j].type = 'w';
			}
			if ((int)*(urbanPixels + 2 * (i * (*width) + j)) <= 1) {
				mapInfo[i][j].type = 'u';
			}
			if ((int)*(roadPixels + 2 * (i * (*width) + j)) <= 1) {
				mapInfo[i][j].type = 'r';
			}
		}
	}
	return mapInfo;
}


