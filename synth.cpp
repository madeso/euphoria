#include <cmath>

const float PI = 3.1415926f;

#include "synth.h"

Synth::Synth()
{
    ResetSample(true);
    volume = 0.5;
    frequency = 440.0; // this is in Hz
    periodSpeed = frequency/44100.0f; // assuming a sample rate of 44.1 kHz
    periodPosition = 0.0;
}

void Synth::ResetSample(bool restart)
{
}


void Synth::SynthSample(int length, float* buffer)
{
    for(int i=0; i<length; ++i)
    {
        periodPosition += periodSpeed;
        if(periodPosition >= 1.0f) // change the "if" to a "while" if you expect frequencies above 44.1 kHz
        {
            periodPosition -= 1.0f;
        }
        float sample = WaveformLookup(periodPosition)*volume;

        if(sample>1.0f) sample=1.0f;
        if(sample<-1.0f) sample=-1.0f;
        buffer[i]=sample;
    }
}

float Synth::WaveformLookup(float periodPosition)
{
    return sin(periodPosition * PI * 2);
}