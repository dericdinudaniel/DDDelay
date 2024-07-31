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
class DDDelayAudioProcessorEditor : public juce::AudioProcessorEditor,
                                    private juce::AudioProcessorParameter::Listener {
   public:
    DDDelayAudioProcessorEditor(DDDelayAudioProcessor&);
    ~DDDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

   private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DDDelayAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DDDelayAudioProcessorEditor)

    void parameterValueChanged(int, float) override;
    void parameterGestureChanged(int, bool) override {}
    void updateDelayKnobs(bool tempoSyncActive);

    RotaryKnob gainKnob{"Gain", audioProcessor.apvts, gainParamID, true};
    RotaryKnob mixKnob{"Mix", audioProcessor.apvts, mixParamID};
    RotaryKnob delayTimeKnob{"Time", audioProcessor.apvts, delayTimeParamID};
    RotaryKnob feedbackKnob{"Feedback", audioProcessor.apvts, feedbackParamID, true};
    RotaryKnob stereoKnob{"Stereo", audioProcessor.apvts, stereoParamID, true};
    RotaryKnob lowCutKnob{"Low Cut", audioProcessor.apvts, lowCutParamID};
    RotaryKnob highCutKnob{"High Cut", audioProcessor.apvts, highCutParamID};
    RotaryKnob delayNoteKnob{"Note", audioProcessor.apvts, delayNoteParamID};
    juce::TextButton tempoSyncButton;

    juce::AudioProcessorValueTreeState::ButtonAttachment tempoSyncAttachment{
        audioProcessor.apvts,
        tempoSyncParamID.getParamID(),
        tempoSyncButton,
    };

    juce::GroupComponent delayGroup, feedbackGroup, outputGroup;

    MainLookAndFeel mainLF;
};
