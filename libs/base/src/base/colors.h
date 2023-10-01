#pragma once


namespace eu
{
    // initially based on http://alumni.media.mit.edu/~wad/color/palette.html
    // pure- colors are values directly from wikipedia
    // short descriptons/attributes from https://www.creativebloq.com/web-design/12-colours-and-emotions-they-evoke-61515112
    // longer description from wikipedia

    enum class NamedColor
    {
        // Clean, virtuous, healthy
        /*
            White is the lightest color and is achromatic. It is the color of fresh snow,
            chalk, and milk, and is the opposite of black. White objects fully reflect
            and scatter all the visible wavelengths of light. White on television and
            computer screens is created by a mixture of red, blue and green light.
        */
        white,

        // Neutral, formal, gloomy
        /*
            Grey or gray is an intermediate color between black and white. It is a
            neutral color or achromatic color, meaning literally that it is a color
            "without color," because it can be composed of black and white. It is the
            color of a cloud-covered sky, of ash and of lead.
        */
        light_gray,
        gray,
        dark_gray,

        // Powerful, sophisticated, edgy
        /*
            Black is the darkest color, the result of the absence or complete
            absorption of visible light. It is an achromatic color, literally a color without
            hue, like white and gray. It is often used symbolically or figuratively to
            represent darkness, while white represents light.
        */
        black,

        // Passionate, aggressive, important
        /*
            Red is the color at the end of the visible spectrum of light, next to orange
            and opposite violet. It has a dominant wavelength of approximately
            625–740 nanometres. It is a primary color in the RGB color model and the
            CMYK color model, and is the complementary color of cyan.
        */
        red,
        pure_red,

        // Serene, trustworthy, inviting
        /*
            Blue is one of the three primary colours of pigments in painting and
            traditional colour theory, as well as in the RGB colour model. It lies
            between violet and green on the spectrum of visible light. The eye
            perceives blue when observing light with a dominant wavelength between
            approximately 450 and 495 nanometres.
        */
        blue,
        pure_blue,
        light_blue,
        normal_blue,

        /*
            Cornflower blue is a shade of medium-to-light blue containing relatively
            little green compared to blue. This hue was one of the favorites of the
            Dutch painter Johannes Vermeer. The most valuable blue sapphires are
            called cornflower blue, having a medium-dark violet-blue tone.
        */
        cornflower_blue,

        // Natural, stable, prosperous
        /*
            Green is the color between blue and yellow on the visible spectrum. It is
            evoked by light which has a dominant wavelength of roughly 495–570 nm.
        */
        green,
        pure_green,
        light_green,

        // Happy, friendly, warning
        /*
            Yellow is the color between orange and green on the spectrum of visible
            light. It is evoked by light with a dominant wavelength of roughly 570–590 nm.
            It is a primary color in subtractive color systems, used in painting or
            color printing.
        */
        yellow,
        pure_yellow,

        // Playful, energetic, cheap
        /*
            Orange is the colour between yellow and red on the spectrum of visible
            light. Human eyes perceive orange when observing light with a dominant
            wavelength between roughly 585 and 620 nanometres. In painting and
            traditional colour theory, it is a secondary colour of pigments, created by
            mixing yellow and red.
        */
        orange,
        pure_orange,

        // Earthy, sturdy, rustic
        /*
            Brown is a composite color. In the CMYK color model used in printing or
            painting, brown is made by combining red, black, and yellow, or red,
            yellow, and blue. In the RGB color model used to project colors onto
            television screens and computer monitors, brown is made by combining
            red and green, in specific proportions.
        */
        brown,
        pure_brown,

        // Luxurious, mysterious, romantic
        /*
            Purple is a color intermediate between blue and red. It is similar to violet,
            but unlike violet, which is a spectral color with its own wavelength on the
            visible spectrum of light, purple is a secondary color made by combining
            red and blue. The complementary color of purple in the RYB color model
            is yellow.
        */
        purple,
        pure_purple,

        // Feminine, young, innocent
        /*
            Pink is a pale red color that is named after a flower of the same name. It
            was first used as a color name in the late 17th century.
        */
        pink,
        pure_pink,

        // Accentuates surrounding colours
        /*
            Beige is variously described as a pale sandy fawn color, a grayish tan, a
            light-grayish yellowish brown, or a pale to grayish yellow. It takes its
            name from French, where the word originally meant natural wool that has
            been neither bleached nor dyed, and hence also the color of natural wool.
        */
        pure_beige,


        /*
            Tan is a pale tone of brown. The name is derived from tannum (oak bark)
            used in the tanning of leather.
        */
        tan,
        pure_tan,

        /*
            Cyan is a greenish-blue color. It is evoked by light with a predominant wavelength
            of between 490–520 nm, between the wavelengths of green and blue.
        */
        cyan,
        pure_cyan
    };


    // Colors optimized for color-blind individuals
    // https://www.nature.com/articles/nmeth.1618
    // renamed to have main color first
    enum class BangWongPalette
    {
        // black
        black,

        // orange
        organge,

        // sky blue
        blue_sky,

        // bluish green
        green_bluish,

        // yellow
        yellow,

        // blue
        blue,

        // vermillion
        red_vermillion,

        // redish purple
        purple_redish
    };
}
