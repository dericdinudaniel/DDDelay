/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 27 Jul 2024 11:24:29am
    Author:  Deric Dinu Daniel

  ==============================================================================
*/

#include "LookAndFeel.h"

// const juce::Typeface::Ptr Fonts::typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::LatoMedium_ttf, BinaryData::LatoMedium_ttfSize);
const juce::Typeface::Ptr Fonts::typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::JetBrainsMonoRegular_ttf, BinaryData::JetBrainsMonoRegular_ttfSize);

juce::Font Fonts::getFont(float height) {
    return juce::FontOptions(typeface).withHeight(height);
}

RotaryKnobLookAndFeel::RotaryKnobLookAndFeel() {
    setColour(juce::Label::textColourId, Colors::Knob::label);
    setColour(juce::Slider::textBoxTextColourId, Colors::Knob::label);
    setColour(juce::Slider::rotarySliderFillColourId, Colors::Knob::trackActive);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::CaretComponent::caretColourId, Colors::Knob::caret);
}

void RotaryKnobLookAndFeel::drawRotarySlider(
    juce::Graphics& g,
    int x, int y, int width, [[maybe_unused]] int height,
    float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle,
    juce::Slider& slider) {
    auto bounds = juce::Rectangle<int>(x, y, width, width).toFloat();
    auto knobRect = bounds.reduced(10.0f, 10.0f);

    auto path = juce::Path();
    path.addEllipse(knobRect);
    dropShadow.drawForPath(g, path);

    g.setColour(Colors::Knob::outline);
    g.fillEllipse(knobRect);

    auto innerRect = knobRect.reduced(2.0f, 2.0f);
    auto gradient = juce::ColourGradient(
        Colors::Knob::gradientTop, 0.0f, innerRect.getY(),
        Colors::Knob::gradientBottom, 0.0f, innerRect.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillEllipse(innerRect);

    auto center = bounds.getCentre();
    auto radius = bounds.getWidth() / 2.0f;
    auto lineWidth = 3.0f;
    auto arcRadius = radius - lineWidth / 2.0f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(center.x,
                                center.y,
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                rotaryEndAngle,
                                true);

    auto strokeType = juce::PathStrokeType(
        lineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    g.setColour(Colors::Knob::trackBackground);
    g.strokePath(backgroundArc, strokeType);

    auto dialRadius = innerRect.getHeight() / 2.0f - lineWidth;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    juce::Point<float> dialStart(center.x + 10.0f * std::sin(toAngle),
                                 center.y - 10.0f * std::cos(toAngle));
    juce::Point<float> dialEnd(center.x + dialRadius * std::sin(toAngle),
                               center.y - dialRadius * std::cos(toAngle));

    juce::Path dialPath;
    dialPath.startNewSubPath(dialStart);
    dialPath.lineTo(dialEnd);
    g.setColour(Colors::Knob::dial);
    g.strokePath(dialPath, strokeType);

    if (slider.isEnabled()) {
        float fromAngle = rotaryStartAngle;
        if (slider.getProperties()["drawFromMiddle"]) {
            fromAngle += (rotaryEndAngle - rotaryStartAngle) / 2.0f;
        }

        juce::Path valueArc;
        valueArc.addCentredArc(center.x,
                               center.y,
                               arcRadius,
                               arcRadius,
                               0.0f,
                               fromAngle,
                               toAngle,
                               true);

        g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
        g.strokePath(valueArc, strokeType);
    }
}

juce::Font RotaryKnobLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label) {
    return Fonts::getFont();
}

class RotaryKnobLabel : public juce::Label {
   public:
    RotaryKnobLabel() : juce::Label() {}

    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails&) override {}

    std::unique_ptr<juce::AccessibilityHandler> createAccessibilityHandler() override {
        return createIgnoredAccessibilityHandler(*this);
    }

    juce::TextEditor* createEditorComponent() override {
        auto* ed = new juce::TextEditor(getName());
        ed->applyFontToAllText(getLookAndFeel().getLabelFont(*this));
        copyAllExplicitColoursTo(*ed);

        ed->setBorder(juce::BorderSize<int>());
        ed->setIndents(2, 1);
        ed->setJustification(juce::Justification::centredTop);
        ed->setPopupMenuEnabled(false);
        ed->setInputRestrictions(8);
        return ed;
    }
};

juce::Label* RotaryKnobLookAndFeel::createSliderTextBox(juce::Slider& slider) {
    auto l = new RotaryKnobLabel();
    l->setJustificationType(juce::Justification::centred);
    l->setKeyboardType(juce::TextInputTarget::decimalKeyboard);
    l->setColour(juce::Label::textColourId, slider.findColour(juce::Slider::textBoxTextColourId));
    l->setColour(juce::TextEditor::textColourId, Colors::Knob::value);
    l->setColour(juce::TextEditor::highlightedTextColourId, Colors::Knob::value);
    l->setColour(juce::TextEditor::highlightColourId, slider.findColour(juce::Slider::rotarySliderFillColourId));
    l->setColour(juce::TextEditor::backgroundColourId, Colors::Knob::textBoxBackground);
    return l;
}

void RotaryKnobLookAndFeel::fillTextEditorBackground(
    juce::Graphics& g, [[maybe_unused]] int width, [[maybe_unused]] int height,
    juce::TextEditor& textEditor) {
    g.setColour(Colors::Knob::textBoxBackground);
    g.fillRoundedRectangle(textEditor.getLocalBounds().reduced(4, 0).toFloat(), 4.0f);
}

