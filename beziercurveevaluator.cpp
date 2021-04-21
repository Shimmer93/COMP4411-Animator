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
	
	Mat4f mb(-1, 3, -3, 1, 
		     3, -6, 3, 0, 
		     -3, 3, 0, 0, 
		     1, 0, 0, 0);

	std::vector<Point> CtrlPts;
	CtrlPts.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());
	if (bWrap) // if wrapped, add a point outside the screen to simulate the first point in the next loop
		CtrlPts.push_back(Point(ptvCtrlPts.front().x + fAniLength, ptvCtrlPts.front().y));

	int numPt = CtrlPts.size();
	if (numPt > 3) {
		for (int i = 0; i < (numPt - 1) / 3; i++) {
			ptvEvaluatedCurvePts.push_back(CtrlPts[i * 3]);
			Vec4f px(CtrlPts[i * 3].x, CtrlPts[i * 3 + 1].x, CtrlPts[i * 3 + 2].x, CtrlPts[i * 3 + 3].x);
			Vec4f py(CtrlPts[i * 3].y, CtrlPts[i * 3 + 1].y, CtrlPts[i * 3 + 2].y, CtrlPts[i * 3 + 3].y);
			Vec4f mbx = mb * px;
			Vec4f mby = mb * py;
			for (float t = 0.01; t < 1; t += 0.01) {
				Vec4f v(t * t * t, t * t, t, 1);
				float x = v * mbx;
				float y = v * mby;
				if (x <= CtrlPts[i * 3 + 3].x && x >= CtrlPts[i * 3].x) { // restrict the curve inside the interval
					if (bWrap && x > fAniLength) x -= fAniLength; // wrap!
					ptvEvaluatedCurvePts.push_back(Point(x, y));
				}
			}
		}
	}
	else // if only 3 or less points, cannot enter the for loop, insert the first point
		ptvEvaluatedCurvePts.push_back(CtrlPts.front());

	if (numPt % 3 != 1) { // if remaining points cannot form a curve, connect them with lines
		for (int j = numPt - (numPt - 1) % 3; j < numPt; j++)
			ptvEvaluatedCurvePts.push_back(CtrlPts[j]);
		if (bWrap) // wrap!
			ptvEvaluatedCurvePts.push_back(Point(ptvCtrlPts.back().x - fAniLength, ptvCtrlPts.back().y));
	}
	else
		ptvEvaluatedCurvePts.push_back(CtrlPts.back());

	if (!bWrap) {
		ptvEvaluatedCurvePts.push_back(Point(0, CtrlPts.front().y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, CtrlPts.back().y));
	}
}