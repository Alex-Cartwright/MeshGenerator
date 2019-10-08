#pragma once
#include <vector>
using namespace std;

static int pnpoly(int nvert, vector<vector<float>> vertxy, float testx, float testy)
{
	int i, j, c = 0;
	for (i = 0, j = nvert - 1; i < nvert; j = i++) {
		if (((vertxy[i][1] > testy) != (vertxy[j][1] > testy)) &&
			(testx < (vertxy[j][0] - vertxy[i][0]) * (testy - vertxy[i][1]) / (vertxy[j][1] - vertxy[i][1]) + vertxy[i][0]))
			c = !c;
	}
	return c;
}
