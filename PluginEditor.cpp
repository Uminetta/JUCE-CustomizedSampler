/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CSamplerAudioProcessorEditor::CSamplerAudioProcessorEditor(CSamplerAudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    keyboardComponent(audioProcessor.getKeyboardState(),
        juce::MidiKeyboardComponent::horizontalKeyboard)

{
    // ============================================================================
    // BACKGROUND IMAGE
    //
    // The plugin loads a background image from:
    //
    // Documents/CustomizedSampler/background.png
    //
    // Replace this file with your own image to customize the UI.
    //
    // ============================================================================
    backgroundImage = juce::ImageFileFormat::loadFrom(
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("CSampler")
        .getChildFile("background.png")
    );
    addAndMakeVisible(keyboardComponent);
    setSize(465, 300);
    keyboardComponent.setAvailableRange(36, 84); // C2 = 36, C5 = 72
    keyboardComponent.isSpecialNote = [this](int note)
        {
            return audioProcessor.isMappedNote(note);
        };
}

CSamplerAudioProcessorEditor::~CSamplerAudioProcessorEditor()
{
}

//==============================================================================
void CSamplerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    if (backgroundImage.isValid())
        g.drawImage(backgroundImage, getLocalBounds().toFloat());
}

void CSamplerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    auto keyboardArea = area.removeFromBottom(80);
    keyboardComponent.setBounds(keyboardArea);
}


//==============================================================================
// ============================================================================
// MAPPED NOTE COLOR (WHITE KEYS)
//
// Change this color to customize how mapped sample notes appear.
//
// Example:
//
// juce::Colours::red
// juce::Colours::green
// juce::Colour(255, 100, 0)
//
// ============================================================================
void CustomKeyboard::drawWhiteNote(int midiNoteNumber,
    juce::Graphics& g,
    juce::Rectangle<float> area,
    bool isDown,
    bool isOver,
    juce::Colour lineColour,
    juce::Colour textColour)
{
    juce::Colour c = juce::Colours::white;

    if (isSpecialNote && isSpecialNote(midiNoteNumber))
        c = juce::Colours::mediumpurple;

    if (isDown)
        c = c.darker();
    else if (isOver)
        c = c.brighter();

    g.setColour(c);
    g.fillRect(area);
}
// ============================================================================
// MAPPED NOTE COLOR (BLACK KEYS)
//
// Same as above but for black keys.
//
// ============================================================================
void CustomKeyboard::drawBlackNote(int midiNoteNumber,
    juce::Graphics& g,
    juce::Rectangle<float> area,
    bool isDown,
    bool isOver,
    juce::Colour noteFillColour
    )
{
    juce::Colour c = juce::Colours::black;

    if (isSpecialNote && isSpecialNote(midiNoteNumber))
        c = juce::Colours::blueviolet;

    if (isDown)
        c = c.darker();
    else if (isOver)
        c = c.brighter();

    g.setColour(c);
    g.fillRect(area);
}