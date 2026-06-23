/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CSamplerAudioProcessor::CSamplerAudioProcessor()
    #ifndef JucePlugin_PreferredChannelConfigurations
        : AudioProcessor(BusesProperties()
    #if ! JucePlugin_IsMidiEffect
    #if ! JucePlugin_IsSynth
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
    #endif
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    #endif
        )
    #endif
{
    for (int i = 0; i < 16; ++i)
        synth.addVoice(new juce::SamplerVoice());
    formatManager.registerBasicFormats();
    loadSamples();
    setPlayConfigDetails(2, 2, 44100.0, 512);
    setPlayHead(nullptr);
    setLatencySamples(0);
}

CSamplerAudioProcessor::~CSamplerAudioProcessor()
{
}


//======================================MY FUNCTIONS============================
// ============================================================================
// SAMPLE DIRECTORY
//
// All samples are loaded from:
//
// Documents/CustomizedSampler/Samples
//
// Folder structure:
//
// Documents
// └── CustomizedSampler
//     ├── background.png
//     └── Samples
//         ├── Default.wav
//         ├── Sample1.wav
//         ├── Sample2.wav
//         └── ...
//
// ============================================================================
juce::File getSamplesFolder()
{
    return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("CSampler")
        .getChildFile("Samples");
}

void CSamplerAudioProcessor::loadSamples()
{
    formatManager.registerBasicFormats();

    // -------------------------------
    // 1️⃣ Define mapped samples
    // -------------------------------
    struct SampleMap
    {
        int midiNote;
        juce::String fileName;
    };

    // ============================================================================
    // CUSTOM SAMPLE MAPPING
    //
    // Format:
    //
    // { MIDI_NOTE_NUMBER, "SampleFile.wav" }
    //
    // Example:
    //
    // {60, "Kick.wav"}   -> Middle C
    // {62, "Snare.wav"}  -> D
    //
    // Add or remove mappings as needed.
    //
    // Mapped notes are highlighted on the keyboard.
    //
    // ============================================================================
    std::vector<SampleMap> samples =
    {
        {60, "Kick.wav"},
        {62, "Snare.wav"},
        {64, "Clap.wav"},
        {65, "HiHat.wav"},
        {67, "Crash.wav"}
    };

    // ============================================================================
    // DEFAULT SAMPLE
    //
    // Any MIDI note not listed in the mapping table above will use this sample.
    //
    // By default:
    //
    // DefSound.wav
    //
    // Replace with any WAV file.
    //
    // ============================================================================
    juce::File defaultFile = getSamplesFolder()
        .getChildFile("DefSound.wav");

    auto defaultReader = std::unique_ptr<juce::AudioFormatReader>(
        formatManager.createReaderFor(defaultFile));

    // -------------------------------
    // 3️⃣ Track which notes are NOT mapped
    // -------------------------------
    juce::BigInteger remainingNotes;
    remainingNotes.setRange(0, 128, true); // start with ALL notes

    // ============================================================================
    // PITCH SHIFTING BEHAVIOR
    //
    // By default, this sampler does NOT perform pitch shifting.
    //
    // Mapped Samples:
    //     rootNote = s.midiNote
    //
    // Default Sample:
    //     rootNote = note
    //
    // As a result, every sample plays at its original pitch regardless of
    // which MIDI note triggers it.
    //
    // Example:
    //     A sample mapped to C3 will only play when C3 is pressed.
    //     It will not be transposed to neighboring notes.
    //
    // ---------------------------------------------------------------------------
    // ENABLE TRADITIONAL SAMPLER PITCH SHIFTING
    // ---------------------------------------------------------------------------
    //
    // To make a sample playable across multiple notes with automatic
    // pitch transposition:
    //
    // 1. Create a single SamplerSound that covers a range of notes:
    //
    //        notes.setRange(0, 128, true);
    //
    // 2. Use a fixed root note, for example:
    //
    //        rootNote = 60; // Middle C
    //
    // JUCE will then automatically pitch-shift the sample based on the
    // distance between the played note and the root note.
    //
    // ============================================================================
    for (auto& s : samples)
    {
        mappedNotes.insert(s.midiNote);

        juce::File file = getSamplesFolder()
            .getChildFile(s.fileName);

        auto reader = std::unique_ptr<juce::AudioFormatReader>(
            formatManager.createReaderFor(file));

        if (reader != nullptr)
        {
            juce::BigInteger notes;
            notes.setBit(s.midiNote);

            remainingNotes.clearBit(s.midiNote); // remove from default pool

            synth.addSound(new juce::SamplerSound(
                s.fileName,
                *reader,
                notes,
                s.midiNote, // root = same → no pitch shift
                0.01,
                0.1,
                10.0
            ));
        }
        else
        {
            DBG("Failed to load mapped sample: " << s.fileName);
        }
    }

    // -------------------------------
    // 5️⃣ Add default sample per note (NO pitch shifting)
    // -------------------------------
    if (defaultReader != nullptr)
    {
        for (int note = 0; note < 128; ++note)
        {
            if (remainingNotes[note])
            {
                juce::BigInteger singleNote;
                singleNote.setBit(note);

                synth.addSound(new juce::SamplerSound(
                    "Default_" + juce::String(note),
                    *defaultReader,
                    singleNote,
                    note, // 🎯 root = note → no pitch shift
                    0.01,
                    0.1,
                    10.0
                ));
            }
        }
    }
    else
    {
        DBG("Failed to load default sample: MetalPipe.wav");
    }
}

bool CSamplerAudioProcessor::isMappedNote(int midiNote) const
{
    return mappedNotes.find(midiNote) != mappedNotes.end();
}

//==============================================================================
const juce::String CSamplerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CSamplerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CSamplerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CSamplerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CSamplerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CSamplerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CSamplerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CSamplerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CSamplerAudioProcessor::getProgramName (int index)
{
    return {};
}

void CSamplerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CSamplerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void CSamplerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CSamplerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif



void CSamplerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    buffer.clear();

    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}


//==============================================================================
bool CSamplerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CSamplerAudioProcessor::createEditor()
{
    return new CSamplerAudioProcessorEditor (*this);
}

//==============================================================================
void CSamplerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CSamplerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CSamplerAudioProcessor();
}
