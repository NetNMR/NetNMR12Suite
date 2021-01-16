#pragma once

/**
	@returns hypotenuse of real (non-complex) scalars a and b by 
	avoiding underflow/overflow
	using (a * sqrt( 1 + (b/a) * (b/a))), rather than
	sqrt(a*a + b*b).
*/
template <class Real>
Real my_hypot(const Real &a, const Real &b)
{
	
	if (a== 0)
		return abs(b);
	else
	{
		Real c = b/a;
		return fabs(a) * sqrt(1 + c*c);
	}
}

