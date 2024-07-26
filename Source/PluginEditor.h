/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Parameters.h"
#include "PluginProcessor.h"

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

    juce::Slider slider;
    juce::Label label;

    juce::AudioProcessorValueTreeState::SliderAttachment attachment{
        audioProcessor.apvts,
        gainParamID.getParamID(),
        slider,
    };

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DDDelayAudioProcessorEditor)
};
