# Harde Kick Synth

**Extreme kick drum synthesizer for Hardcore, Hardtechno & Hardstyle**

[![GitHub](https://img.shields.io/badge/github-repo-blue)](https://github.com/selimbibberrr-creator/mistral-vst-test)
[![License: GPL-3.0](https://img.shields.io/badge/license-GPL--3.0-green)](https://opensource.org/licenses/GPL-3.0)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)](https://en.cppreference.com/w/cpp/17)
[![JUCE](https://img.shields.io/badge/JUCE-6.1.5+-blue)](https://juce.com/)

---

## 📋 Overview

**Harde Kick Synth** is a **professional VST3 plugin** designed specifically for creating **extreme kick drums** in Hardcore, Hardtechno, and Hardstyle music. It features:

- **5 Sound Sources**: Oscillators, Noise, Sampler, Sub Generator
- **Advanced Filterbank**: Standard + State-Variable filters with flexible routing
- **6 Distortion Algorithms**: Soft Clip, Hard Clip, Chebyshev, Bitcrush, Tape, Foldback
- **Multiband Distortion**: 3-band processing with customizable crossovers
- **Full Effects Chain**: Compressor, Parametric EQ, Reverb, Delay, Stereo Imager
- **Modular Routing**: 16x16 patch matrix for custom signal flow
- **Preset System**: 6 factory presets + user presets with genre/BPM tags
- **Visual Feedback**: Oscilloscope, Spectrum Analyzer, ADSR Editor

---

## 🎯 Features by Genre

| Genre | BPM Range | Kick Characteristics | Recommended Preset |
|-------|-----------|---------------------|-------------------|
| **Hardcore** | 160-200+ | Aggressive, "barking", short | Hardcore Gabber Kick |
| **Hardtechno** | 145-160 | Punchy, deep, "knocking" | Hardtechno Knocking Kick |
| **Hardstyle** | 140-155 | "Bonk", long tail, harmonic rich | Hardstyle Bonk Kick |

---

## 🚀 Quick Start

### Option 1: Using JUCE Projucer (Recommended)

1. **Download JUCE Framework** (v6.1.5+)
   - [Download JUCE](https://juce.com/get-juce/)
   - Extract to a folder (e.g., `/opt/JUCE` or `C:\JUCE`)

2. **Open the Projucer file**
   ```bash
   # Set JUCE_PATH environment variable
   export JUCE_PATH="/path/to/JUCE"  # macOS/Linux
   set JUCE_PATH="C:\Path\To\JUCE"  # Windows
   
   # Open in Projucer
   open HardeKickSynth.jucer  # macOS
   start HardeKickSynth.jucer # Windows
   ```

3. **Configure the project**
   - Set your IDE (Xcode, Visual Studio, etc.)
   - Configure VST3/AU/AAX formats
   - Click "Save Project & Open in IDE"

4. **Build in your IDE**
   - Select **Release** configuration
   - Build the project
   - Plugin will be in the build directory

### Option 2: Using CMake

1. **Set up JUCE path**
   ```bash
   export JUCE_PATH="/path/to/JUCE"
   ```

2. **Configure and build**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   cmake --build . --config Release
   ```

3. **Find the plugin**
   - VST3: `build/Binaries/HardeKickSynth_VST3/`
   - AU: `build/Binaries/HardeKickSynth.component/`

---

## 📁 Project Structure

```
HardeKickSynth/
├── HardeKickSynth.jucer          # JUCE Projucer file
├── CMakeLists.txt                # CMake build configuration
├── DOCS/
│   ├── BUILDING.md               # Build instructions
│   └── USAGE.md                  # User manual
├── Tests/
│   ├── CMakeLists.txt
│   ├── TestHarness.h
│   └── TestHarness.cpp
├── Source/
│   ├── Core/
│   │   ├── AudioEngine.h/.cpp          # Central audio processing
│   │   ├── DSP/                      # DSP optimizations
│   │   │   ├── Oversampling.h/.cpp
│   │   │   ├── SimdUtils.h/.cpp
│   │   │   ├── DCBlocker.h/.cpp
│   │   │   ├── BLIT.h/.cpp
│   │   │   ├── GainStaging.h/.cpp
│   │   │   ├── LatencyCompensator.h/.cpp
│   │   │   └── PerformanceMonitor.h/.cpp
│   │   ├── Oscillators/            # 5 sound sources
│   │   │   ├── Oscillator.h/.cpp
│   │   │   ├── Oscillator1.h/.cpp
│   │   │   ├── Oscillator2.h/.cpp
│   │   │   ├── NoiseRumble.h/.cpp
│   │   │   ├── Sampler.h/.cpp
│   │   │   └── SubGenerator.h/.cpp
│   │   ├── Envelopes/              # ADSR
│   │   │   └── ADSR.h/.cpp
│   │   ├── Mixer/                  # 5-channel mixer
│   │   │   └── Mixer.h/.cpp
│   │   ├── Filters/                # Filterbank
│   │   │   ├── Filter.h/.cpp
│   │   │   ├── StandardFilter.h/.cpp
│   │   │   ├── StateVariableFilter.h/.cpp
│   │   │   └── FilterBank.h/.cpp
│   │   ├── Distortion/             # 6 algorithms
│   │   │   ├── DistortionAlgorithm.h/.cpp
│   │   │   ├── DistortionUnit.h/.cpp
│   │   │   ├── SoftClip.h/.cpp
│   │   │   ├── HardClip.h/.cpp
│   │   │   ├── Chebyshev.h/.cpp
│   │   │   ├── Bitcrush.h/.cpp
│   │   │   ├── Tape.h/.cpp
│   │   │   └── Foldback.h/.cpp
│   │   ├── Effects/                # Effects
│   │   │   ├── Compressor.h/.cpp
│   │   │   ├── ParametricEQ.h/.cpp
│   │   │   ├── Reverb.h/.cpp
│   │   │   ├── Delay.h/.cpp
│   │   │   └── StereoImager.h/.cpp
│   │   ├── Routing/                # Routing
│   │   │   └── RoutingMatrix.h/.cpp
│   │   └── Presets/                # Preset system
│   │       └── PresetManager.h/.cpp
│   └── UI/
│       ├── Components/             # UI components
│       │   ├── Oscilloscope.h/.cpp
│       │   ├── SpectrumAnalyzer.h/.cpp
│       │   └── ADSREditor.h/.cpp
│       ├── MainWindow.h/.cpp       # Main UI
│       └── PluginEditor.h/.cpp     # VST3 interface
├── LICENSE
└── README.md
```

---

## 🎛️ Signal Flow

### Default Routing
```
[OSC1] → [Mixer] → [Filter A] → [Filter B] → [Distortion] → [Compressor] → [EQ] → [Reverb] → [Delay] → [Stereo Imager] → [Output]
[OSC2] → [Mixer]
[Noise] → [Mixer]
[Sampler] → [Mixer]
[Sub] → [Mixer]
```

### Custom Routing
The **Routing Matrix** allows you to:
- Create parallel processing paths
- Set up feedback loops
- Route sidechain inputs
- Customize the signal flow completely

---

## 🎧 Factory Presets

| Preset Name | Genre | BPM | Description |
|-------------|-------|-----|-------------|
| Hardcore Gabber Kick | Hardcore | 180 | Aggressive gabber kick with hard clipping and bitcrush |
| Hardcore Barking Kick | Hardcore | 170 | Barking kick with aggressive distortion chain |
| Hardtechno Knocking Kick | Hardtechno | 150 | Punchy knocking kick with tape saturation |
| Hardtechno Deep Kick | Hardtechno | 145 | Deep punchy kick with multiband saturation |
| Hardstyle Bonk Kick | Hardstyle | 150 | Classic hardstyle bonk kick with pitch envelope |
| Hardstyle Tail Kick | Hardstyle | 140 | Hardstyle kick with long tail and reverb |

---

## 📊 Technical Specifications

| Specification | Value |
|---------------|-------|
| **Sample Rate** | 44.1kHz / 48kHz / 96kHz |
| **Bit Depth** | 32-bit float (internal), 24-bit output |
| **Polyphony** | Mono (kicks are monophonic) |
| **Latency** | <5ms @ 44.1kHz |
| **CPU Usage** | <10% per instance (with 1x oversampling) |
| **Oversampling** | 1x, 2x, or 4x |
| **Plugin Formats** | VST3, AU, AAX |

---

## 🔧 Requirements

### Operating System
- **Windows**: 10/11 (64-bit)
- **macOS**: 10.15+ (Catalina or later)
- **Linux**: Ubuntu 20.04+, Fedora, etc.

### Dependencies
- **JUCE Framework** v6.1.5+ (for VST3/AU/AAX)
- **C++17 Compiler**
  - Windows: Visual Studio 2019/2022
  - macOS: Xcode 12+ with Command Line Tools
  - Linux: GCC 9+ or Clang 10+
- **CMake** v3.15+ (for CMake builds)

---

## 📚 Documentation

- **[BUILDING.md](DOCS/BUILDING.md)** - Complete build instructions
- **[USAGE.md](DOCS/USAGE.md)** - User manual with parameter reference

---

## 🎵 Example Usage

### Hardcore Kick
```
Oscillator 1: Square Wave @ 60Hz, PWM 30%
Oscillator 2: Noise (White), Bandpass @ 800Hz
Filter: LPF 24dB @ 300Hz, Resonance 40%
Distortion: Hard Clip (Drive 80%) + Bitcrush (8-bit, 12kHz)
ADSR: Attack 5ms, Decay 150ms, Sustain 0%, Release 200ms
```

### Hardstyle Bonk Kick
```
Oscillator 1: Sine @ 55Hz (Sub)
Oscillator 2: Wavetable (70% Sine→Square)
Sub Generator: Sine @ 40Hz, Saturation 30%
Filter: Bandpass @ 50Hz, Resonance 60%
Distortion: Multiband (Soft Clip + Chebyshev + Tape)
EQ: Low Shelf +4dB @ 40Hz, Peak -3dB @ 200Hz, Peak +4dB @ 2kHz
ADSR: Attack 0ms, Decay 500ms, Sustain 20%, Release 800ms
```

---

## 🧪 Testing

### Run Unit Tests
```bash
cd build
cmake --build . --target HardeKickSynthTests
./Tests/HardeKickSynthTests
```

### Manual Testing
1. Load the plugin in your DAW
2. Play MIDI notes (C1-C5 recommended)
3. Test all parameters
4. Check CPU usage (<10% per instance)

---

## 🐛 Troubleshooting

### JUCE not found
```
Error: JUCE not found
```
**Solution**: Set the `JUCE_PATH` environment variable to your JUCE installation.

### Compiler errors
```
error: 'JuceHeader.h' file not found
```
**Solution**: Make sure JUCE is properly installed and the path is correct in the Projucer file.

### Plugin not showing in DAW
**Solution**:
- Check the plugin format (VST3/AU/AAX)
- Make sure the plugin is in the correct directory
- Rescan plugins in your DAW

---

## 📜 License

This project is licensed under the **GPL-3.0 License** - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Open a Pull Request

---

## 📬 Contact

For questions, feedback, or support:
- **GitHub Issues**: [https://github.com/selimbibberrr-creator/mistral-vst-test/issues](https://github.com/selimbibberrr-creator/mistral-vst-test/issues)
- **Email**: selim.bibberrr@gmail.com
- **Repository**: [https://github.com/selimbibberrr-creator/mistral-vst-test](https://github.com/selimbibberrr-creator/mistral-vst-test)

---

## 🎉 Acknowledgments

- **JUCE Framework**: [https://juce.com/](https://juce.com/)
- **Native Instruments**: For inspiration on kick synthesis
- **EDMProd**: For DSP techniques
- **Screech House**: For Hardstyle tutorials
- **All Hardcore/Hardtechno/Hardstyle producers**: For the inspiration!

---

**Made with ❤️ for the Hard Dance community** 🎧
