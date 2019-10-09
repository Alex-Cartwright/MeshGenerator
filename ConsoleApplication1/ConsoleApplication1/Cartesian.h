#pragma once
#include <vector>
using namespace std;
class Cartesian
{
public:
	Cartesian(int xinput, int yinput, float xdistinput, float ydistinput, int nvert, vector<vector<float>> vertxyinput);
	~Cartesian();

	float** GenerateCartesianGrid();
	vector<vector<float>> FindPointsNearSurface(float withinDistance);

	vector<vector<float>> FindPointsInsideSurface();
	
	float** cartesianArray;

	vector<vector<float>> listOfLines, surfaceIntersectNodes;
	vector<float> LineDeletion();
	vector<vector<float>> CalculateSpringForces(vector<vector<float>> listOfSprings);
private:
	int xn, yn, nvert, nodesInsideSurfaceN, nodesNearSurfaceN;
	float xdist, ydist, xspacing, yspacing;

	vector<vector<float>> vertxy;
	vector<vector<float>> nodesInsideSurface, nodesNearSurface;

	float distance(float x1, float y1, float x2, float y2);

	vector<vector<float>> GenerateListOfLines();
	vector<float> LineTruncation(int nvert, vector<vector<float>> vertxy, vector<float> testLine);
	
	char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y,
		float p2_x, float p2_y, float p3_x, float p3_y, float* i_x, float* i_y);
};

struct Node
{
	int cartesianGridID, nodeID;
	float xInitial, yInitial, xCurrent, yCurrent;
};