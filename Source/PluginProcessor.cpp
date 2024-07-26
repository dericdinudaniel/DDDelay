/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

#include "PluginEditor.h"

//==============================================================================
DDDelayAudioProcessor::DDDelayAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      params(apvts) {
}

DDDelayAudioProcessor::~DDDelayAudioProcessor() {
}

//==============================================================================
const juce::String DDDelayAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool DDDelayAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool DDDelayAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool DDDelayAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double DDDelayAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int DDDelayAudioProcessor::getNumPrograms() {
    return 1;  // NB: some hosts don't cope very well if you tell them there are 0 programs,
               // so this should be at least 1, even if you're not really implementing programs.
}

int DDDelayAudioProcessor::getCurrentProgram() {
    return 0;
}

void DDDelayAudioProcessor::setCurrentProgram(int index) {
}

const juce::String DDDelayAudioProcessor::getProgramName(int index) {
    return {};
}

void DDDelayAudioProcessor::changeProgramName(int index, const juce::String& newName) {
}

//==============================================================================
void DDDelayAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    params.prepareToPlay(sampleRate);
    params.reset();

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = juce::uint32(samplesPerBlock);
    spec.numChannels = 2;
    delayLine.prepare(spec);

    double numSamples = Parameters::maxDelayTime / 1000.0 * sampleRate;
    int maxDelayInSamples = int(std::ceil(numSamples));
    delayLine.setMaximumDelayInSamples(maxDelayInSamples);
    delayLine.reset();

    DBG(maxDelayInSamples);
}

void DDDelayAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DDDelayAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

void DDDelayAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    params.update();

    float sampleRate = float(getSampleRate());

    float* channelDataLl = buffer.getWritePointer(0);
    float* channelDataRl = buffer.getWritePointer(1);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        params.smoothen();
        float delayInSamples = params.delayTime / 1000.0f * sampleRate;
        delayLine.setDelay(delayInSamples);

        float dryL = channelDataLl[sample];
        float dryR = channelDataRl[sample];

        delayLine.pushSample(0, dryL);
        delayLine.pushSample(1, dryR);

        float wetL = delayLine.popSample(0);
        float wetR = delayLine.popSample(1);

        float mixL = dryL + wetL * params.mix;
        float mixR = dryR + wetR * params.mix;

        channelDataLl[sample] = mixL * params.gain;
        channelDataRl[sample] = mixR * params.gain;
    }
}

//==============================================================================
bool DDDelayAudioProcessor::hasEditor() const {
    return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DDDelayAudioProcessor::createEditor() {
    return new DDDelayAudioProcessorEditor(*this);
}

//==============================================================================
void DDDelayAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    copyXmlToBinary(*apvts.state.createXml(), destData);
}

void DDDelayAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr and xml->hasTagName(apvts.state.getType())) {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new DDDelayAudioProcessor();
}
