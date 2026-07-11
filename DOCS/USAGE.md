# Harde Kick Synth Usage Guide

## Quick Start

### Loading the Plugin
1. Open your DAW
2. Create a new MIDI track
3. Load **Harde Kick Synth** as an instrument plugin
4. Play MIDI notes (C1-C5 recommended for kicks)

---

## Interface Overview

### Main Sections

#### 1. **Oscillators Section**
- **Oscillator 1**: Sine/Square/Saw/Triangle + PWM + Detune
- **Oscillator 2**: Wavetable/FM/Noise
- **Noise Rumble**: Filtered noise for tails
- **Sampler**: Load your own kick samples
- **Sub Generator**: Pure sine for sub frequencies

#### 2. **Filters Section**
- **Filter A**: Standard filter (LP/HP/BP/Notch, 12/24/48dB)
- **Filter B**: State-variable filter with morphing
- **Routing**: Series/Parallel/Mid-Side/Feedback

#### 3. **Distortion Section**
- **Algorithm**: Soft Clip, Hard Clip, Chebyshev, Bitcrush, Tape, Foldback
- **Drive**: Amount of distortion
- **Multiband**: Enable 3-band distortion
- **Oversampling**: 1x/2x/4x for quality

#### 4. **Effects Section**
- **Compressor**: VCA/FET/Opto with sidechain
- **EQ**: 6-band parametric equalizer
- **Reverb**: Hall/Room/Spring for tails
- **Delay**: Syncable delay
- **Stereo Imager**: Mid/Side width control

#### 5. **Envelope Section**
- **ADSR Editor**: Visual envelope editing
- **Attack/Decay/Sustain/Release**: Per oscillator

#### 6. **Visual Feedback**
- **Oscilloscope**: Waveform visualization
- **Spectrum Analyzer**: Frequency analysis

---

## Presets

### Factory Presets

#### Hardcore (160-200 BPM)
1. **Hardcore Gabber Kick**
   - Square wave @ 60Hz
   - Filtered noise for click
   - Hard clipping + bitcrush
   - LPF @ 300Hz, 24dB
   - **Best for**: Gabber, Early Hardcore

2. **Hardcore Barking Kick**
   - Sine sub @ 50Hz
   - Saw wave for harmonics
   - Multiband distortion
   - **Best for**: Modern Hardcore, Barking kicks

#### Hardtechno (145-160 BPM)
3. **Hardtechno Knocking Kick**
   - Sine sub @ 50Hz
   - FM synthesis for harmonics
   - 909 sample layer
   - Tape saturation
   - **Best for**: Punchy, knocking kicks

4. **Hardtechno Deep Kick**
   - Sine fundamental @ 55Hz
   - Sub generator @ 30Hz
   - Multiband saturation
   - **Best for**: Deep, powerful kicks

#### Hardstyle (140-155 BPM)
5. **Hardstyle Bonk Kick**
   - Sine sub @ 55Hz
   - Wavetable morphing (70% sine→square)
   - Multiband distortion (Soft Clip + Chebyshev + Tape)
   - EQ: Boost 40Hz, Cut 200Hz, Boost 2kHz
   - **Best for**: Classic hardstyle bonk

6. **Hardstyle Tail Kick**
   - Sine @ 60Hz
   - Noise rumble with long decay
   - Soft clipping
   - Reverb for tail
   - **Best for**: Kicks with long tails

---

## Parameter Reference

### Oscillator 1
| Parameter | Range | Description |
|-----------|-------|-------------|
| Waveform | Sine/Square/Saw/Triangle | Oscillator waveform |
| Frequency | 20-2000Hz | Base frequency |
| Pulse Width | 0-100% | PWM duty cycle (for square wave) |
| Detune | -50 to +50 cents | Pitch detuning |
| Phase | 0-360° | Phase offset |
| Level | 0-100% | Output level |
| Pan | L-R | Stereo panning |

### Oscillator 2
| Parameter | Range | Description |
|-----------|-------|-------------|
| Waveform | Wavetable/FM/Noise | Oscillator type |
| Wavetable Position | 0-100% | Morph position |
| FM Index | 0-10 | FM modulation index |
| Modulator Freq | 20-2000Hz | FM modulator frequency |
| Noise Color | White/Pink/Brown | Noise type |
| Level | 0-100% | Output level |
| Pan | L-R | Stereo panning |

### Noise Rumble
| Parameter | Range | Description |
|-----------|-------|-------------|
| Filter Type | LP/HP/BP/Notch | Filter type |
| Cutoff | 20-20000Hz | Filter cutoff frequency |
| Resonance | 0-100% | Filter resonance |
| Decay | 0-5000ms | Decay time for tail |
| Level | 0-100% | Output level |
| Pan | L-R | Stereo panning |

