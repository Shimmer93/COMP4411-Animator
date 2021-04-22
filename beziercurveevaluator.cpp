#include "beziercurveevaluator.h"
#include "mat.h"
#include "vec.h"
#include <iostream>
using namespace std;
void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
										 std::vector<Point>& ptvEvaluatedCurvePts,
										 const float& fAniLength,
										 const bool& bWrap) const
{
	ptvEvaluatedCurvePts.clear();
	
	Mat4f m(-1, 3, -3, 1, 
		     3, -6, 3, 0, 
		     -3, 3, 0, 0, 
		     1, 0, 0, 0);

	std::vector<Point> ctrlPts;
	ctrlPts.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());
	if (bWrap) // if wrapped, add a point outside the screen to simulate the first point in the next loop
		ctrlPts.push_back(Point(ptvCtrlPts.front().x + fAniLength, ptvCtrlPts.front().y));

	int numPt = ctrlPts.size();
	if (numPt > 3) {
		for (int i = 0; i < (numPt - 1) / 3; i++) {
			ptvEvaluatedCurvePts.push_back(ctrlPts[i * 3]);
			Vec4f px(ctrlPts[i * 3].x, ctrlPts[i * 3 + 1].x, ctrlPts[i * 3 + 2].x, ctrlPts[i * 3 + 3].x);
			Vec4f py(ctrlPts[i * 3].y, ctrlPts[i * 3 + 1].y, ctrlPts[i * 3 + 2].y, ctrlPts[i * 3 + 3].y);
			Vec4f mx = m * px;
			Vec4f my = m * py;
			for (float t = 0.01; t < 1; t += 0.01) {
				Vec4f v(t * t * t, t * t, t, 1);
				float x = v * mx;
				float y = v * my;
				if (x <= ctrlPts[i * 3 + 3].x && x >= ctrlPts[i * 3].x) { // restrict the curve inside the interval
					if (bWrap && x > fAniLength) x -= fAniLength; // wrap!
					ptvEvaluatedCurvePts.push_back(Point(x, y));
				}
			}
		}
	}
	else // if only 3 or less points, cannot enter the for loop, insert the first point
		ptvEvaluatedCurvePts.push_back(ctrlPts.front());

	if (numPt % 3 != 1) { // if remaining points cannot form a curve, connect them with lines
		for (int j = numPt - (numPt - 1) % 3; j < numPt; j++)
			ptvEvaluatedCurvePts.push_back(ctrlPts[j]);
		if (bWrap) // wrap!
			ptvEvaluatedCurvePts.push_back(Point(ptvCtrlPts.back().x - fAniLength, ptvCtrlPts.back().y));
	}
	else // otherwise just insert the last point
		ptvEvaluatedCurvePts.push_back(ctrlPts.back());

	if (!bWrap) {
		ptvEvaluatedCurvePts.push_back(Point(0, ctrlPts.front().y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ctrlPts.back().y));
	}
}