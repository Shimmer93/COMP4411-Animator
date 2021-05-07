#include "c2splinecurveevaluator.h"
#include "mat.h"
#include "vec.h"
#include <iostream>
using namespace std;

void C2SplineCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	ptvEvaluatedCurvePts.clear();

	Mat4f m(2, -2, 1, 1,
			-3, 3, -2, -1,
			0, 0, 1, 0,
			1, 0, 0, 0);

	std::vector<Point> ctrlPts;
	ctrlPts.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());

	if (bWrap) {
		ctrlPts.push_back(Point(ptvCtrlPts[0].x + fAniLength, ptvCtrlPts[0].y));
	}

	const int numPt = ctrlPts.size();
	if (numPt < 2) return;

	float* xdiffs = new float[numPt];
	float* xgammas = new float[numPt];
	float* xsigmas = new float[numPt];
	float* xderivs = new float[numPt];
	float* ydiffs = new float[numPt];
	float* ygammas = new float[numPt];
	float* ysigmas = new float[numPt];
	float* yderivs = new float[numPt];

	xdiffs[0] = 3 * (ctrlPts[1].x - ctrlPts[0].x);
	for (int i = 1; i < numPt - 1; i++)
		xdiffs[i] = 3 * (ctrlPts[i + 1].x - ctrlPts[i - 1].x);
	xdiffs[numPt - 1] = 3 * (ctrlPts[numPt - 1].x - ctrlPts[numPt - 2].x);

	xgammas[0] = 0.5;
	for (int i = 1; i < numPt - 1; i++)
		xgammas[i] = 1.0 / (4.0 - xgammas[i - 1]);
	xgammas[numPt - 1] = 1.0 / (2.0 - xgammas[numPt - 2]);

	xsigmas[0] = xdiffs[0] * xgammas[0];
	for (int i = 1; i < numPt; i++)
		xsigmas[i] = (xdiffs[i] - xsigmas[i - 1]) * xgammas[i];

	xderivs[numPt - 1] = xsigmas[numPt - 1];
	for (int i = numPt - 2; i >= 0; i--)
		xderivs[i] = xsigmas[i] - xgammas[i] * xderivs[i + 1];

	ydiffs[0] = 3 * (ctrlPts[1].y - ctrlPts[0].y);
	for (int i = 1; i < numPt - 1; i++)
		ydiffs[i] = 3 * (ctrlPts[i + 1].y - ctrlPts[i - 1].y);
	ydiffs[numPt - 1] = 3 * (ctrlPts[numPt - 1].y - ctrlPts[numPt - 2].y);
	if (bWrap) {
		ydiffs[numPt - 1] = ydiffs[0] = 3 * (ctrlPts[1].y - ctrlPts[numPt - 2].y);
	}

	ygammas[0] = 0.5;
	for (int i = 1; i < numPt - 1; i++)
		ygammas[i] = 1.0 / (4.0 - ygammas[i - 1]);
	ygammas[numPt - 1] = 1.0 / (2.0 - ygammas[numPt - 2]);

	ysigmas[0] = ydiffs[0] * ygammas[0];
	for (int i = 1; i < numPt; i++)
		ysigmas[i] = (ydiffs[i] - ysigmas[i - 1]) * ygammas[i];

	yderivs[numPt - 1] = ysigmas[numPt - 1];
	for (int i = numPt - 2; i >= 0; i--)
		yderivs[i] = ysigmas[i] - ygammas[i] * yderivs[i + 1];

	float prev_x = -fAniLength;
	for (int i = 0; i < numPt - 1; i++) {
		Vec4f px(ctrlPts[i].x, ctrlPts[i + 1].x, xderivs[i], xderivs[i + 1]);
		Vec4f py(ctrlPts[i].y, ctrlPts[i + 1].y, yderivs[i], yderivs[i + 1]);
		Vec4f mx = m * px;
		Vec4f my = m * py;
		for (float t = 0.0; t < 1; t += 0.01) {
			Vec4f v(t * t * t, t * t, t, 1);
			float x = v * mx;
			float y = v * my;
			if (x <= ctrlPts[i + 1].x && x >= ctrlPts[i].x && x > prev_x) {
				if (bWrap && x > fAniLength) x -= fAniLength; // wrapped!
				ptvEvaluatedCurvePts.push_back(Point(x, y));
			}
			prev_x = x;
		}
	}

	if (!bWrap) {
		ptvEvaluatedCurvePts.push_back(Point(0, ctrlPts.front().y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ctrlPts.back().y));
	}

	delete[] xdiffs;
	delete[] xgammas;
	delete[] xsigmas;
	delete[] xderivs;
	delete[] ydiffs;
	delete[] ygammas;
	delete[] ysigmas;
	delete[] yderivs;
}