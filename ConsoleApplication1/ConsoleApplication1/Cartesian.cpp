#include "Cartesian.h"
#include "pnpoly.h"




Cartesian::Cartesian(int xinput, int yinput, float xdistinput, float ydistinput, int nvertinput, vector<vector<float>> vertxyinput)
{
	xn = xinput;
	yn = yinput;
	xdist = xdistinput;
	ydist = ydistinput;
	cartesianArray = GenerateCartesianGrid();
	nvert = nvertinput;
	vertxy = vertxyinput;
	nodesInsideSurface = FindPointsInsideSurface();
	nodesInsideSurfaceN = nodesInsideSurface.size();
	listOfLines = GenerateListOfLines();
}

Cartesian::~Cartesian()
{
}

float** Cartesian::GenerateCartesianGrid()
{
	
	//Generate a cartesian mesh of size xdist by ydist with xn by yn points
	//xn, yn > 2

	//Create empty (xn*yn) x 2 array full of zeroes
	cartesianArray = 0;
	cartesianArray = new float* [xn * yn];

	//n = number of dimensions
	for (int n = 0; n < xn * yn; n++)
	{
		cartesianArray[n] = new float[2];
		//xn * yn number of points, n dimensions so each has x,y coords
		//Fill with zeroes
		cartesianArray[n][0] = 0;
		cartesianArray[n][1] = 0;
	}

	//Calculate coordinate spacing

	xspacing = xdist / (xn - 1);
	yspacing = ydist / (yn - 1);

	//Populate array with correct cartesian values
	for (int i = 0; i < xn; i++)
	{
		//Loop through x values
		for (int j = 0; j < yn; j++)
		{
			//Loop through y values
			cartesianArray[j + i * yn][0] = i * xspacing;
			cartesianArray[j + i * yn][1] = j * yspacing;
		}
	}


	return cartesianArray;
}

vector<vector<float>> Cartesian::FindPointsNearSurface(float withinDistance)
{
	for (int i = 0; i < nvert; i++) {
		for (int j = 0; j < nodesInsideSurfaceN; j++)
		{
			if (distance(vertxy[i][0], vertxy[i][1], nodesInsideSurface[j][1], nodesInsideSurface[j][2]) < withinDistance)
			{
				vector<float> row;
				row.push_back(nodesInsideSurface[j][0]);
				row.push_back(nodesInsideSurface[j][1]);
				row.push_back(nodesInsideSurface[j][2]);
				nodesNearSurface.push_back(row);
				nodesNearSurfaceN++;
			}
		}
	}
	return nodesNearSurface;
}


vector<vector<float>> Cartesian::FindPointsInsideSurface()
{
	//Find nodes inside the surface and return a vector of all the points and their coordinates
	for (int i = 0; i < xn * yn; i++)
	{
		bool insideShape;
		insideShape = pnpoly(vertxy.size(), vertxy, cartesianArray[i][0], cartesianArray[i][1]);
		if (insideShape)
		{
			vector<float> row;
			row.push_back((float)i);
			row.push_back(cartesianArray[i][0]);
			row.push_back(cartesianArray[i][1]);
			nodesInsideSurface.push_back(row);
		}
	}
	return nodesInsideSurface;
}

vector<float> Cartesian::LineDeletion()
{
	//Check nodes near surface
	for (int i = 0; i < nodesNearSurfaceN; i++)
	{
		//if all four members short, leave as is?
		//if one member short, delete both perpendicular members
		//if two members short, move node onto surface edge
		// check whether surface concave/convex?
	}
	return vector<float>();
}