MainLookAndFeel::MainLookAndFeel() {
    setColour(juce::GroupComponent::textColourId, Colors::Group::label);
    setColour(juce::GroupComponent::outlineColourId, Colors::Group::outline);
}

juce::Font MainLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label) {
    return Fonts::getFont();
}

// copied from drawGroupComponentOutline in juce_LookAndFeel_V2, just changed the font
void MainLookAndFeel::drawGroupComponentOutline(juce::Graphics& g, int width, int height,
                                                const juce::String& text, const juce::Justification& position,
                                                juce::GroupComponent& group) {
    const float textH = 16.0f;
    const float indent = 3.0f;
    const float textEdgeGap = 4.0f;
    auto cs = 5.0f;

    // juce::Font f(withDefaultMetrics(juce::FontOptions{textH}));
    juce::Font f(Fonts::getFont(textH));

    juce::Path p;
    auto x = indent;
    auto y = f.getAscent() - 3.0f;
    auto w = juce::jmax(0.0f, (float)width - x * 2.0f);
    auto h = juce::jmax(0.0f, (float)height - y - indent);
    cs = juce::jmin(cs, w * 0.5f, h * 0.5f);
    auto cs2 = 2.0f * cs;

    auto textW = text.isEmpty() ? 0
                                : juce::jlimit(0.0f,
                                               juce::jmax(0.0f, w - cs2 - textEdgeGap * 2),
                                               (float)f.getStringWidth(text) + textEdgeGap * 2.0f);
    auto textX = cs + textEdgeGap;

    if (position.testFlags(juce::Justification::horizontallyCentred))
        textX = cs + (w - cs2 - textW) * 0.5f;
    else if (position.testFlags(juce::Justification::right))
        textX = w - cs - textW - textEdgeGap;

    p.startNewSubPath(x + textX + textW, y);
    p.lineTo(x + w - cs, y);

    p.addArc(x + w - cs2, y, cs2, cs2, 0, juce::MathConstants<float>::halfPi);
    p.lineTo(x + w, y + h - cs);

    p.addArc(x + w - cs2, y + h - cs2, cs2, cs2, juce::MathConstants<float>::halfPi, juce::MathConstants<float>::pi);
    p.lineTo(x + cs, y + h);

    p.addArc(x, y + h - cs2, cs2, cs2, juce::MathConstants<float>::pi, juce::MathConstants<float>::pi * 1.5f);
    p.lineTo(x, y + cs);

    p.addArc(x, y, cs2, cs2, juce::MathConstants<float>::pi * 1.5f, juce::MathConstants<float>::twoPi);
    p.lineTo(x + textX, y);

    auto alpha = group.isEnabled() ? 1.0f : 0.5f;

    g.setColour(group.findColour(juce::GroupComponent::outlineColourId)
                    .withMultipliedAlpha(alpha));

    g.strokePath(p, juce::PathStrokeType(2.0f));

    g.setColour(group.findColour(juce::GroupComponent::textColourId)
                    .withMultipliedAlpha(alpha));
    g.setFont(f);
    g.drawText(text,
               juce::roundToInt(x + textX), 0,
               juce::roundToInt(textW),
               juce::roundToInt(textH),
               juce::Justification::centred, true);
}

ButtonLookAndFeel::ButtonLookAndFeel() {
    setColour(juce::TextButton::textColourOffId, Colors::Button::text);
    setColour(juce::TextButton::textColourOnId, Colors::Button::textToggled);
    setColour(juce::TextButton::buttonColourId, Colors::Button::background);
    setColour(juce::TextButton::buttonOnColourId, Colors::Button::backgroundToggled);
}

void ButtonLookAndFeel::drawButtonBackground(juce::Graphics& g,
                                             juce::Button& button,
                                             const juce::Colour& backgroundColour,
                                             [[maybe_unused]] bool shouldDrawButtonAsHighlighted,
                                             bool shouldDrawButtonAsDown) {
    auto bounds = button.getLocalBounds().toFloat();
    auto cornerSize = bounds.getHeight() * 0.25f;
    auto buttonRect = bounds.reduced(1.0f, 1.0f).withTrimmedBottom(1.0f);

    if (shouldDrawButtonAsDown) {
        buttonRect.translate(0.0f, 1.0f);
    }

    g.setColour(backgroundColour);
    g.fillRoundedRectangle(buttonRect, cornerSize);

    g.setColour(Colors::Button::outline);
    g.drawRoundedRectangle(buttonRect, cornerSize, 2.0f);
}

void ButtonLookAndFeel::drawButtonText(juce::Graphics& g,
                                       juce::TextButton& button,
                                       [[maybe_unused]] bool shouldDrawButtonAsHighlighted,
                                       bool shouldDrawButtonAsDown) {
    auto bounds = button.getLocalBounds().toFloat();
    auto buttonRect = bounds.reduced(1.0f, 1.0f).withTrimmedBottom(1.0f);

    if (shouldDrawButtonAsDown) {
        buttonRect.translate(0.0f, 1.0f);
    }

    if (button.getToggleState()) {
        g.setColour(button.findColour(juce::TextButton::textColourOnId));
    } else {
        g.setColour(button.findColour(juce::TextButton::textColourOffId));
    }

    g.setFont(Fonts::getFont(15.0f));
    g.drawText(button.getButtonText(), buttonRect, juce::Justification::centred);
}
