#ifndef CORE_NAMED_COLORS_H
#define CORE_NAMED_COLORS_H

#include "core/enumtostring.h"

// todo: add custom colors, shader blue?

// based on http://alumni.media.mit.edu/~wad/color/palette.html

// descriptons from https://www.creativebloq.com/web-design/12-colours-and-emotions-they-evoke-61515112
// longer description from wikipedia
// change blog colors to colors with a Nice prefix and replace the non-nice colors with wikipedia constants

enum class Color
{
  // Powerful, sophisticated, edgy
  /*
    Black is the darkest color, the result of the absence or complete
    absorption of visible light. It is an achromatic color, literally a color without
    hue, like white and gray. It is often used symbolically or figuratively to
    represent darkness, while white represents light.
   */
  Black,

  // Neutral, formal, gloomy
  /*
    Grey or gray is an intermediate color between black and white. It is a
    neutral color or achromatic color, meaning literally that it is a color
    "without color," because it can be composed of black and white. It is the
    color of a cloud-covered sky, of ash and of lead.
   */
  Gray, LightGray, DarkGray,

  // Clean, virtuous, healthy
  /*
    White is the lightest color and is achromatic. It is the color of fresh snow,
    chalk, and milk, and is the opposite of black. White objects fully reflect
    and scatter all the visible wavelengths of light. White on television and
    computer screens is created by a mixture of red, blue and green light.
   */
  White,

  // Passionate, aggressive, important
  /*
    Red is the color at the end of the visible spectrum of light, next to orange
    and opposite violet. It has a dominant wavelength of approximately
    625–740 nanometres. It is a primary color in the RGB color model and the
    CMYK color model, and is the complementary color of cyan.
   */
  Red,
  
  // Serene, trustworthy, inviting
  /*
    Blue is one of the three primary colours of pigments in painting and
    traditional colour theory, as well as in the RGB colour model. It lies
    between violet and green on the spectrum of visible light. The eye
    perceives blue when observing light with a dominant wavelength between
    approximately 450 and 495 nanometres.
   */
  Blue, LightBlue,

  // Natural, stable, prosperous
  /*
    Green is the color between blue and yellow on the visible spectrum. It is
    evoked by light which has a dominant wavelength of roughly 495–570 nm.
   */
  Green, LightGreen,

  // Happy, friendly, warning
  /*
    Yellow is the color between orange and green on the spectrum of visible
    light. It is evoked by light with a dominant wavelength of roughly 570–590 nm.
    It is a primary color in subtractive color systems, used in painting or
    color printing. 
   */
  Yellow,

  // Playful, energetic, cheap
  /*
    Orange is the colour between yellow and red on the spectrum of visible
    light. Human eyes perceive orange when observing light with a dominant
    wavelength between roughly 585 and 620 nanometres. In painting and
    traditional colour theory, it is a secondary colour of pigments, created by
    mixing yellow and red.
   */
  Orange,

  // Earthy, sturdy, rustic
  /*
    Brown is a composite color. In the CMYK color model used in printing or
    painting, brown is made by combining red, black, and yellow, or red,
    yellow, and blue. In the RGB color model used to project colors onto
    television screens and computer monitors, brown is made by combining
    red and green, in specific proportions. 
   */
  Brown,

  // Luxurious, mysterious, romantic
  /*
    Purple is a color intermediate between blue and red. It is similar to violet,
    but unlike violet, which is a spectral color with its own wavelength on the
    visible spectrum of light, purple is a secondary color made by combining
    red and blue. The complementary color of purple in the RYB color model
    is yellow. 
   */
  Purple,

  // Feminine, young, innocent
  /*
    Pink is a pale red color that is named after a flower of the same name. It
    was first used as a color name in the late 17th century.
   */
  Pink,
  
  // Accentuates surrounding colours
  /*
    Beige is variously described as a pale sandy fawn color, a grayish tan, a
    light-grayish yellowish brown, or a pale to grayish yellow. It takes its
    name from French, where the word originally meant natural wool that has
    been neither bleached nor dyed, and hence also the color of natural wool.
   */
  Beige,

  /*
    Cornflower blue is a shade of medium-to-light blue containing relatively
    little green compared to blue. This hue was one of the favorites of the
    Dutch painter Johannes Vermeer. The most valuable blue sapphires are
    called cornflower blue, having a medium-dark violet-blue tone.
   */
  CornflowerBlue,

  /*
    Tan is a pale tone of brown. The name is derived from tannum (oak bark)
    used in the tanning of leather.
  */
  Tan,

  /*
    Cyan is a greenish-blue color. It is evoked by light with a predominant wavelength
    of between 490–520 nm, between the wavelengths of green and blue. 
  */
  Cyan,

  MAX_VALUE
};

BEGIN_ENUM_LIST(Color)
  ENUM_VALUE(Color, Black)
  ENUM_VALUE(Color, DarkGray)
  ENUM_VALUE(Color, Red)
  ENUM_VALUE(Color, Blue)
  ENUM_VALUE(Color, Green)
  ENUM_VALUE(Color, Brown)
  ENUM_VALUE(Color, Purple)
  ENUM_VALUE(Color, LightGray)
  ENUM_VALUE(Color, LightGreen)
  ENUM_VALUE(Color, LightBlue)
  ENUM_VALUE(Color, Cyan)
  ENUM_VALUE(Color, Orange)
  ENUM_VALUE(Color, Yellow)
  ENUM_VALUE(Color, Tan)
  ENUM_VALUE(Color, Pink)
  ENUM_VALUE(Color, White)
END_ENUM_LIST()

unsigned int
ToColorHex(Color color);

#endif  // CORE_NAMED_COLORS_H
