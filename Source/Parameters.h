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
const juce::ParameterID delayTimeParamID{"delayTime", 1};
const juce::ParameterID mixParamID{"mix", 1};

class Parameters {
   public:
    Parameters(juce::AudioProcessorValueTreeState& apvts);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    static constexpr float minDelayTime = 5.0f;
    static constexpr float maxDelayTime = 5000.0f;

    void prepareToPlay(double sampleRate) noexcept;
    void update() noexcept;
    void reset() noexcept;
    void smoothen() noexcept;

    float gain = 0.0f;
    float delayTime = 0.0f;
    float mix = 0.0f;

   private:
    juce::AudioParameterFloat* gainParam;
    juce::LinearSmoothedValue<float> gainSmoother;

    juce::AudioParameterFloat* delayTimeParam;
    float targetDelayTime = 0.0f;
    float coeff = 0.0f;  // one-pole smoothing coefficient

    juce::AudioParameterFloat* mixParam;
    juce::LinearSmoothedValue<float> mixSmoother;
};
