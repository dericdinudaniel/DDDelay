/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

#include "PluginEditor.h"
#include "ProtectYourEars.h"

//==============================================================================
DDDelayAudioProcessor::DDDelayAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      params(apvts) {
    lowCutFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    highCutFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
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

    feedbackL = 0.0f;
    feedbackR = 0.0f;

    lowCutFilter.prepare(spec);
    lowCutFilter.reset();

    highCutFilter.prepare(spec);
    highCutFilter.reset();

    lastLowCut = -1.0f;
    lastHighCut = -1.0f;

    tempo.reset();
}

void DDDelayAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

// #ifndef JucePlugin_PreferredChannelConfigurations
bool DDDelayAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    const auto mono = juce::AudioChannelSet::mono();
    const auto stereo = juce::AudioChannelSet::stereo();
    const auto mainIn = layouts.getMainInputChannelSet();
    const auto mainOut = layouts.getMainOutputChannelSet();

    if (mainIn == mono and mainOut == mono) return true;
    if (mainIn == mono and mainOut == stereo) return true;
    if (mainIn == stereo and mainOut == stereo) return true;

    return false;
}
// #endif

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

    params.update();
    tempo.update(getPlayHead());

    float syncedTime = float(tempo.getMillisecondsForNoteLength(params.delayNote));
    if (syncedTime > Parameters::maxDelayTime) {
        syncedTime = Parameters::maxDelayTime;
    }

    float sampleRate = float(getSampleRate());

    auto mainInput = getBusBuffer(buffer, true, 0);
    auto mainInputChannels = mainInput.getNumChannels();
    auto isMainInputStereo = mainInputChannels > 1;
    const float* inputDataL = mainInput.getReadPointer(0);
    const float* inputDataR = mainInput.getReadPointer(isMainInputStereo ? 1 : 0);

    auto mainOutput = getBusBuffer(buffer, false, 0);
    auto mainOutputChannels = mainOutput.getNumChannels();
    auto isMainOutputStereo = mainOutputChannels > 1;
    float* outputDataL = mainOutput.getWritePointer(0);
    float* outputDataR = mainOutput.getWritePointer(isMainOutputStereo ? 1 : 0);

    if (isMainOutputStereo) {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            params.smoothen();

            float delayTime = params.tempoSync ? syncedTime : params.delayTime;
            float delayInSamples = delayTime / 1000.0f * sampleRate;
            delayLine.setDelay(delayInSamples);

            if (params.lowCut != lastLowCut) {
                lowCutFilter.setCutoffFrequency(params.lowCut);
                lastLowCut = params.lowCut;
            }
            if (params.highCut != lastHighCut) {
                highCutFilter.setCutoffFrequency(params.highCut);
                lastHighCut = params.highCut;
            }

            float dryL = inputDataL[sample];
            float dryR = inputDataR[sample];

            float mono = (dryL + dryR) * 0.5f;

            delayLine.pushSample(0, mono * params.panL + feedbackR);
            delayLine.pushSample(1, mono * params.panR + feedbackL);

            float wetL = delayLine.popSample(0);
            float wetR = delayLine.popSample(1);

            feedbackL = wetL * params.feedback;
            feedbackL = lowCutFilter.processSample(0, feedbackL);
            feedbackL = highCutFilter.processSample(0, feedbackL);

            feedbackR = wetR * params.feedback;
            feedbackR = lowCutFilter.processSample(1, feedbackR);
            feedbackR = highCutFilter.processSample(1, feedbackR);

            float mixL = dryL + wetL * params.mix;
            float mixR = dryR + wetR * params.mix;

            outputDataL[sample] = mixL * params.gain;
            outputDataR[sample] = mixR * params.gain;
        }
    } else {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            params.smoothen();

            float delayTime = params.tempoSync ? syncedTime : params.delayTime;
            float delayInSamples = delayTime / 1000.0f * sampleRate;
            delayLine.setDelay(delayInSamples);

            if (params.lowCut != lastLowCut) {
                lowCutFilter.setCutoffFrequency(params.lowCut);
                lastLowCut = params.lowCut;
            }
            if (params.highCut != lastHighCut) {
                highCutFilter.setCutoffFrequency(params.highCut);
                lastHighCut = params.highCut;
            }

            float dry = inputDataL[sample];
            delayLine.pushSample(0, dry + feedbackL);

            float wet = delayLine.popSample(0);

            feedbackL = wet * params.feedback;
            feedbackL = lowCutFilter.processSample(0, feedbackL);
            feedbackL = highCutFilter.processSample(0, feedbackL);

            float mix = dry + wet * params.mix;
            outputDataL[sample] = mix * params.gain;
        }
    }

#if JUCE_DEBUG
    protectYourEars(buffer);
#endif
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
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void DDDelayAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
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
