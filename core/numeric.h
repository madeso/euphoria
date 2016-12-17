#ifndef CORE_NUMERIC_H
#define CORE_NUMERIC_H

bool IsEqual(float lhs, float rhs);
bool IsZero(float r);
float ZeroOrValue(float r);

/** Calculates the sign as a positive or a negative int.
@returns 1 if r is greater than 0, -1 if not.
@see Abs()
*/
const int Sign(const float r);

const float Lerp(const float f, float scale, const float t);

/** Performs a single interpolating step to transform a old value to a new value.
Larger smoothing values result in a smoother but less resposive path

Example:
@code mousex = Curve( ( MousePos.X - OldMousePos.X ) * Sensitivity(), mousex, 6 ); @endcode

@param target is the target value
@param current is the current value
@param smoothing is the smoothing value > 0, the lower, the less smooth

@returns The smoothed result.
*/
const float Curve(const float target, const float current, const float smoothing);

/** Calculates the square of the argument.
@see Sqrt()
*/
const float Square(const float r);

/** Calculates the square-root of the argument.
@see Square()
*/
const float Sqrt(const float r);

/** Calculates the absolute value of the argument.
@see Sign()
*/
const float Abs(const float r);

/** Calculates the minimum of two values
@see Max()
@see KeepWithin()
*/
const float Min(const float lhs, const float rhs);

/** Calculates the maximum of two values.
@see Min()
@see KeepWithin()
*/
const float Max(const float lhs, const float rhs);

/** Transforms a value from one range to the 0-1 range.
This function does not limmit the value, so if it's below the lower bound it will be below 0.
@param L the lower bound of the range
@param v the value
@param U the upper bound of the range
@returns The tranformed value.
@see From01()
@see Remap()
@see KeepWithin()
*/
const float To01(const float L, const float v, const float U);

/** Transforms a value from the 0-1 range to another range.
This function does not limmit he value, so if it's below 0 the result will be below the lower bound.
@param L the lower bound of the range
@param v the value
@param U the upper bound of the range
@returns The tranformed value
@see To01()
@see KeepWithin()
@see Remap()
*/
const float From01(const float L, const float v, const float U);

/** Remaps/tranforms from one range to another.
This function does not limmit it's input, so if the value is outside the original range, it will be outside the new range.
@param ol old lower range
@param ou old upper range
@param v the value
@param nl new lower range
@param nu new upper range
@returns The transformed value
@see From01()
@see To01()
@see KeepWithin()
*/
const float Remap(const float ol, const float ou, const float v, const float nl, const float nu);

/** Calculate smooth value suitable for infinite looping.
@param min is the minimum output value
@param value is a float that indicates what to return
             0 means minimum, 0.5 means maximum and 1 is mimimum again, the values inbetween follow a sinus/cosinus curve doing a full 360.
@param max is the maximum output value
@returns a value that lies between min and max
*/
const float Get360Angular(const float min, const float value, const float max);

/** Keeps a value within a minimum and a maximum.
Limmits it to the range if it get's outside.
@param min is the minimum value
@param v is the value to limmit
@param max is the maximum value
@returns the limmited value
@see Min()
@see Max()
@see Wrap()
@see IsWithin()
*/
const float KeepWithin(const float min, const float v, const float max);

/** Checks wheter a value is within a range.
@see KeepWithin()
*/
const bool IsWithin(const float min, const float c, const float max);

/** Checks wheter a value is within a range, beeing at the border is ok.
@see KeepWithin()
*/
const bool IsWithinInclusive(const float min, const float c, const float max);

/** Keeps a value within a range.
Wraps it to the others side if it gets outside.
@param min is the lower range
@param v is the value to be wrapped
@param max is the upper range
@returns the wrapped value
@see KeepWithin()
@see Wrapi()
@see IncrementAndWrap()
*/
const float Wrap(const float min, const float v, const float max);

/** Keeps a value within a range.
Wraps it to the others side if it gets outside.
@param min is the lower range
@param v is the value to be wrapped
@param max is the upper range
@returns the wrapped value
@see KeepWithin()
@see Wrap()
@see IncrementAndWrapi()
*/
const int Wrapi(const int min, const int v, const int max);

/** rounds a value to the nearest nice value.
If the granularity is 1 the function rounds to the closest integer, at .5 the closest half integer, at 2 even integers etc...
@param num the number to round
@param gran the granularity
*/
const float Round(const float num, const float gran);

/** Increment and wrap a value.
@returns return number of wraps (with sign)
@see Wrap()
*/
const int IncrementAndWrap(const float min, float* current, const float change, const float max);

/** Increment and wrap a value.
@returns return number of wraps (with sign)
@see Wrapi()
*/
const int IncrementAndWrapi(const int min, int* current, const int change, const int max);

/// @}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup constants Constants
/// @{

/** The matematical constant pi.
@see HalfPi()
*/
const float Pi();

/** The matematical constant pi divided by 2.
@see Pi()
*/
const float HalfPi();

/** The boolean value true.
@see kFalse()
*/
const bool kTrue();

/** The boolean value false.
@see kTrue()
*/
const bool kFalse();

#endif  // CORE_NUMERIC_H