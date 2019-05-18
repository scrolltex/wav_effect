#pragma once
#include "utility.h"
#include <algorithm>

enum CurveType
{
	Linear,
	Logarithmic,
	Sine
};

/**
 * \brief Apply curve for value
 * \param x value between 0 and 1
 * \param curveType curve to apply
 */
[[nodiscard]] inline float applyCurve(float x, CurveType curveType)
{
	x = std::clamp(x, 0.f, 1.f);

	switch (curveType)
	{
		case Logarithmic: return log(1.f / (1.f - (exp(1.f) - 1.f) * (x - 1.f))) + 1.f;
		case Sine: return pow(sin((5.f * x) / pi), 2);
		case Linear: [[fallthrough]]
		default: return x;
	}
}
