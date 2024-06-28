/*
  ==============================================================================

    Parameters.cpp
    Created: 28 Jun 2024 12:09:41am
    Author:  Deric Dinu Daniel

  ==============================================================================
*/

#include "Parameters.h"

template <typename T>
static void castParameter(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& id, T& dest) {
    dest = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(dest);  // parameter does not exist or wrong type
}

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts) {
    castParameter(apvts, gainParamID, gainParam);
}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        gainParamID,
        "Output Gain",
        juce::NormalisableRange<float>{-12.0f, 12.0f},
        0.0f));
    return layout;
}

void Parameters::update() noexcept {
    gain = juce::Decibels::decibelsToGain(gainParam->get());
}