vector<vector<float>> Cartesian::CalculateSpringForces(vector<vector<float>> listOfSprings)
{
	//F = kx
//listOfSprings should be node1ID, node2ID, x1, y1, x2, y2

	vector<vector<float>> displacements;
	for (int i = 0; i < listOfSprings.size(); i++)
	{
		//Calculate displacements
		vector<float> displacementRow;
		displacementRow.push_back(listOfSprings[i][0]);
		displacementRow.push_back(listOfSprings[i][1]);
		displacementRow.push_back(abs(listOfSprings[i][4] - listOfSprings[i][2]) - listOfSprings[i][6]);
		displacementRow.push_back(abs(listOfSprings[i][5] - listOfSprings[i][3]) - listOfSprings[i][7]);
		displacements.push_back(displacementRow);
	}
	
	//delete duplicates
	int deleteCount = 0;
	for (int i = 0; i < displacements.size() - deleteCount; i++)
	{
		for (int j = 0; j < displacements.size() - deleteCount; j++)
		{
			if (displacements[i][0] == displacements[j][1] && displacements[i][1] == displacements[j][0])
			{
				//duplicate found
				displacements.erase(displacements.begin() + j);
				deleteCount++;
				break;
			}
		}

	}



	//input stiffnesses and calculate force
	vector<vector<float>> totalNodeDisplacements;
	
	for (int i = 0; i < nodesInsideSurface.size(); i++)
	{
		vector<float> totalDispRow;
		float xdisp = 0, ydisp = 0;
		for (int j = 0; j < displacements.size(); j++)
		{
			if (nodesInsideSurface[i][0] == displacements[j][0])
			{
				if (nodesInsideSurface[i][1] > cartesianArray[(int)displacements[j][1]][1])
				{
					xdisp += 0.3 * (displacements[j][2]);
				}
				else
				{
					xdisp -= 0.3 * (displacements[j][2]);
				}

				if (nodesInsideSurface[i][2] > cartesianArray[(int)displacements[j][1]][2])
				{
					ydisp += 0.3 * (displacements[j][3]);
				}
				else
				{
					ydisp -= 0.3 * (displacements[j][3]);
				}

				
			}

			if (nodesInsideSurface[i][0] == displacements[j][1])
			{
				if (nodesInsideSurface[i][1] > cartesianArray[(int)displacements[j][0]][1])
				{
					xdisp -= 0.1 * (displacements[j][2]);
				}
				else
				{
					xdisp += 0.1 * (displacements[j][2]);
				}

				if (nodesInsideSurface[i][2] > cartesianArray[(int)displacements[j][0]][2])
				{
					ydisp -= 0.1 * (displacements[j][3]);
				}
				else
				{
					ydisp += 0.1 * (displacements[j][3]);
				}


			}
		}
		totalDispRow.push_back(nodesInsideSurface[i][0]);
		totalDispRow.push_back(nodesInsideSurface[i][1]);
		totalDispRow.push_back(nodesInsideSurface[i][2]);
		totalDispRow.push_back(xdisp);
		totalDispRow.push_back(ydisp);
		totalNodeDisplacements.push_back(totalDispRow);
	}
	//loop over each node, calculate force from each spring, then displace it

	return totalNodeDisplacements;
}



float Cartesian::distance(float x1, float y1, float x2, float y2)
{
	return sqrt(pow(x2 - x1, 2) +
		pow(y2 - y1, 2) * 1.0);
}

