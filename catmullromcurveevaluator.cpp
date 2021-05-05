#include "catmullromcurveevaluator.h"

#include "beziercurveevaluator.h"
#include "mat.h"
#include "vec.h"
#include <iostream>
using namespace std;
void CatmullRomCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	ptvEvaluatedCurvePts.clear();

	/*Mat4f m(-1, 3, -3, 1,
			2, -5, 4, -1,
			-1, 0, 1, 0,
			0, 2, 0, 0);
	m *= 0.5;*/

	Mat4f m(
		-tension, 2 - tension, tension - 2, tension,
		2 * tension, tension - 3, 3 - 2 * tension, -tension,
		-tension, 0, tension, 0,
		0, 1, 0, 0
	);

	ptvEvaluatedCurvePts.clear();

	std::vector<Point> ctrlPts;
	ctrlPts.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());

	if (bWrap) {
		ctrlPts.push_back(Point(ptvCtrlPts[0].x + fAniLength, ptvCtrlPts[0].y));
		ctrlPts.push_back(Point(ptvCtrlPts[1].x + fAniLength, ptvCtrlPts[1].y));
		if (ptvCtrlPts.size() >= 3) // if there are 3 or more points, add the first 3 points to the back so that the wrapped curve is natural
			ctrlPts.push_back(Point(ptvCtrlPts[2].x + fAniLength, ptvCtrlPts[2].y));
		else // otherwise add the last point to the front
			ctrlPts.insert(ctrlPts.begin(), Point(ptvCtrlPts[1].x - fAniLength, ptvCtrlPts[1].y));
	}
	else { // add the first and last point again so that the curve is more natural at two ends
		ctrlPts.insert(ctrlPts.begin(), ptvCtrlPts.front());
		ctrlPts.push_back(ptvCtrlPts.back());
		ptvEvaluatedCurvePts.push_back(ptvCtrlPts.front());
		ptvEvaluatedCurvePts.push_back(ptvCtrlPts.back());
	}

	int numPt = ctrlPts.size();
	if (numPt > 3) {
		float prev_x = -fAniLength;
		for (int i = 0; i < numPt - 3; i++) {
			Vec4f px(ctrlPts[i].x, ctrlPts[i + 1].x, ctrlPts[i + 2].x, ctrlPts[i + 3].x);
			Vec4f py(ctrlPts[i].y, ctrlPts[i + 1].y, ctrlPts[i + 2].y, ctrlPts[i + 3].y);
			Vec4f mx = m * px;
			Vec4f my = m * py;
			for (float t = 0.01; t < 1; t += 0.01) {
				Vec4f v(t * t * t, t * t, t, 1);
				float x = v * mx;
				float y = v * my;
				if (x <= ctrlPts[i + 3].x && x >= ctrlPts[i].x && x > prev_x) {
					if (bWrap && x > fAniLength) x -= fAniLength; // wrapped!
					ptvEvaluatedCurvePts.push_back(Point(x, y));
				}
				prev_x = x;
			}
		}
	}

	if (!bWrap) {
		ptvEvaluatedCurvePts.push_back(Point(0, ctrlPts.front().y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ctrlPts.back().y));
	}
}