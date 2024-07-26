/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "Parameters.h"

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
    juce::Slider slider;
    juce::Label label;

    juce::AudioProcessorValueTreeState::SliderAttachment attachment{
        audioProcessor.apvts,
        gainParamID.getParamID(),
        slider,
    };

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DDDelayAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DDDelayAudioProcessorEditor)
};
