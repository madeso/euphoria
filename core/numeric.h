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

template <typename T>
void
UpdateMax(T* t, T value)
{
  *t = Max(*t, value);
}

/** Transforms a value from one range to the 0-1 range.
This function does not limmit the value, so if it's below the lower bound it
will be below 0.
@param lower_bound the lower bound of the range
@param value the value
@param upper_bound the upper bound of the range
@returns The tranformed value.
@see From01()
@see Remap()
@see KeepWithin()
*/
float
To01(float lower_bound, float value, float upper_bound);

/** Transforms a value from the 0-1 range to another range.
This function does not limmit he value, so if it's below 0 the result will be
below the lower bound.
@param lower_bound the lower bound of the range
@param value the value
@param upper_bound the upper bound of the range
@returns The tranformed value
@see To01()
@see KeepWithin()
@see Remap()
*/
float
From01(float lower_bound, float value, float upper_bound);

/** Remaps/tranforms from one range to another.
This function does not limmit it's input, so if the value is outside the
original range, it will be outside the new range.
@param old_lower_bound old lower range
@param old_upper_bound old upper range
@param value the value
@param new_lower_bound new lower range
@param new_upper_bound new upper range
@returns The transformed value
@see From01()
@see To01()
@see KeepWithin()
*/
float
Remap(
    float old_lower_bound,
    float old_upper_bound,
    float value,
    float new_lower_bound,
    float new_upper_bound);

/** Calculate smooth value suitable for infinite looping.
@param min is the minimum output value
@param value is a float that indicates what to return
             0 means minimum, 0.5 means maximum and 1 is mimimum again, the
values inbetween follow a sinus/cosinus curve doing a full 360.
@param max is the maximum output value
@returns a value that lies between min and max
*/
float
Get360Angular(float min, float value, float max);

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
float
KeepWithin(float min, float v, float max);

/** Checks wheter a value is within a range.
@see KeepWithin()
*/
const bool
IsWithin(float min, float c, float max);

/** Checks wheter a value is within a range, beeing at the border is ok.
@see KeepWithin()
*/
const bool
IsWithinInclusive(float min, float c, float max);

const bool
IsWithinInclusivei(int min, int c, int max);

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
float
Wrap(float min, float v, float max);

float
PingPong01(float v);
float
PingPong(float min, float v, float max);

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
int
Wrapi(int min, int v, int max);

/** rounds a value to the nearest nice value.
If the granularity is 1 the function rounds to the closest integer, at .5 the
closest half integer, at 2 even integers etc...
@param num the number to round
@param gran the granularity
*/
float
Round(float num, float gran);

/** Increment and wrap a value.
@returns return number of wraps (with sign)
@see Wrap()
*/
int
IncrementAndWrap(float min, float* current, float change, float max);

/** Increment and wrap a value.
@returns return number of wraps (with sign)
@see Wrapi()
*/
int
IncrementAndWrapi(int min, int* current, int change, int max);

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
