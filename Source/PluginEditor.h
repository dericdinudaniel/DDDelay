/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "LookAndFeel.h"
#include "Parameters.h"
#include "PluginProcessor.h"
#include "RotaryKnob.h"

//==============================================================================
/**
 */
class DDDelayAudioProcessorEditor : public juce::AudioProcessorEditor {
   public:
    DDDelayAudioProcessorEditor(DDDelayAudioProcessor&);
    ~DDDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

   private:
    DDDelayAudioProcessor& audioProcessor;

    RotaryKnob gainKnob{"Gain", audioProcessor.apvts, gainParamID, true};
    RotaryKnob mixKnob{"Mix", audioProcessor.apvts, mixParamID};
    RotaryKnob delayTimeKnob{"Time", audioProcessor.apvts, delayTimeParamID};

    juce::GroupComponent delayGroup, feedbackGroup, outputGroup;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DDDelayAudioProcessorEditor)
};