vector<vector<float>> Cartesian::GenerateListOfLines()
{
	bool insideShape;

	//Create list of lines
	for (int i = 0; i < xn; i++)
	{
		//Loop through x values
		for (int j = 0; j < yn; j++)
		{
			vector<float> lineRow, testLine, temp;
			//Loop through y values

			//Only generate lines if inside polygon
			insideShape = pnpoly(nvert, vertxy, cartesianArray[j + i * yn][0], cartesianArray[j + i * yn][1]);
			if (insideShape)
			{
				if (i == 0)
				{
					//LEFT COLUMN
					if (j == 0)
					{
						//Bottom Left Corner

						//Above Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back((j + 1) + i * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[(j + 1) + i * yn][0], cartesianArray[(j + 1) + i * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[(j + 1) + i * yn][0]); // x coord of y + 1 point
							lineRow.push_back(cartesianArray[(j + 1) + i * yn][1]); // y coord of y + 1 point
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[(j + 1) + i * yn][0]); // x coord of y + 1 point
							testLine.push_back(cartesianArray[(j + 1) + i * yn][1]); // y coord of y + 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}

						//Right Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back(j + (i + 1) * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[j + (i + 1) * yn][0], cartesianArray[j + (i + 1) * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[j + (i + 1) * yn][0]); // x coord of x + 1 point
							lineRow.push_back(cartesianArray[j + (i + 1) * yn][1]); // y coord of x + 1 point
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[j + (i + 1) * yn][0]); // x coord of x + 1 point
							testLine.push_back(cartesianArray[j + (i + 1) * yn][1]); // y coord of x + 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}
					}
					else if (j == (yn - 1))
					{
						//Top Left Corner

						//Below Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back((j - 1) + i * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[(j - 1) + i * yn][0], cartesianArray[(j - 1) + i * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[(j - 1) + i * yn][0]); // x coord of y - 1 point
							lineRow.push_back(cartesianArray[(j - 1) + i * yn][1]); // y coord of y - 1 point
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[(j - 1) + i * yn][0]); // x coord of y - 1 point
							testLine.push_back(cartesianArray[(j - 1) + i * yn][1]); // y coord of y - 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}

						//Right Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back(j + (i + 1) * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[j + (i + 1) * yn][0], cartesianArray[j + (i + 1) * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[j + (i + 1) * yn][0]); // x coord of x + 1 point
							lineRow.push_back(cartesianArray[j + (i + 1) * yn][1]); // y coord of x + 1 point
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[j + (i + 1) * yn][0]); // x coord of x + 1 point
							testLine.push_back(cartesianArray[j + (i + 1) * yn][1]); // y coord of x + 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}
					}
					else
					{
						//All other nodes in left column not at corners

						//Below Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back((j - 1) + i * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[(j - 1) + i * yn][0], cartesianArray[(j - 1) + i * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[(j - 1) + i * yn][0]); // x coord of y - 1 point
							lineRow.push_back(cartesianArray[(j - 1) + i * yn][1]); // y coord of y - 1 point
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[(j - 1) + i * yn][0]); // x coord of y - 1 point
							testLine.push_back(cartesianArray[(j - 1) + i * yn][1]); // y coord of y - 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}

						//Above Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back((j + 1) + i * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[(j + 1) + i * yn][0], cartesianArray[(j + 1) + i * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[(j + 1) + i * yn][0]); // x coord of y + 1 point
							lineRow.push_back(cartesianArray[(j + 1) + i * yn][1]); // y coord of y + 1 point
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[(j + 1) + i * yn][0]); // x coord of y + 1 point
							testLine.push_back(cartesianArray[(j + 1) + i * yn][1]); // y coord of y + 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}

						//Right Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back(j + (i + 1) * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[j + (i + 1) * yn][0], cartesianArray[j + (i + 1) * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[j + (i + 1) * yn][0]); // x coord of x + 1 point
							lineRow.push_back(cartesianArray[j + (i + 1) * yn][1]); // y coord of x + 1 point
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[j + (i + 1) * yn][0]); // x coord of x + 1 point
							testLine.push_back(cartesianArray[j + (i + 1) * yn][1]); // y coord of x + 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}
					}
				}
				else if (i == xn - 1)
				{
					//RIGHT COLUMN
					if (j == 0)
					{
						//Bottom Right Corner

						//Left Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back(j + (i - 1) * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[j + (i - 1) * yn][0], cartesianArray[j + (i - 1) * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[j + (i - 1) * yn][0]); // x coord of x - 1 point
							lineRow.push_back(cartesianArray[j + (i - 1) * yn][1]); // y coord of x - 1 point
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[j + (i - 1) * yn][0]); // x coord of x - 1 point
							testLine.push_back(cartesianArray[j + (i - 1) * yn][1]); // y coord of x - 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}

						//Above Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back((j + 1) + i * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[(j + 1) + i * yn][0], cartesianArray[(j + 1) + i * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[(j + 1) + i * yn][0]); // x coord of y + 1 point
							lineRow.push_back(cartesianArray[(j + 1) + i * yn][1]); // y coord of y + 1 point
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[(j + 1) + i * yn][0]); // x coord of y + 1 point
							testLine.push_back(cartesianArray[(j + 1) + i * yn][1]); // y coord of y + 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}
					}
					else if (j == (yn - 1))
					{
						//Top Right Corner

						//Left Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back(j + (i - 1) * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[j + (i - 1) * yn][0], cartesianArray[j + (i - 1) * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[j + (i - 1) * yn][0]); // x coord of x - 1 point
							lineRow.push_back(cartesianArray[j + (i - 1) * yn][1]); // y coord of x - 1 point
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[j + (i - 1) * yn][0]); // x coord of x - 1 point
							testLine.push_back(cartesianArray[j + (i - 1) * yn][1]); // y coord of x - 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}

						//Below Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back((j - 1) + i * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[(j - 1) + i * yn][0], cartesianArray[(j - 1) + i * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[(j - 1) + i * yn][0]); // x coord of y - 1 point
							lineRow.push_back(cartesianArray[(j - 1) + i * yn][1]); // y coord of y - 1 point
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[(j - 1) + i * yn][0]); // x coord of y - 1 point
							testLine.push_back(cartesianArray[(j - 1) + i * yn][1]); // y coord of y - 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}

					}
					else
					{
						//All other nodes in right column not at corners

						//Left Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back(j + (i - 1) * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[j + (i - 1) * yn][0], cartesianArray[j + (i - 1) * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[j + (i - 1) * yn][0]); // x coord of x - 1 point
							lineRow.push_back(cartesianArray[j + (i - 1) * yn][1]); // y coord of x - 1 point
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[j + (i - 1) * yn][0]); // x coord of x - 1 point
							testLine.push_back(cartesianArray[j + (i - 1) * yn][1]); // y coord of x - 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}

						//Below Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back((j - 1) + i * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[(j - 1) + i * yn][0], cartesianArray[(j - 1) + i * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[(j - 1) + i * yn][0]); // x coord of y - 1 point
							lineRow.push_back(cartesianArray[(j - 1) + i * yn][1]); // y coord of y - 1 point
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[(j - 1) + i * yn][0]); // x coord of y - 1 point
							testLine.push_back(cartesianArray[(j - 1) + i * yn][1]); // y coord of y - 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}

						//Above Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back((j + 1) + i * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[(j + 1) + i * yn][0], cartesianArray[(j + 1) + i * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[(j + 1) + i * yn][0]); // x coord of y + 1 point
							lineRow.push_back(cartesianArray[(j + 1) + i * yn][1]); // y coord of y + 1 point
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[(j + 1) + i * yn][0]); // x coord of y + 1 point
							testLine.push_back(cartesianArray[(j + 1) + i * yn][1]); // y coord of y + 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}

					}
				}
				else
				{
					//ALL OTHER COLUMNS
					if (j == 0)
					{
						//Bottom Line Node

						//Left Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back(j + (i - 1) * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[j + (i - 1) * yn][0], cartesianArray[j + (i - 1) * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[j + (i - 1) * yn][0]); // x coord of x - 1 point
							lineRow.push_back(cartesianArray[j + (i - 1) * yn][1]); // y coord of x - 1 point
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[j + (i - 1) * yn][0]); // x coord of x - 1 point
							testLine.push_back(cartesianArray[j + (i - 1) * yn][1]); // y coord of x - 1 point
							
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}

						//Above Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back((j + 1) + i * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[(j + 1) + i * yn][0], cartesianArray[(j + 1) + i * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[(j + 1) + i * yn][0]); // x coord of y + 1 point
							lineRow.push_back(cartesianArray[(j + 1) + i * yn][1]); // y coord of y + 1 point
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[(j + 1) + i * yn][0]); // x coord of y + 1 point
							testLine.push_back(cartesianArray[(j + 1) + i * yn][1]); // y coord of y + 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}

						//Right Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back(j + (i + 1) * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[j + (i + 1) * yn][0], cartesianArray[j + (i + 1) * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[j + (i + 1) * yn][0]); // x coord of x + 1 point
							lineRow.push_back(cartesianArray[j + (i + 1) * yn][1]); // y coord of x + 1 point
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[j + (i + 1) * yn][0]); // x coord of x + 1 point
							testLine.push_back(cartesianArray[j + (i + 1) * yn][1]); // y coord of x + 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}
					}
					else if (j == (yn - 1))
					{
						//Top Line Node

						//Left Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back(j + (i - 1) * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[j + (i - 1) * yn][0], cartesianArray[j + (i - 1) * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[j + (i - 1) * yn][0]); // x coord of x - 1 point
							lineRow.push_back(cartesianArray[j + (i - 1) * yn][1]); // y coord of x - 1 point
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[j + (i - 1) * yn][0]); // x coord of x - 1 point
							testLine.push_back(cartesianArray[j + (i - 1) * yn][1]); // y coord of x - 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}


						//Below Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back((j - 1) + i * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[(j - 1) + i * yn][0], cartesianArray[(j - 1) + i * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[(j - 1) + i * yn][0]); // x coord of y - 1 point
							lineRow.push_back(cartesianArray[(j - 1) + i * yn][1]); // y coord of y - 1 point
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[(j - 1) + i * yn][0]); // x coord of y - 1 point
							testLine.push_back(cartesianArray[(j - 1) + i * yn][1]); // y coord of y - 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}


						//Right Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back(j + (i + 1) * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[j + (i + 1) * yn][0], cartesianArray[j + (i + 1) * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[j + (i + 1) * yn][0]); // x coord of x + 1 point
							lineRow.push_back(cartesianArray[j + (i + 1) * yn][1]); // y coord of x + 1 point
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[j + (i + 1) * yn][0]); // x coord of x + 1 point
							testLine.push_back(cartesianArray[j + (i + 1) * yn][1]); // y coord of x + 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}

					}
					else
					{
						//All other nodes in left column not at corners

						//Left Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back(j + (i - 1) * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[j + (i - 1) * yn][0], cartesianArray[j + (i - 1) * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[j + (i - 1) * yn][0]); // x coord of x - 1 point
							lineRow.push_back(cartesianArray[j + (i - 1) * yn][1]); // y coord of x - 1 point
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[j + (i - 1) * yn][0]); // x coord of x - 1 point
							testLine.push_back(cartesianArray[j + (i - 1) * yn][1]); // y coord of x - 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}
						

						//Below Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back((j - 1) + i * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[(j - 1) + i * yn][0], cartesianArray[(j - 1) + i * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[(j - 1) + i * yn][0]); // x coord of y - 1 point
							lineRow.push_back(cartesianArray[(j - 1) + i * yn][1]); // y coord of y - 1 point
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[(j - 1) + i * yn][0]); // x coord of y - 1 point
							testLine.push_back(cartesianArray[(j - 1) + i * yn][1]); // y coord of y - 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}


						//Above Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back((j + 1) + i * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[(j + 1) + i * yn][0], cartesianArray[(j + 1) + i * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[(j + 1) + i * yn][0]); // x coord of y + 1 point
							lineRow.push_back(cartesianArray[(j + 1) + i * yn][1]); // y coord of y + 1 point
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[(j + 1) + i * yn][0]); // x coord of y + 1 point
							testLine.push_back(cartesianArray[(j + 1) + i * yn][1]); // y coord of y + 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(0);
							lineRow.push_back(yspacing);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}


						//Right Node
						lineRow.push_back(j + i * yn);
						lineRow.push_back(j + (i + 1) * yn);
						insideShape = pnpoly(nvert, vertxy, cartesianArray[j + (i + 1) * yn][0], cartesianArray[j + (i + 1) * yn][1]);
						if (insideShape)
						{
							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							lineRow.push_back(cartesianArray[j + (i + 1) * yn][0]); // x coord of x + 1 point
							lineRow.push_back(cartesianArray[j + (i + 1) * yn][1]); // y coord of x + 1 point
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
						}
						else
						{
							//Truncate the line
							testLine.push_back(cartesianArray[j + i * yn][0]); //x value
							testLine.push_back(cartesianArray[j + i * yn][1]); //y value
							testLine.push_back(cartesianArray[j + (i + 1) * yn][0]); // x coord of x + 1 point
							testLine.push_back(cartesianArray[j + (i + 1) * yn][1]); // y coord of x + 1 point

							lineRow.push_back(cartesianArray[j + i * yn][0]); //x value
							lineRow.push_back(cartesianArray[j + i * yn][1]); //y value
							temp = LineTruncation(nvert, vertxy, testLine);
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							lineRow.push_back(xspacing);
							lineRow.push_back(0);
							listOfLines.push_back(lineRow);
							lineRow.clear();
							testLine.clear();

							//update intersect Nodes
							lineRow.push_back(temp[0]);
							lineRow.push_back(temp[1]);
							surfaceIntersectNodes.push_back(lineRow);
							lineRow.clear();
						}

					}
				}
			}
				
		}
	}

	return listOfLines;
}

