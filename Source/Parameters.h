/*
  ==============================================================================

    Parameters.h
    Created: 28 Jun 2024 12:09:41am
    Author:  Deric Dinu Daniel

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

const juce::ParameterID gainParamID{"gain", 1};

class Parameters {
   public:
    Parameters(juce::AudioProcessorValueTreeState& apvts);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void update() noexcept;

    float gain = 0.0f;

   private:
    juce::AudioParameterFloat* gainParam;
};
