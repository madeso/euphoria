#pragma once

namespace euphoria::core
{
    struct palette;
}

namespace euphoria::core::palettes
{
    // collected by https://github.com/peterhellberg/gfx/blob/master/palettes.go

    // A basic 1-bit (black and white) palette.
    const palette&
    one_bit();

    // A grayscale palette calculated using 2-bits per color.
    // It was used by the original gameboy and a few other computer systems.
    // https://lospec.com/palette-list/2-bit-grayscale
    const palette&
    two_bit_grayscale();

    // A calculated palette using 1 bit for each RGB value.
    // It was used by a number of early computers.
    const palette&
    three_bit();

    // Created by Endesga. #ARQ4
    // https://lospec.com/palette-list/arq4
    const palette&
    arq4();

    // Color Graphics Adapter Palette.
    // CGA was a graphics card released in 1981 for the IBM PC.
    // The standard mode uses one of two 4-color palettes
    // (each with a low-intensity and high-intensity mode),
    // but a hack allows use of all 16. #cga
    // https://lospec.com/palette-list/color-graphics-adapter
    const palette&
    cga();

    // Endesga 8 Palette.
    // Created by Endesga. #EDG8
    // https://lospec.com/palette-list/endesga-8
    const palette&
    endesga_8();

    // Endesga 16 Palette.
    // Created by Endesga. #EDG16
    // https://lospec.com/palette-list/endesga-16
    const palette&
    endesga_16();

    // Endesga 32 Palette.
    // Created by Endesga for his game NYKRA. #EDG32
    // https://lospec.com/palette-list/endesga-32
    const palette&
    endesga_32();

    // Endesga 36 Palette.
    // Created by Endesga. #EDG36
    // https://lospec.com/palette-list/endesga-36
    const palette&
    endesga_36();

    // Endesga 64 Palette.
    // "Honed over years of palette creation, refined for materialistic pixelart
    // and design. High contrast, high saturation, shaped around painting the
    // organic and structured life of the heptaverse." Created by Endesga.
    // #EDG64
    // https://lospec.com/palette-list/endesga-64
    const palette&
    endesga_64();

    // Created by Endesga. #EN4
    // https://lospec.com/palette-list/en4
    const palette&
    endesga_4();

    // Created by AprilSundae.
    // https://lospec.com/palette-list/ink
    const palette&
    ink();

    // The palette used by PICO-8.
    // The PICO-8 is a virtual video game console created by Lexaloffle Games.
    // https://lospec.com/palette-list/pico-8
    const palette&
    pico8();

    // Ammo-8 created by rsvp asap.
    // https://lospec.com/palette-list/ammo-8
    const palette&
    ammo8();

    // Created by Javier Guerrero.
    // https://lospec.com/palette-list/nyx8
    const palette&
    nyx8();

    // 15P DX palette by GrafxKid.
    // https://lospec.com/palette-list/15p-dx
    const palette&
    fifteen_p_dx();

    // 20P DX palette by GrafxKid.
    // https://lospec.com/palette-list/20p-dx
    const palette&
    twenty_p_dx();

    // Created by Arne.
    // https://lospec.com/palette-list/arne-16
    const palette&
    arne16();

    // 3rd place winner of the PixelJoint 16 color palette competition (2015).
    // Created by Night.
    // https://lospec.com/palette-list/night-16
    const palette&
    night16();

    // AAP-16 created by Adigun Polack, meant for beginners.
    // https://lospec.com/palette-list/aap-16
    const palette&
    aap16();

    // AAP-64 palette created by Adigun Polack.
    // https://lospec.com/palette-list/aap-16
    const palette&
    aap64();

    // Splendor 128 palette created by Adigun Polack as a successor to
    // his AAP-64 palette. #Splendor128
    // https://lospec.com/palette-list/aap-splendor128
    const palette&
    splendor128();

    // Famicube palette created by Arne as part of his Famicube Project.
    // https://lospec.com/palette-list/famicube
    const palette&
    famicube();
}