### Sampler
| Parameter | Range | Description |
|-----------|-------|-------------|
| Sample | - | Load WAV file |
| Pitch | -24 to +24 st | Pitch shift in semitones |
| Start | 0-100% | Start point in sample |
| End | 0-100% | End point in sample |
| Reverse | On/Off | Reverse playback |
| Time Stretch | 50-200% | Time stretching |
| Level | 0-100% | Output level |
| Pan | L-R | Stereo panning |

### Sub Generator
| Parameter | Range | Description |
|-----------|-------|-------------|
| Frequency | 20-100Hz | Sub frequency |
| Saturation | 0-100% | Soft clipping amount |
| Phase | 0-360° | Phase offset |
| Level | 0-100% | Output level |
| Pan | L-R | Stereo panning |

---

## Filter Parameters

### Standard Filter (Filter A)
| Parameter | Range | Description |
|-----------|-------|-------------|
| Type | LP/HP/BP/Notch | Filter type |
| Cutoff | 20-20000Hz | Cutoff frequency |
| Resonance | 0-100% | Resonance amount |
| Slope | 12/24/48dB | Filter slope |
| Key Tracking | 0-100% | Key tracking amount |

### State Variable Filter (Filter B)
| Parameter | Range | Description |
|-----------|-------|-------------|
| Type | LP/BP/HP/Notch | Filter type |
| Morph | 0-100% | Morph between types |
| Cutoff | 20-20000Hz | Cutoff frequency |
| Resonance | 0-100% | Resonance amount |
| Drive | 0-100% | Nonlinear drive |

### Filter Bank Routing
| Mode | Description |
|------|-------------|
| Series | Filter A → Filter B |
| Parallel | Filter A + Filter B (mixable) |
| Mid/Side | Filter A (Mid), Filter B (Side) |
| Feedback | Filter B → Filter A (extreme resonance) |

---

## Distortion Parameters

### Main Distortion Unit
| Parameter | Range | Description |
|-----------|-------|-------------|
| Algorithm | Soft Clip/Hard Clip/Chebyshev/Bitcrush/Tape/Foldback | Distortion type |
| Drive | 0-100% | Distortion amount |
| Output | -24 to +12dB | Output gain |
| Mix | 0-100% | Dry/Wet mix |
| Oversampling | 1x/2x/4x | Quality setting |

### Multiband Distortion
| Parameter | Range | Description |
|-----------|-------|-------------|
| Enable | On/Off | Enable multiband mode |
| Low/Mid Crossover | 20-2000Hz | Crossover frequency |
| Mid/High Crossover | 200-20000Hz | Crossover frequency |
| Low Algorithm | - | Distortion for low band |
| Mid Algorithm | - | Distortion for mid band |
| High Algorithm | - | Distortion for high band |
| Low/Mid/High Drive | 0-100% | Drive per band |

---

## Effects Parameters

### Compressor
| Parameter | Range | Description |
|-----------|-------|-------------|
| Type | VCA/FET/Opto | Compressor type |
| Threshold | -60 to 0dB | Compression threshold |
| Ratio | 1:1 to ∞:1 | Compression ratio |
| Attack | 0-500ms | Attack time |
| Release | 0-2000ms | Release time |
| Makeup Gain | -24 to +24dB | Output gain |
| Sidechain | On/Off | Enable sidechain |

### Parametric EQ
| Parameter | Range | Description |
|-----------|-------|-------------|
| Band Type | Bell/Notch/LowShelf/HighShelf/LP/HP | EQ band type |
| Frequency | 20-20000Hz | Center frequency |
| Q | 0.1-10 | Bandwidth |
| Gain | -24 to +24dB | Gain/attenuation |
| Enable | On/Off | Enable band |

### Reverb
| Parameter | Range | Description |
|-----------|-------|-------------|
| Type | Hall/Room/Spring | Reverb type |
| Decay | 0.1-10s | Decay time |
| Pre-Delay | 0-500ms | Pre-delay time |
| Size | 0-100% | Room size |
| Damping | 0-100% | High frequency damping |
| Mix | 0-100% | Dry/Wet mix |

### Delay
| Parameter | Range | Description |
|-----------|-------|-------------|
| Time | 0-5000ms or 1/4-16 beats | Delay time |
| Sync | ms/BPM | Time mode |
| Feedback | 0-100% | Feedback amount |
| Filter | 0-100% | High-cut filter |
| BPM | 20-300 | BPM for sync mode |

