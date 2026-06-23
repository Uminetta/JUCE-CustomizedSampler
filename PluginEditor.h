/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//========================CUSTOM KEYBOARD COLORS================================

class CustomKeyboard : public juce::MidiKeyboardComponent
{
public:
    CustomKeyboard(juce::MidiKeyboardState& state,
        juce::MidiKeyboardComponent::Orientation orientation)
        : juce::MidiKeyboardComponent(state, orientation)
    {
    }

    std::function<bool(int)> isSpecialNote;

protected:
    void drawWhiteNote(int midiNoteNumber,
        juce::Graphics& g,
        juce::Rectangle<float> area,
        bool isDown,
        bool isOver,
        juce::Colour lineColour,
        juce::Colour textColour) override;

    void drawBlackNote(int midiNoteNumber,
                juce::Graphics& g,
                juce::Rectangle<float> area,
                bool isDown,
                bool isOver,
                juce::Colour noteFillColour) override;
};



//==============================================================================
/**
*/
class CSamplerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CSamplerAudioProcessorEditor (CSamplerAudioProcessor&);
    ~CSamplerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    CSamplerAudioProcessor& audioProcessor;
    CustomKeyboard keyboardComponent;
    juce::Image backgroundImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CSamplerAudioProcessorEditor)
};
