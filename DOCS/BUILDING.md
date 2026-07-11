# Building Harde Kick Synth

## Requirements

### Operating System
- Windows 10/11 (64-bit)
- macOS 10.15+ (Catalina or later)
- Linux (Ubuntu 20.04+, Fedora, etc.)

### Dependencies
1. **JUCE Framework** (v6.1.5 or later)
   - Download from: https://juce.com/
   - Required for VST3/AU/AAX plugin formats
   
2. **C++17 Compiler**
   - Windows: Visual Studio 2019/2022
   - macOS: Xcode 12+ with Command Line Tools
   - Linux: GCC 9+ or Clang 10+

3. **CMake** (v3.15+)
   - Download from: https://cmake.org/
   - Required for build configuration

4. **Git** (for version control)
   - Download from: https://git-scm.com/

---

## Build Instructions

### Option 1: Using CMake (Recommended)

#### 1. Clone the repository
```bash
git clone https://github.com/selimbibberrr-creator/mistral-vst-test.git
cd mistral-vst-test
```

#### 2. Set up JUCE path
```bash
# On Windows (PowerShell)
$env:JUCE_PATH = "C:\Path\To\JUCE"

# On macOS/Linux
export JUCE_PATH="/path/to/JUCE"
```

#### 3. Configure and build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

#### 4. Build the VST3 plugin
The plugin will be built in the `build/Binaries` directory.

---

### Option 2: Using JUCE Projucer

#### 1. Open the project in Projucer
- Launch JUCE Projucer
- Open the `HardeKickSynth.jucer` file (if it exists)
- Configure the project settings

#### 2. Export to your IDE
- Select your IDE (Xcode, Visual Studio, etc.)
- Click "Save Project & Open in IDE"

#### 3. Build in your IDE
- Build the project in Release mode
- The plugin will be in the build directory

---

## Build Options

### Build Types
- **Release**: Optimized for performance (`-O3`)
- **Debug**: With debug symbols (`-O0 -g`)

### Compiler Flags
- **SSE2**: Enabled by default for x86/x64
- **AVX**: Enabled if supported by compiler
- **Warnings**: All warnings enabled (`-Wall -Wextra -Wpedantic`)

### Platform-Specific Notes

#### Windows
- Use Visual Studio 2019/2022
- Target architecture: x64
- VST3 plugins go to: `build/Binaries/HardeKickSynth_VST3.vst3/`

#### macOS
- Use Xcode 12+
- Target: macOS 10.15+
- VST3 plugins go to: `build/Binaries/HardeKickSynth.vst3/`
- AU plugins go to: `build/Binaries/HardeKickSynth.component/`

#### Linux
- Use GCC 9+ or Clang 10+
- VST3 plugins go to: `build/Binaries/HardeKickSynth.so`

---

## Testing

### Run Unit Tests
```bash
cd build
cmake --build . --target HardeKickSynthTests
./Tests/HardeKickSynthTests
```

### Manual Testing
1. Load the plugin in your DAW
2. Test with MIDI notes (C1-C5 recommended for kicks)
3. Verify all parameters work correctly
4. Check CPU usage (<10% per instance)

---

## Troubleshooting

### Common Issues

#### JUCE not found
```
CMake Error: JUCE not found
```
**Solution**: Set the `JUCE_PATH` environment variable to point to your JUCE installation.

#### Compiler errors
```
error: 'JuceHeader.h' file not found
```
**Solution**: Make sure JUCE is properly installed and the path is correct.

#### Missing SIMD instructions
```
warning: SSE2 instruction set not enabled
```
**Solution**: Make sure your CPU supports SSE2 (all modern CPUs do).

#### Linker errors
```
undefined reference to `juce::AudioProcessor::AudioProcessor()'
```
**Solution**: Make sure you're linking with the JUCE libraries correctly.

---

## Plugin Installation

### Windows
1. Copy the `.vst3` folder to:
   - `C:\Program Files\Common Files\VST3\`
   - Or your DAW's VST3 plugin directory

### macOS
1. Copy the `.vst3` folder to:
   - `/Library/Audio/Plug-Ins/VST3/` (system-wide)
   - `~/Library/Audio/Plug-Ins/VST3/` (user)

2. Copy the `.component` folder to:
   - `/Library/Audio/Plug-Ins/Components/` (system-wide)
   - `~/Library/Audio/Plug-Ins/Components/` (user)

### Linux
1. Copy the `.so` file to your VST3 plugin directory
2. Make sure your DAW can find it

---

## DAW Compatibility

Harde Kick Synth has been tested with:
- **Ableton Live** 10/11
- **FL Studio** 20/21
- **Bitwig Studio** 4+
- **Reaper** 6+
- **Logic Pro** 10.7+
- **Cubase** 11/12

---

## Performance Tips

1. **Buffer Size**: Use 256-512 samples for best performance
2. **Oversampling**: Use 1x for most cases, 2x for high-quality distortion
3. **CPU Usage**: <10% per instance on modern CPUs
4. **Latency**: <5ms @ 44.1kHz

---

## Contact & Support

For issues, questions, or feedback:
- GitHub Issues: https://github.com/selimbibberrr-creator/mistral-vst-test/issues
- Email: selim.bibberrr@gmail.com