### Stereo Imager
| Parameter | Range | Description |
|-----------|-------|-------------|
| Width | 0-100% | Stereo width |
| Focus | 0-100% | Mid/Side focus |
| Phase Correction | On/Off | Enable phase correction |

---

## ADSR Envelope

Each sound source has its own ADSR envelope:

| Parameter | Range | Description |
|-----------|-------|-------------|
| Attack | 0-2000ms | Attack time |
| Decay | 0-2000ms | Decay time |
| Sustain | 0-100% | Sustain level |
| Release | 0-2000ms | Release time |

### ADSR Editor
- **Drag points** to adjust envelope shape
- **A**: Attack point
- **D**: Decay point
- **S**: Sustain point
- **R**: Release point

---

## Routing Matrix

The routing matrix allows you to connect modules in any order:

### Default Routing
```
[OSC1] → [Mixer] → [Filter A] → [Filter B] → [Distortion] → [Compressor] → [EQ] → [Reverb] → [Delay] → [Stereo Imager] → [Output]
[OSC2] → [Mixer]
[Noise] → [Mixer]
[Sampler] → [Mixer]
[Sub] → [Mixer]
```

### Custom Routing
You can create custom routings like:
- Parallel distortion paths
- Feedback loops
- Sidechain routing
- Pre/post effect routing

---

## Tips & Tricks

### For Hardcore Kicks
1. Use **Square or Saw wave** for Oscillator 1
2. Add **Noise Rumble** with bandpass filter @ 800Hz
3. Use **Hard Clip + Bitcrush** for distortion
4. Set **LPF @ 300Hz, 24dB** for punch
5. Use **fast ADSR** (Attack: 0-10ms, Decay: 50-200ms)

### For Hardtechno Kicks
1. Use **Sine wave** for sub (40-60Hz)
2. Add **FM synthesis** for harmonics
3. Use **Tape saturation** for warmth
4. Set **LPF @ 100-300Hz, 12dB**
5. Use **medium ADSR** (Attack: 0-5ms, Decay: 100-500ms)

### For Hardstyle Kicks
1. Use **Sine wave** for sub (50-60Hz)
2. Add **Wavetable** (Sine→Square) for mid
3. Use **Multiband distortion** (Soft Clip + Chebyshev)
4. Set **BPF @ 50-100Hz** for peak
5. Use **long ADSR** (Attack: 0-20ms, Decay: 200-800ms, Release: 300-1000ms)

---

## MIDI Control

### Note Range
- **C1 (32.7Hz)**: Deep sub
- **C2 (65.4Hz)**: Standard kick fundamental
- **C3 (130.8Hz)**: Higher kick
- **C4 (261.6Hz)**: Click/attack

### Velocity
- Higher velocity = louder kick
- Use velocity for dynamic expression

### Pitch Bend
- Affects all oscillators
- Use for pitch modulation effects

---

## CPU Optimization

### Tips to Reduce CPU Usage
1. **Reduce Oversampling**: Use 1x or 2x instead of 4x
2. **Disable Unused Modules**: Turn off oscillators/effects you're not using
3. **Use Smaller Buffer Sizes**: 256-512 samples for best performance
4. **Limit Multiband Distortion**: Use single-band when possible
5. **Reduce Filter Slope**: 12dB instead of 48dB when possible

### Expected CPU Usage
| Configuration | CPU Usage |
|---------------|------------|
| Default (1x oversampling) | <5% |
| High Quality (2x oversampling) | <10% |
| Maximum Quality (4x oversampling) | <15% |

---

## Troubleshooting

### No Sound
- Check MIDI input
- Verify note range (C1-C5)
- Check volume levels
- Make sure oscillators are enabled

### Distortion Sounds Bad
- Reduce Drive parameter
- Try different algorithm
- Check for clipping (use oscilloscope)

### High CPU Usage
- Reduce oversampling
- Disable unused modules
- Use smaller buffer size

### Clicks and Pops
- Increase attack time slightly
- Check for DC offset
- Use DC blocking filter

---

## Preset Management

### Saving Presets
1. Adjust parameters to your liking
2. Click **"Save Preset"** button
3. Enter a name for your preset
4. Preset is saved with current settings

### Loading Presets
1. Click **"Load Preset"** button
2. Select a preset from the menu
3. Preset is loaded with all settings

### Deleting Presets
1. Select a user preset
2. Click **"Delete Preset"** button
3. Confirm deletion

---

## Version History

### v1.0.0 (Current)
- Initial release
- All features implemented
- 6 factory presets included

---

## Support

For help, questions, or feedback:
- GitHub Issues: https://github.com/selimbibberrr-creator/mistral-vst-test/issues
- Email: selim.bibberrr@gmail.com
