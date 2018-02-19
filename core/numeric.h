#ifndef CORE_NUMERIC_H
#define CORE_NUMERIC_H

bool
IsEqual(float lhs, float rhs);
bool
IsZero(float r);
float
ZeroOrValue(float r);

float
Floor(float v);

float
Ceil(float v);

int
Floori(float v);

int
Ceili(float v);

/** Calculates the sign as a positive or a negative int.
@returns 1 if r is greater than 0, -1 if not.
@see Abs()
*/
int
Sign(float r);

float
Lerp(float f, float scale, float t);

/** Performs a single interpolating step to transform a old value to a new
value.
Larger smoothing values result in a smoother but less resposive path

Example:
@code mousex = Curve( ( MousePos.X - OldMousePos.X ) * Sensitivity(), mousex, 6
); @endcode

@param target is the target value
@param current is the current value
@param smoothing is the smoothing value > 0, the lower, the less smooth

@returns The smoothed result.
*/
float
Curve(float new_value, float old_value, float smoothing_value);

/** Calculates the square of the argument.
@see Sqrt()
*/
float
Square(float r);

/** Calculates the square-root of the argument.
@see Square()
*/
float
Sqrt(float r);

/** Calculates the absolute value of the argument.
@see Sign()
*/
float
Abs(float r);

/** Calculates the minimum of two values
@see Max()
@see KeepWithin()
*/
float
Min(float lhs, float rhs);
int
Min(int lhs, int rhs);

/** Calculates the maximum of two values.
@see Min()
@see KeepWithin()
*/
float
Max(float lhs, float rhs);
int
Max(int lhs, int rhs);

bool
IsWithinInclusivei(int min, int c, int max);

/** rounds a value to the nearest nice value.
If the granularity is 1 the function rounds to the closest integer, at .5 the
closest half integer, at 2 even integers etc...
@param num the number to round
@param gran the granularity
*/
float
Round(float num, float gran);

/// @}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup constants Constants
/// @{

/** The matematical constant pi.
@see HalfPi()
*/
float
Pi();

/** The matematical constant pi divided by 2.
@see Pi()
*/
float
HalfPi();

/** The boolean value true.
@see kFalse()
*/
const bool
TrueValue();

/** The boolean value false.
@see kTrue()
*/
const bool
FalseValue();

template <typename T>
T
DefaultIfCloseToZero(T value, T def, T epsilon)
{
  if(Abs(value) < epsilon)
    return def;
  else
    return value;
}

#endif  // CORE_NUMERIC_H
