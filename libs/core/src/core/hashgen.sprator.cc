#include "core/hashgen.h"



#include "core/image.h"
#include "core/rgb.h"
#include "assert/assert.h"
#include "core/rng.h"
#include "core/numeric.h"
#include "core/image_draw.h"
#include "core/cint.h"
#include "core/table_bool.h"
#include "core/generator_cell.h"


namespace
{
    using namespace eu::core;


    Rgbai
    calculate_border_color(const Rgbai& base)
    {
        auto h = to_hsl(to_rgb(base));
        h.h -= Angle::from_degrees(15);
        h.l *= 0.4f;
        return {to_rgbi(to_rgb(h)), base.a};
    }


    void
    apply_sprator_algorithm(BoolTable* half_side, int number_of_steps)
    {
        generator::Rules rules;
        generator::add_complex_rules
        (
            &rules,
            number_of_steps,
            [](bool current, const WallCounter& wc) -> std::optional<bool>
            {
                const auto c = wc.count
                (
                    1,
                    false,
                    NeighborhoodAlgorithm::plus
                );
                if(current)
                {
                    if(c == 2 || c == 3 ) { return true; }
                    else { return false; }
                }
                else
                {
                    if(c <= 1) { return true; }
                    else { return false; }
                }
            }
        );

        auto cell = generator::CellularAutomata
        (
            &rules,
            half_side,
            Lrud<OutsideRule>{OutsideRule::empty}
        );

        while(false == cell.is_done()) { cell.update(); }
    }


    BoolTable
    mirror(const BoolTable& half_side)
    {
        const auto height = half_side.get_height();
        const auto half_width = half_side.get_width();
        const auto width = half_width * 2;

        // offset everything by 1 to get a nice border
        constexpr auto offset = 1;
        constexpr auto extra_size = 2;

        auto result_table = BoolTable::from_width_height
        (
            width + extra_size,
            height + extra_size,
            false
        );

        for(int y=0; y<height; y+=1)
        {
            for(int x=0; x<half_width; x+=1)
            {
                const auto src = half_side[{x, y}];
                const auto x_mirror = width - (x + 1);
                result_table[{x + offset, y + offset}] = src;
                result_table[{x_mirror + offset, y + offset}] = src;
            }
        }

        return result_table;
    }


    int
    calculate_scale(const Image& image, const BoolTable& table)
    {
        auto calculate_scale = [](int image_scale, int table_scale) -> int
        {
            const auto image_scale_float = static_cast<float>(image_scale);
            const auto table_scale_float = static_cast<float>(table_scale);
            const auto scale_factor = image_scale_float / table_scale_float;
            return std::max(1, floor_to_int(scale_factor));
        };

        const auto scale = std::min
        (
            calculate_scale(image.width, table.get_width()),
            calculate_scale(image.height, table.get_height())
        );

        return scale;
    }


    void
    draw_image_with_border
    (
        Image* image,
        const BoolTable& result_table,
        const Rgbai& background_color,
        const Rgbai& foreground_color,
        const Rgbai& border_color
    )
    {
        clear(image, background_color);
        auto img = draw
        (
            result_table,
            foreground_color,
            background_color,
            calculate_scale(*image, result_table),
            BorderSettings{border_color}
        );
        paste_image(image, vec2i{0, 0}, img);
    }


    template
    <
        typename TGenerator
    >
    void
    randomize_world(BoolTable* half_side, TGenerator* generator)
    {
        set_white_noise
        (
            half_side,
            Lrud<BorderSetupRule>{BorderSetupRule::random},
            [&]() -> bool { return generator->get_next_float01() < 0.5f; }
        );
    }


    template
    <
        typename TGenerator,
        typename I
    >
    void
    render_sprator_impl
    (
        Image* image,
        I code,
        const Rgbai& foreground_color,
        const std::optional<Rgbai> border_color_arg,
        const Rgbai& background_color
    )
    {
        constexpr int half_width = 4;
        constexpr int height = 8;
        const int number_of_steps = 3;
        // todo(Gustav): figure out color (randomly?)
        const Rgbai border_color = border_color_arg.value_or
        (
            calculate_border_color(foreground_color)
        );

        auto half_side = BoolTable::from_width_height(half_width, height);

        auto generator = TGenerator{code};

        // randomize world
        randomize_world<TGenerator>(&half_side, &generator);

        // apply sprator algorithm
        apply_sprator_algorithm(&half_side, number_of_steps);

        // flip and copy from small table to big table
        const auto result_table = mirror(half_side);

        // draw image with border
        draw_image_with_border(image, result_table, background_color, foreground_color, border_color);
    }


    template
    <
        typename TGenerator,
        typename I
    >
    void
    render_sprator_impl
    (
        std::vector<Image>* images,
        I code,
        const Rgbai& foreground_color,
        const std::optional<Rgbai> border_color_arg,
        const Rgbai& background_color
    )
    {
        constexpr int half_width = 4;
        constexpr int height = 8;
        const int number_of_steps = 3;
        // todo(Gustav): figure out color (randomly?)
        const Rgbai border_color = border_color_arg.value_or
        (
            calculate_border_color(foreground_color)
        );

        auto half_side = BoolTable::from_width_height(half_width, height);

        auto generator = TGenerator{code};

        // randomize world
        randomize_world<TGenerator>(&half_side, &generator);

        // apply sprator algorithm
        apply_sprator_algorithm(&half_side, number_of_steps - 1);

        bool first = true;

        auto orig_half_side = half_side;

        for(auto& image: *images)
        {
            if(first) { first = false; }
            else
            {
                half_side = orig_half_side;

                // animate...
                auto hit = BoolTable::from_width_height(half_side.get_width(), half_side.get_height(), false);
                for(int animation_frame=0; animation_frame<2; animation_frame+=1)
                {
                    int x = 0;
                    int y = 0;
                    do
                    {
                        x = floor_to_int(generator.get_next_float01() * half_side.get_width());
                        y = floor_to_int(generator.get_next_float01() * half_side.get_height());
                    } while(hit[{x, y}]);
                    hit[{x, y}] = true;
                    half_side[{x, y}] = !half_side[{x, y}];
                }
            }


            apply_sprator_algorithm(&half_side, 1);

            // flip and copy from small table to big table
            const auto result_table = mirror(half_side);

            // draw image with border
            draw_image_with_border(&image, result_table, background_color, foreground_color, border_color);
        }
    }
}


namespace eu::core
{
    void
    render_sprator
    (
        Image* image,
        U32 code,
        const Rgbai& foreground_color,
        std::optional<Rgbai> border_color_arg,
        const Rgbai& background_color
    )
    {
        render_sprator_impl<RandomXorShift32>
        (
                image,
                code,
                foreground_color,
                border_color_arg,
                background_color
        );
    }


    void
    render_sprator
    (
        std::vector<Image>* images,
        U32 code,
        const Rgbai& foreground_color,
        std::optional<Rgbai> border_color_arg,
        const Rgbai& background_color
    )
    {
        render_sprator_impl<RandomXorShift32>
        (
                images,
                code,
                foreground_color,
                border_color_arg,
                background_color
        );
    }
}

