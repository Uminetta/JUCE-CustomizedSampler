<img width="465" height="299" alt="image" src="https://github.com/user-attachments/assets/a0100a82-10d9-4af6-8f76-2d904702e3b7" />

# CustomizedSampler

A lightweight JUCE-based VST3 sampler plugin designed to be easily customized without requiring advanced C++ knowledge.

The plugin allows users to:

* Map WAV files to specific MIDI notes
* Define a fallback sample for unmapped notes
* Highlight mapped notes on the on-screen keyboard
* Customize keyboard colors
* Replace the plugin background image
* Configure pitch-shifting behavior

---

## Features

* Simple sample-to-note mapping
* Custom background image support
* Visual keyboard with highlighted mapped notes
* Fallback sample for unmapped notes
* JUCE-based VST3 plugin
* Easy source-code customization

---

## Folder Structure

The plugin loads resources from:

Documents/CustomizedSampler/

Example:

Documents/
└── CustomizedSampler
├── background.png
└── Samples
├── DefSound.wav
├── Kick.wav
├── Snare.wav
└── Clap.wav

---

## Custom Sample Mapping

Mappings are defined in:

PluginProcessor.cpp

Locate:

std::vector<SampleMap> samples

Example:

{
{60, "Kick.wav"},
{62, "Snare.wav"},
{64, "Clap.wav"}
};

Format:

{ MIDI_NOTE_NUMBER, "SampleFile.wav" }

Mapped notes are automatically highlighted on the keyboard.

---

## Default Sample

Any MIDI note that is not explicitly mapped uses the fallback sample:

DefSound.wav

This can be changed inside:

PluginProcessor.cpp

Locate:

juce::File defaultFile

and replace the filename with your own sample.

---

## Keyboard Colors

Mapped note colors are defined in:

PluginEditor.cpp

White keys:

juce::Colours::mediumpurple

Black keys:

juce::Colours::blueviolet

Replace with any JUCE color.

---

## Background Image

The plugin background is loaded from:

Documents/CustomizedSampler/background.png

Replace this image with your own PNG file.

---

## Pitch Shifting

Current behavior:

* Each note uses its own root note.
* Samples play at their original pitch.
* No automatic pitch shifting occurs.

To create a traditional sampler:

* Use a single SamplerSound for multiple notes.
* Set a fixed root note (typically MIDI note 60).

Documentation comments in PluginProcessor.cpp describe the required changes.

---

## Building

Requirements:

* JUCE
* Visual Studio 2022 (Windows)
* VST3 SDK

Build target:

* VST3

---

## License

MIT License

Feel free to modify, distribute, and use this project in your own music, games, or audio tools.
