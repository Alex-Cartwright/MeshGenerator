// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Cartesian.h>
#include <iterator>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "rapidcsv.h"
#include "pnpoly.h"

using namespace std;


int main()
{
	//Import Surface Mesh

	rapidcsv::Document doc("polylines.csv");
	std::vector<float> surfaceMesh = doc.GetColumn<float>("x");
	//std::cout << "Read " << surfaceMesh.size() << " values." << std::endl;

	std::vector<vector<float>> surfaceCoords;
	for (size_t i = 0; i < surfaceMesh.size(); i++)
	{
		vector<float> row;

		row.push_back(doc.GetCell<float>("x", std::to_string(i + 1)));
		row.push_back(doc.GetCell<float>("y", std::to_string(i + 1)));

		surfaceCoords.push_back(row);
		//std::cout << surfaceCoords[i][0] << "," << surfaceCoords[i][1] << std::endl;
	}


	printf("Creating a Cartesian Array \n");
	int xn = 24, yn = 48, nCartesian = xn*yn;
	float xdist = 80, ydist = 160;
	Cartesian cartesianGrid = Cartesian(xn, yn, xdist, ydist, surfaceMesh.size(), surfaceCoords);
	
	printf("Array Contents: \n");
	ofstream myFile;
	myFile.open("initialCartesian.txt");
	for (int i = 0; i < nCartesian; i++)
	{
		myFile << cartesianGrid.cartesianArray[i][0] << "," << cartesianGrid.cartesianArray[i][1] << "\n";
	}
	myFile.close();

	myFile.open("listOfCartesianLines.txt");
	for (int i = 0; i < cartesianGrid.listOfLines.size(); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			myFile << cartesianGrid.listOfLines[i][j] << ",";
		}
		myFile << "\n";
	}
	myFile.close();

	vector<vector<float>> testInsideNodes = cartesianGrid.FindPointsNearSurface(1);
	myFile.open("nearSurfaceNodes.txt");
	for (int i = 0; i < testInsideNodes.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			myFile << testInsideNodes[i][j] << ",";
		}
		myFile << "\n";
	}
	myFile.close();

	myFile.open("surfaceIntersectNodes.txt");
	for (int i = 0; i < cartesianGrid.surfaceIntersectNodes.size(); i++)
	{
		for (int j = 0; j < 2; j++)
		{
			myFile << cartesianGrid.surfaceIntersectNodes[i][j] << ",";
		}
		myFile << "\n";
	}
	myFile.close();

	vector<vector<float>> springsOut = cartesianGrid.CalculateSpringForces(cartesianGrid.listOfLines);
	myFile.open("springs.txt");
	for (int i = 0; i < springsOut.size(); i++)
	{
		for (int j = 0; j < 5; j++)
		{
			myFile << springsOut[i][j] << ",";
		}
		myFile << "\n";
	}
	myFile.close();
	//bool insideShape;
	//vector<vector<float>> insideShapeCartesianGridCoords;
	//int insideShapeN = 0;
	//remove points not within polygon
	/*myFile.open("truncatedCartesian.txt");
	for (int i = 0; i < nCartesian; i++)
	{
		insideShape = pnpoly(surfaceMesh.size(), surfaceCoords, cartesianGrid.cartesianArray[i][0], cartesianGrid.cartesianArray[i][1]);
		if (insideShape)
		{
			vector<float> row;
			row.push_back(cartesianGrid.cartesianArray[i][0]);
			row.push_back(cartesianGrid.cartesianArray[i][1]);
			insideShapeCartesianGridCoords.push_back(row);
			myFile << insideShapeCartesianGridCoords[insideShapeN][0] << "," << insideShapeCartesianGridCoords[insideShapeN][1] << std::endl;
			insideShapeN++;
		}
		else
		{
			//for 
		}
	}
	myFile.close();*/

	
}


