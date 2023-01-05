#pragma once

namespace euphoria::core
{


 /**
  * A generalized curve formula.
  *
  * tor curve is a generalized, parameterized curve shaping function,
  * especially useful for easing functions and interpolation.
  * DESMOS DEMO: https://www.desmos.com/calculator/yvbmqxxphj
  *
  * Originally based on Colugo's curve editor: https://twitter.com/ColugoMusic/status/1363071439679729665?s=20
  * which is based off of Yann van der Cruyssen's sCurve formula: https://twitter.com/Morusque/status/1352569197499441155
  * which is based off of this paper: https://arxiv.org/pdf/2010.09714.pdf
  *
  * tor curve expands upon this by adding a third parameter
  * controlling the falloffs/asymptotes, for both the horizontal
  * and vertical axes.
  * @author torcado, 2021 (javascript)
  * @author madeso (c++ port)
  * 
  * @param x the input value, range 0-1
  * @param a defines the general slope of the curve
  * @param b defines the midpoint location
  * @param c defines the flatness of the curve at the midpoint/ends
  */
float tor_curve(float x, float a, float b, float c);

}
