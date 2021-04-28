#pragma once
#pragma warning(disable : 4786)

#include "curveevaluator.h"

class CatmullRomCurveEvaluator : public CurveEvaluator
{
public:
	CatmullRomCurveEvaluator() : CurveEvaluator(), tension(0.5) {}
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;
	void adjustTension(float dTension) { tension += dTension; }

private:
	float tension;
};

