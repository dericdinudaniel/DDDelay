/*
  ==============================================================================

    RotaryKnob.cpp
    Created: 26 Jul 2024 3:50:10pm
    Author:  Deric Dinu Daniel

  ==============================================================================
*/
#include "RotaryKnob.h"

#include <JuceHeader.h>

#include "LookAndFeel.h"

//==============================================================================
RotaryKnob::RotaryKnob(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID, bool drawFromMiddle)
    : attachment(apvts, parameterID.getParamID(), slider) {
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 16);
    slider.setBounds(0, 0, 70, 86);
    addAndMakeVisible(slider);

    label.setText(text, juce::NotificationType::dontSendNotification);
    label.setJustificationType(juce::Justification::horizontallyCentred);
    label.setBorderSize(juce::BorderSize<int>{0, 0, 2, 0});
    label.attachToComponent(&slider, false);
    addAndMakeVisible(label);

    float pi = juce::MathConstants<float>::pi;
    slider.setRotaryParameters(1.25 * pi, 2.75 * pi, true);
    slider.setLookAndFeel(RotaryKnobLookAndFeel::get());
    setSize(70, 110);

    slider.getProperties().set("drawFromMiddle", drawFromMiddle);
}

RotaryKnob::~RotaryKnob() {
}

void RotaryKnob::resized() {
    slider.setTopLeftPosition(0, 24);
}
