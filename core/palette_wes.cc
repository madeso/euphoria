#include "core/palette_wes.h"

#include "core/palette.h"

// source: https://github.com/jiffyclub/palettable/blob/master/palettable/wesanderson/wesanderson.py

namespace
{
  Rgbi
  C(unsigned char r, unsigned char g, unsigned char b)
  {
    return {r, g, b};
  }
}  // namespace

namespace palette
{
    // http://wesandersonpalettes.tumblr.com/post/79263620764/hotel-chevalier
    const Palette&
    Chevalier()
    {
      static const auto p = Palette {"Chevalier",
            C(53, 82, 67),
              C(254, 202, 73),
              C(201, 213, 213),
              C(187, 162, 137)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/79263667140/sam-i-love-you-but-you-dont-know-what-youre
    const Palette&
    Moonrise1()
    {
      static const auto p = Palette {"Moonrise1",
            C(114, 202, 221),
              C(240, 165, 176),
              C(140, 133, 54),
              C(195, 180, 119),
            C(250, 208, 99)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/79348206200/mendls-heaven
    const Palette&
    Mendl()
    {
      static const auto p = Palette {"Mendl",
            C(222, 141, 185),
              C(184, 192, 246),
              C(207, 147, 135),
              C(92, 128, 204)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/79348364517/margot-takes-a-bath
    const Palette&
    Margot1()
    {
      static const auto p = Palette {"Margot1",
            C(137, 119, 18),
              C(243, 194, 164),
              C(246, 159, 151),
              C(254, 214, 140),
            C(98, 144, 117)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/79348553036/castello-cavalcanti-how-can-i-help
    const Palette&
    Cavalcanti()
    {
      static const auto p = Palette {"Cavalcanti",
            C(209, 170, 0),
              C(8, 50, 19),
              C(146, 148, 96),
              C(111, 152, 121),
            C(132, 33, 17)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/79641731527/sam-why-do-you-always-use-binoculars-suzy-it
    const Palette&
    Moonrise2()
    {
      static const auto p = Palette {"Moonrise2",
            C(102, 124, 116),
              C(181, 106, 39),
              C(194, 186, 124),
              C(31, 25, 23)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/79641785036/margot-takes-a-break
    const Palette&
    Margot2()
    {
      static const auto p = Palette {"Margot2",
            C(118, 139, 147),
              C(188, 36, 15),
              C(249, 236, 197),
              C(212, 115, 41)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/79783357790/suzy-ive-always-wanted-to-be-an-orphan-most-of
    const Palette&
    Moonrise3()
    {
      static const auto p = Palette {"Moonrise3",
            C(242, 218, 82),
              C(197, 157, 0),
              C(203, 203, 201),
              C(27, 30, 20)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/79784389334/the-grand-budapest-hotel
    const Palette&
    GrandBudapest1()
    {
      static const auto p = Palette {"GrandBudapest1",
            C(238, 174, 101),
              C(251, 79, 85),
              C(72, 19, 19),
              C(204, 95, 39)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/79956897654/coming-soon
    const Palette&
    Moonrise4()
    {
      static const auto p = Palette {"Moonrise4",
            C(123, 135, 97),
              C(193, 166, 46),
              C(79, 143, 107),
              C(59, 69, 60),
            C(159, 50, 8)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/79956949771/steve-zissou-dont-point-that-gun-at-him-hes-an
    const Palette&
    Zissou()
    {
      static const auto p = Palette {"Zissou",
            C(0, 153, 230),
              C(18, 37, 90),
              C(242, 56, 20),
              C(223, 183, 139),
            C(182, 195, 197)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/79957796915/royal-o-reilly-tenenbaum-1932-2001
    const Palette&
    Royal1()
    {
      static const auto p = Palette {"Royal1",
            C(121, 164, 58),
              C(242, 214, 175),
              C(94, 72, 41),
              C(24, 20, 1)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/80149649946/jack-i-wonder-if-the-three-of-us-wouldve-been
    const Palette&
    Darjeeling1()
    {
      static const auto p = Palette {"Darjeeling1",
            C(158, 151, 151),
              C(194, 142, 0),
              C(131, 102, 89),
              C(156, 90, 51)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/80149872170/mrs-fox-you-know-you-really-are-fantastic-mr
    const Palette&
    FantasticFox1()
    {
      static const auto p = Palette {"FantasticFox1",
            C(249, 219, 32),
              C(147, 75, 78),
              C(66, 23, 13),
              C(194, 121, 34),
            C(226, 200, 167)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/109473707895/etheline-raleigh-says-youve-been-spending-six
    const Palette&
    Margot3()
    {
      static const auto p = Palette {"Margot3",
            C(135, 162, 164),
              C(202, 160, 101),
              C(214, 202, 191),
              C(214, 160, 160)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/109473911685/m-gustave-you-see-there-are-still-faint
    const Palette&
    GrandBudapest2()
    {
      static const auto p = Palette {"GrandBudapest2",
            C(255, 166, 142),
              C(251, 204, 183),
              C(140, 17, 8),
              C(41, 11, 4)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/109568074320/steve-zissou-the-deeper-you-go-the-weirder-life
    const Palette&
    Aquatic1()
    {
      static const auto p = Palette {"Aquatic1",
            C(52, 36, 25),
              C(28, 64, 39),
              C(241, 201, 14),
              C(102, 88, 153),
            C(184, 147, 130)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/109980167015/peter-fuck-the-itinerary
    const Palette&
    Darjeeling2()
    {
      static const auto p = Palette {"Darjeeling2",
            C(213, 227, 216),
              C(97, 138, 152),
              C(249, 218, 149),
              C(174, 75, 22),
            C(120, 112, 100)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/110716093015/ash-should-we-dance
    const Palette&
    FantasticFox2()
    {
      static const auto p = Palette {"FantasticFox2",
            C(228, 191, 68),
              C(198, 87, 66),
              C(154, 208, 187),
              C(51, 39, 55),
            C(171, 161, 141)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/112305028860/m-gustave-mendls-is-the-best
    const Palette&
    GrandBudapest3()
    {
      static const auto p = Palette {"GrandBudapest3",
            C(255, 220, 182),
              C(37, 56, 69),
              C(231, 173, 157),
              C(102, 117, 110),
            C(139, 63, 49),
            C(150, 109, 53)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/115124780615/royal-anybody-interested-in-grabbing-a-couple-of
    const Palette&
    Royal2()
    {
      static const auto p = Palette {"Royal2",
            C(194, 171, 186),
              C(140, 59, 73),
              C(182, 172, 166),
              C(33, 32, 83),
            C(209, 211, 213)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/116909186645/walt-bishop-our-daughters-been-abducted-by-one
    const Palette&
    Moonrise5()
    {
      static const auto p = Palette {"Moonrise5",
            C(223, 140, 144),
              C(216, 210, 142),
              C(245, 190, 37),
              C(61, 74, 28),
            C(209, 48, 96),
            C(168, 107, 76)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/117849683385/concierge-and-how-long-will-you-be-staying-with
    const Palette&
    GrandBudapest4()
    {
      static const auto p = Palette {"GrandBudapest4",
            C(186, 110, 0),
              C(80, 32, 86),
              C(255, 246, 187),
              C(154, 127, 25),
            C(31, 18, 27)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/118877161325/sam-im-not-that-strong-of-a-swimmer-so-i-wear-a
    const Palette&
    Moonrise6()
    {
      static const auto p = Palette {"Moonrise6",
            C(191, 81, 61),
              C(201, 162, 150),
              C(197, 193, 136),
              C(123, 177, 145),
            C(217, 174, 48)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/122169507295/m-gustave-its-quite-a-thing-winning-the-loyalty
    const Palette&
    GrandBudapest5()
    {
      static const auto p = Palette {"GrandBudapest5",
            C(225, 146, 131),
              C(140, 27, 76),
              C(209, 147, 54),
              C(231, 199, 190),
            C(51, 12, 0)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/125170837755/steve-zissou-please-dont-make-fun-of-me-i-just
    const Palette&
    Aquatic2()
    {
      static const auto p = Palette {"Aquatic2",
            C(139, 156, 184),
              C(233, 229, 65),
              C(88, 159, 194),
              C(160, 141, 94),
            C(189, 185, 206)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/129921576355/royal-ive-always-been-considered-an-asshole-for
    const Palette&
    Royal3()
    {
      static const auto p = Palette {"Royal3",
            C(252, 87, 108),
              C(237, 126, 83),
              C(226, 153, 139),
              C(46, 23, 24),
            C(251, 214, 202)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/137096576550/social-services-access-denied
    const Palette&
    Moonrise7()
    {
      static const auto p = Palette {"Moonrise7",
            C(35, 35, 85),
              C(97, 104, 96),
              C(167, 91, 68),
              C(145, 43, 41),
            C(255, 227, 143)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/139482629630/ah-weve-never-made-great-husbands-have-we-of
    const Palette&
    Aquatic3()
    {
      static const auto p = Palette {"Aquatic3",
            C(214, 161, 66),
              C(194, 128, 114),
              C(200, 183, 161),
              C(189, 68, 45),
            C(100, 84, 60)
      };
      return p;
    }

    // http://wesandersonpalettes.tumblr.com/post/143938510215/the-chief-steward-welcome-aboard
    const Palette&
    Darjeeling3()
    {
      static const auto p = Palette {"Darjeeling3",
            C(168, 171, 80),
              C(255, 232, 61),
              C(169, 211, 210),
              C(36, 71, 125),
            C(90, 145, 124)
      };
      return p;
    }

}  // namespace palette