vector<float> Cartesian::LineTruncation(int nvert, vector<vector<float>> vertxy, vector<float> testLine)
{
	//Takes a test line and checks it against every line in the nvert-polygon vertxy and sees which it intersects
	//need to see what happens if it intersects multiple lines - just choose closest one? 
	float* x_intersect, * y_intersect;
	float xint = 10, yint = 10;
	x_intersect = &xint;
	y_intersect = &yint;
	vector<float> lineReturn;
	for (int i = 0; i < nvert; i++)
	{
		if (i == nvert - 1)
		{
			if (get_line_intersection(testLine[0], testLine[1], testLine[2], testLine[3], vertxy[i][0], vertxy[i][1], vertxy[0][0], vertxy[0][1], x_intersect, y_intersect) == 1)
			{
				//lineReturn.push_back(*x_intersect);
				//lineReturn.push_back(*y_intersect);
				break;
			}
		}
		else
		{
			if (get_line_intersection(testLine[0], testLine[1], testLine[2], testLine[3], vertxy[i][0], vertxy[i][1], vertxy[i + 1][0], vertxy[i + 1][1], x_intersect, y_intersect) == 1)
			{
				break;
			}
		}
	}
	
	lineReturn.push_back(xint);
	lineReturn.push_back(yint);
	return lineReturn;
}

char Cartesian::get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float* i_x, float* i_y)
{
	float s1_x, s1_y, s2_x, s2_y;
	s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
	s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

	float s, t;
	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
	t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	{
		// Collision detected
		if (i_x != NULL)
			* i_x = p0_x + (t * s1_x);
		if (i_y != NULL)
			* i_y = p0_y + (t * s1_y);
		return 1;
	}

	return 0; // No collision
}

// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines 
// intersect the intersection point may be stored in the floats i_x and i_y.
