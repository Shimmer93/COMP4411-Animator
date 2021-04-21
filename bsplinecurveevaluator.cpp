#include "bsplinecurveevaluator.h"
#include "mat.h"
#include "vec.h"
#include <iostream>
using namespace std;

void BSplineCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	ptvEvaluatedCurvePts.clear();

	Mat4f mb(-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 0, 3, 0,
		1, 4, 1, 0);
	mb /= 6.0;

	std::vector<Point> CtrlPts;
	CtrlPts.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());

	if (bWrap) {
		CtrlPts.push_back(Point(ptvCtrlPts[0].x + fAniLength, ptvCtrlPts[0].y));
		CtrlPts.push_back(Point(ptvCtrlPts[1].x + fAniLength, ptvCtrlPts[1].y));
		if (ptvCtrlPts.size() >= 3)
			CtrlPts.push_back(Point(ptvCtrlPts[2].x + fAniLength, ptvCtrlPts[2].y));
		else
			CtrlPts.insert(CtrlPts.begin(), Point(ptvCtrlPts[1].x - fAniLength, ptvCtrlPts[1].y));
	}
	else {
		CtrlPts.insert(CtrlPts.begin(), ptvCtrlPts.front());
		CtrlPts.push_back(ptvCtrlPts.back());
		ptvEvaluatedCurvePts.push_back(ptvCtrlPts.front());
		ptvEvaluatedCurvePts.push_back(ptvCtrlPts.back());
	}

	int numPt = CtrlPts.size();
	if (numPt > 3) {
		for (int i = 0; i < numPt - 3; i++) {
			Vec4f px(CtrlPts[i].x, CtrlPts[i + 1].x, CtrlPts[i + 2].x, CtrlPts[i + 3].x);
			Vec4f py(CtrlPts[i].y, CtrlPts[i + 1].y, CtrlPts[i + 2].y, CtrlPts[i + 3].y);
			Vec4f mbx = mb * px;
			Vec4f mby = mb * py;
			for (float t = 0.01; t < 1; t += 0.01) {
				Vec4f v(t * t * t, t * t, t, 1);
				float x = v * mbx;
				float y = v * mby;
				if (bWrap && x > fAniLength) x -= fAniLength;
				ptvEvaluatedCurvePts.push_back(Point(x, y));
			}
		}
	}

	if (!bWrap) {
		ptvEvaluatedCurvePts.push_back(Point(0, CtrlPts.front().y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, CtrlPts.back().y));
	}
}