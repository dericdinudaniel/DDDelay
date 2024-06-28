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

    void prepareToPlay(double sampleRate) noexcept;
    void update() noexcept;
    void reset() noexcept;
    void smoothen() noexcept;

    float gain = 0.0f;

   private:
    juce::AudioParameterFloat* gainParam;
    juce::LinearSmoothedValue<float> gainSmoother;
};
