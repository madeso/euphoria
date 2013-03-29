class Synth
{
public:
    Synth();

    void ResetSample(bool restart);
    void SynthSample(int length, float* buffer);
    float WaveformLookup(float periodPosition);
private:
    float frequency;
    float volume;
    float periodSpeed;
    float periodPosition;
};