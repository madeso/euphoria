This repo contains 2 different projects that are slightly related and used together: "Min synth" and "Musik Maskin".


"Min synth" is a small software synth intended for integration into other projects.
The name is swedish and means "my synth", but it could also mean "minimal (or minimum) synth" (works in english too).

"Musik maskin" is a music making application using "min synth".
The name is swedish for "music machine".


Both projects strive to be configurable to be easily built into a game/engine but there should also be something download and runnable when it's done.


Min synth
=========

** Current features **

 * One single somewhat configurable oscilator.


** Planned features **

 * envelopes
 * arpegiators
 * polyphonic output
 * integer based
 * load instruments from file
 * simple effects like reverb, delay and bitcrusher
   inspiration: https://github.com/ElectroSmash/pedal-pi
 * samples
 * soundfonts
 * possible nyquist plugins
 * basic general audio input, so you can connect a guitar and play
 * perhaps create a fake buzz interface to use the open source [buzz machines](https://github.com/Buzztrax/buzzmachines)

Musik maskin
=========

** Current features **

 * play sound on keyboard input, choose the oscilator type and master sound in a "dear imgui" based gui.


** Planned features **

 * configurable midi input
 * piano roll, with ability to only display notes in a certain scale like bosca and "chord notes"
 * basic tracker/table like note placing functionality
 * place pattern in a gui like fl studio/dance ejay, should be many shortcuts so there is less drag and drop though
 * node based genereator/effect connection [like](http://jeskola.net/buzz/) [buzz](https://www.youtube.com/watch?v=77zg3fJyaH0)
 * custom piano roll like [hydrogen](https://www.youtube.com/watch?v=EwR1KbX6MZg) with simple pattern generator like fl studio
 * replace minsynth with something more battle tested like https://github.com/FluidSynth/fluidsynth
 * possible weird instruments like gamepads and a arduino+usbhost shield+rockband keytar input thingy (because I have a rockband keytar already)
 * music generation code




