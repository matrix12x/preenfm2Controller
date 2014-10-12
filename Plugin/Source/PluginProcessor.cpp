/*
 * Copyright 2014 Xavier Hosxe
 *
 * Author: Xavier Hosxe (xavier <dot> hosxe
 *                      (at) g m a i l <dot> com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "MidifiedParameter.h"
#include "UI/PreenLookAndFeel.h"

int Midificator::paramIndexCounter = 0;

//==============================================================================
Pfm2AudioProcessor::Pfm2AudioProcessor()
{

    myLookAndFeel = new preenfmLookAndFeel();
    LookAndFeel::setDefaultLookAndFeel(myLookAndFeel);

    // Important !!!! reset paramIndexCounter.
    Midificator::resetParamIndexCounter();

    pfm2Editor = nullptr;
    teragon::Parameter* newParam;
    parameterIndex = 0;

    for (int k=0; k<2048; k++) {
        nrpmIndex[k] = -1;
    }

    // Algo
    int nrpmParam = PREENFM2_NRPN_ALGO;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Algo").toRawUTF8(), nrpmParam, 1, 1, 28,1);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    // Velocity
    nrpmParam = PREENFM2_NRPN_VELOCITY;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Velocity").toRawUTF8(), nrpmParam, 1, 0, 16,1);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    // Voices
    nrpmParam = PREENFM2_NRPN_VOICE;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Voices").toRawUTF8(), nrpmParam, 1, 0, 16,1);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    // Velocity
    nrpmParam = PREENFM2_NRPN_GLIDE;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Glide").toRawUTF8(), nrpmParam, 1, 0, 10,1);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    for (int k=0; k<6; k++) {
        nrpmParam = PREENFM2_NRPN_MIX1 + k * 2;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Volume "+String(k+1)).toRawUTF8(), nrpmParam, 100, 0,1,1);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;
    }
    for (int k=0; k<6; k++) {
        nrpmParam = PREENFM2_NRPN_PAN1 + k * 2;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Pan "+String(k+1)).toRawUTF8(), nrpmParam, 100, -1, 1, 0);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;
    }
    for (int k=0; k<5; k++) {
        nrpmParam = PREENFM2_NRPN_IM1 + k * 2;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("IM "+String(k+1)).toRawUTF8(), nrpmParam, 100, 0,16,1.5);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;
    }
    for (int k=0; k<5; k++) {
        nrpmParam = PREENFM2_NRPN_IM1_VELOCITY + k * 2;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("IM Velocity "+String(k+1)).toRawUTF8(), nrpmParam, 100, 0,16,1);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;
    }
    // OPERATOR
    for (int k=0; k<6; k++) {
        //        opShape[k] = new ComboBox("OP"+String(k+1)+" Shape");
        //		0, 7,
        nrpmParam = PREENFM2_NRPN_OPERATOR1_SHAPE + k * 4;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Op"+String(k+1)+" Shape").toRawUTF8(), nrpmParam, 1, 1, 8, 1);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;

    }
    for (int k=0; k<6; k++) {
        //        opFrequencyType[k] = new ComboBox("Op"+ String(1) + " Freq Type");
        //        0, 1
        nrpmParam = PREENFM2_NRPN_OPERATOR1_FREQUENCY_TYPE + k * 4;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Op"+String(k+1)+" Freq Type").toRawUTF8(), nrpmParam, 1, 1, 2, 1);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;
    }
    for (int k=0; k<6; k++) {
        //
        //        opFrequency[k] = new Slider("Op" + String(k+1) + " Frequency");
        //        opFrequency[k]->setRange (0, 16, 1.0f / 12.0f);
        nrpmParam = PREENFM2_NRPN_OPERATOR1_FREQUENCY + k * 4;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Op" + String(k+1) + " Frequency").toRawUTF8(), nrpmParam, 100, 0, 16, 1.0);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;
    }
    for (int k=0; k<6; k++) {
        //        opFrequencyFineTune[k] = new Slider("Op"+ String(k+1)+ " Fine Tune");
        //        opFrequencyFineTune[k]->setRange (-1.0f, 1.0f, .01f);
        nrpmParam = PREENFM2_NRPN_OPERATOR1_FREQUENCY_FINE_TUNE + k * 4;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Op"+ String(k+1)+ " Fine Tune").toRawUTF8(), nrpmParam, 100, -1, 1, 0);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;

    }
    for (int k=0; k<6; k++) {
        for (int p=0; p<4; p++) {
            const char* pointName[] = { " Attk", " Deca", " Sust", " Rele" };
            nrpmParam = PREENFM2_NRPN_ENV1_ATTK + k * 8 + p * 2;
            newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Op"+String(k+1)+" Env"+ pointName[p]).toRawUTF8(), nrpmParam, 100, 0, 16, 1);
            newParam->addObserver(this);
            parameterSet.add(newParam);
            nrpmIndex[nrpmParam] = parameterIndex++;
        }
        for (int p=0; p<4; p++) {
            const char* pointName[] = { " Attk lvl", " Deca lvl", " Sust lvl", " Rele lvl" };
            float level[] = { 1.0, 1.0, 1.0, 0.0 };
            nrpmParam = PREENFM2_NRPN_ENV1_ATTK_LEVEL + k * 8 + p * 2;
            newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Op"+String(k+1)+" Env"+ pointName[p]).toRawUTF8(), nrpmParam, 100, 0, 1, level[p]);
            newParam->addObserver(this);
            parameterSet.add(newParam);
            nrpmIndex[nrpmParam] = parameterIndex++;
        }
        //        enveloppe[k]->setName (TRANS("enveloppe " + String(k+1)));
        //
    }
    for (int k=0; k<12; k++) {
        nrpmParam = PREENFM2_NRPN_MTX1_SOURCE + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Mtx"+String(k+1)+" Source").toRawUTF8(), nrpmParam, 1, 1, 18, 1);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;

        nrpmParam = PREENFM2_NRPN_MTX1_MULTIPLIER + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Mtx"+String(k+1)+" Multiplier").toRawUTF8(), nrpmParam, 100, -10, 10, 0);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;

        nrpmParam = PREENFM2_NRPN_MTX1_DESTINATION + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Mtx"+String(k+1)+" Destination").toRawUTF8(), nrpmParam, 1, 1, 43, 1);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;
    }

    for (int k = 0; k < 3; k++) {
        //        addAndMakeVisible(lfoShape[k] = new ComboBox("LFO"+ String(k+1)+ " Shape"));
        //        lfoShape[k]->addItem("Sin", 1);
        //        lfoShape[k]->addItem("Ramp", 2);
        //        lfoShape[k]->addItem("Saw", 3);
        //        lfoShape[k]->addItem("Square", 4);
        //        lfoShape[k]->addItem("Random", 5);
        nrpmParam = PREENFM2_NRPN_LFO1_SHAPE + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("LFO"+ String(k+1)+ " Shape").toRawUTF8(), nrpmParam, 1, 1, 5, 1);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;


        //        addAndMakeVisible(lfoExtMidiSync[k] = new ComboBox("LFO"+ String(k+1) + " External Sync"));
        //        lfoExtMidiSync[k]->addItem("Internal", 2400);
        //        lfoExtMidiSync[k]->addItem("MC/16", 2410);
        //        lfoExtMidiSync[k]->addItem("MC/8", 2420);
        //        lfoExtMidiSync[k]->addItem("MC/4", 2430);
        //        lfoExtMidiSync[k]->addItem("MC/2", 2440);
        //        lfoExtMidiSync[k]->addItem("MC", 2450);
        //        lfoExtMidiSync[k]->addItem("MC*2", 2460);
        //        lfoExtMidiSync[k]->addItem("MC*3", 2470);
        //        lfoExtMidiSync[k]->addItem("MC*4", 2480);
        //        lfoExtMidiSync[k]->addItem("MC*8", 2490);
        nrpmParam = PREENFM2_NRPN_LFO1_FREQUENCY + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("LFO"+ String(k+1) + " External Sync").toRawUTF8(), nrpmParam, 1, 2400, 2490, 2400);
        newParam->addObserver(this);
        ((MidifiedFloatParameter*)newParam)->setSendRealValue(true);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;




        //        addAndMakeVisible(lfoFrequency[k] = new Slider("LFO"+ String(k+1) + " Frequency"));
        //        lfoFrequency[k]->setRange (0, 24.0f, .01f);
        nrpmParam = PREENFM2_NRPN_LFO1_FREQUENCY + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("LFO"+ String(k+1) + " Frequency").toRawUTF8(), nrpmParam, 100, 0, 24, 1);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;



        //        addAndMakeVisible(lfoBias[k] = new Slider("LFO"+ String(k+1) + " Bias"));
        //        lfoBias[k]->setRange (-1.0f, 1.0f, .01f);
        nrpmParam = PREENFM2_NRPN_LFO1_BIAS + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("LFO"+ String(k+1) + " Bias").toRawUTF8(), nrpmParam, 100, -1, 1, 0);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;



        //        addAndMakeVisible(lfoKsynOnOff[k] = new ComboBox("LFO"+ String(k+1) + " KeySync"));
        //        lfoKsynOnOff[k]->addItem("Off", 1);
        //        lfoKsynOnOff[k]->addItem("On", 2);
        nrpmParam = PREENFM2_NRPN_LFO1_KSYN + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("LFO"+ String(k+1) + " KeySync").toRawUTF8(), nrpmParam, 1, 0, 1, 0);
        newParam->addObserver(this);
        ((MidifiedFloatParameter*)newParam)->setBias(-1);
        //        ((MidifiedFloatParameter*)newParam)->setSendRealValue(true);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;

        //        addAndMakeVisible(lfoKSync[k] = new Slider("LFO"+ String(k+1) + " KeySync time"));
        //        lfoKSync[k]->setRange (0.0f, 16.0f, .01f);
        nrpmParam = PREENFM2_NRPN_LFO1_KSYN + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("LFO"+ String(k+1) + " KeySync time").toRawUTF8(), nrpmParam, 100, 0, 16, 0.01);
        newParam->addObserver(this);
        ((MidifiedFloatParameter*)newParam)->setBias(.01);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;
    }

    for (int p=0; p<4; p++) {
        const char* pointName[] = { " Attk", " Deca", " Sust", " Rele" };
        nrpmParam = PREENFM2_NRPN_FREE_ENV1_ATTK + p;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String(String("Free Env 1") + pointName[p]).toRawUTF8(), nrpmParam, 100, 0, 16, 1);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;
    }

    for (int p=0; p<3; p++) {
        const char* pointName[] = { " Sile", " Attk", " Deca" };
        nrpmParam = PREENFM2_NRPN_FREE_ENV2_SILENCE + p;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String(String("Free Env 2") + pointName[p]).toRawUTF8(), nrpmParam, 100, 0, 16, 1);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;
    }






    //"Step Seq " + String(k+1)
    for (int seq=0; seq<2; seq++) {
        //        addAndMakeVisible(stepSeqExtMidiSync[k] = new ComboBox("Step Seq " + String(k+1) + " External Sync"));
        //        stepSeqExtMidiSync[k]->setEditableText (false);
        //        stepSeqExtMidiSync[k]->setColour (ComboBox::buttonColourId, Colours::blue);
        //        stepSeqExtMidiSync[k]->setJustificationType (Justification::left);
        //        stepSeqExtMidiSync[k]->addItem("Internal", 1);
        //        stepSeqExtMidiSync[k]->addItem("MC/4", 2);
        //        stepSeqExtMidiSync[k]->addItem("MC/2", 3);
        //        stepSeqExtMidiSync[k]->addItem("MC", 4);
        //        stepSeqExtMidiSync[k]->addItem("MC*2", 5);
        //        stepSeqExtMidiSync[k]->addItem("MC*4", 6);
        //        stepSeqExtMidiSync[k]->setSelectedId(1);
        //        stepSeqExtMidiSync[k]->addListener (this);


        nrpmParam = PREENFM2_NRPN_STEPSEQ1_BPM + seq *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Step Seq " + String(seq+1) + " External Sync").toRawUTF8(), nrpmParam, 1, 240, 2450, 240);
        newParam->addObserver(this);
        ((MidifiedFloatParameter*)newParam)->setSendRealValue(true);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;


        //        addAndMakeVisible(stepSeqBPM[k] = new Slider("SEQ"+ String(k+1) + " BPM"));
        //        stepSeqBPM[k]->setRange (10, 240.0f, 1.0f);
        //        stepSeqBPM[k]->setSliderStyle (Slider::RotaryVerticalDrag);
        //        stepSeqBPM[k]->setTextBoxStyle (Slider::TextBoxLeft, false, 35, 16);
        //        stepSeqBPM[k]->setDoubleClickReturnValue(true, 3.0f);
        //        stepSeqBPM[k]->setValue(3.0f, dontSendNotification);
        //        stepSeqBPM[k]->addListener (this);

        nrpmParam = PREENFM2_NRPN_STEPSEQ1_BPM + seq *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Step Seq " + String(seq+1) + " BPM").toRawUTF8(), nrpmParam, 1, 10, 240, 1);
        newParam->addObserver(this);
        ((MidifiedFloatParameter*)newParam)->setBias(10);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;


        //        addAndMakeVisible(stepSeqGate[k] = new Slider("SEQ"+ String(k+1) + " Gate"));
        //        stepSeqGate[k]->setRange (0.0f, 1.0f, 0.01f);
        //        stepSeqGate[k]->setSliderStyle (Slider::LinearHorizontal);
        //        stepSeqGate[k]->setTextBoxStyle (Slider::TextBoxBelow, false, 35, 16);
        //        stepSeqGate[k]->setDoubleClickReturnValue(true, 0.5f);
        //        stepSeqGate[k]->setValue(0.5f, dontSendNotification);
        //        stepSeqGate[k]->addListener (this);

        nrpmParam = PREENFM2_NRPN_STEPSEQ1_GATE + seq *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Step Seq " + String(seq+1) + " Gate").toRawUTF8(), nrpmParam, 100, 0, 1, .5);
        newParam->addObserver(this);
        parameterSet.add(newParam);
        nrpmIndex[nrpmParam] = parameterIndex++;


        for (int step= 0; step <16; step ++) {
            nrpmParam = PREENFM2_NRPN_STEPSEQ1_STEP1 + (seq * 128) + step;
            newParam = new MidifiedFloatParameter(&nrpmParameterMap, String(String("Step Seq ") + String(seq + 1) + " Step " + String(step  + 1 )).toRawUTF8(), nrpmParam, 1, 0, 16, 16 - step);
            newParam->addObserver(this);
            parameterSet.add(newParam);
            nrpmIndex[nrpmParam] = parameterIndex++;
        }
    }

    nrpmParam = PREENFM2_NRPN_FREE_ENV2_LOOP;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, "Free Env 2 Loop", nrpmParam, 1, 1, 3, 1);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;



    nrpmParam = PREENFM2_NRPN_ARP_CLOCK ;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Clock Combo").toRawUTF8(), nrpmParam, 1, 1, 3, 1);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_ARP_BPM ;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("arp bpm slider").toRawUTF8(), nrpmParam, 1, 10, 240, 60);
    newParam->addObserver(this);
    ((MidifiedFloatParameter*)newParam)->setBias(10);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_ARP_DIRECTION ;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("arp dir combo box").toRawUTF8(), nrpmParam, 1, 1, 13, 1);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_ARP_OCTAVE ;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("arp octave slider").toRawUTF8(), nrpmParam, 1, 1, 3, 1);
    newParam->addObserver(this);
    ((MidifiedFloatParameter*)newParam)->setBias(1);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_ARP_PATTERN;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("arp pattern combo box").toRawUTF8(), nrpmParam, 1, 1, 25, 1);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_ARP_DIVISION;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("arp division combo box").toRawUTF8(), nrpmParam, 1, 1, 17, 1);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_ARP_DURATION;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("arp duration combo box").toRawUTF8(), nrpmParam, 1, 1, 17, 1);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_ARP_LATCH;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("arp latch combo box").toRawUTF8(), nrpmParam, 1, 1, 2, 1);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_FILTER_TYPE;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Filter Combo").toRawUTF8(), nrpmParam, 1, 1, 7, 1);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_FILTER_PARAM1;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("filter param1 slider").toRawUTF8(), nrpmParam, 100, 0, 1, .5);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_FILTER_PARAM2;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("filter param2 slider").toRawUTF8(), nrpmParam, 100, 0, 1, .5);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_FILTER_GAIN;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("filter gain slider").toRawUTF8(), nrpmParam, 100, 0, 2, .9);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    programName[0] = "pfm2";

    presetName[0] = 'p';
    presetName[1] = 'f';
    presetName[2] = 'm';
    presetName[3] = '2';
    for (int k=4; k<13; k++) {
        presetName[k] = 0;
    }


    // Midi Channel
    currentMidiChannel = 1;
    nrpmParam = 127 * 128 + 126;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, "Midi Channel", nrpmParam, 1, 0, 16,1);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    nrpmIndex[2045] = parameterIndex++;
    
    nrpmParam = 127 * 128 + 126;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, "push button", nrpmParam, 1, 0, 127, 0);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    // Put in last slot
    nrpmIndex[2046] = parameterIndex++;

    nrpmParam = 127 * 128 + 127;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, "pull button", nrpmParam, 1, 0, 127, 0);
    newParam->addObserver(this);
    parameterSet.add(newParam);
    // Put in last slot
    nrpmIndex[2047] = parameterIndex++;

    
    
    midiMessageCollector.reset(44100);
}

Pfm2AudioProcessor::~Pfm2AudioProcessor()
{
    delete myLookAndFeel;
}

//==============================================================================
const String Pfm2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int Pfm2AudioProcessor::getNumParameters()
{
    return parameterSet.size();
}

float Pfm2AudioProcessor::getParameter (int index)
{
    return parameterSet[index]->getScaledValue();
}


const String Pfm2AudioProcessor::getParameterName (int index)
{
    return String(parameterSet[index]->getName().c_str());
}

const String Pfm2AudioProcessor::getParameterText (int index)
{
    return String(parameterSet[index]->getDisplayText().c_str());
}

const String Pfm2AudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String Pfm2AudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool Pfm2AudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool Pfm2AudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool Pfm2AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Pfm2AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool Pfm2AudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double Pfm2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Pfm2AudioProcessor::getNumPrograms()
{
    return NUMBER_OF_PROGRAM;
}

int Pfm2AudioProcessor::getCurrentProgram()
{
    return currentProgram;
}

void Pfm2AudioProcessor::setCurrentProgram (int index)
{
    currentProgram = index;
    if (pfm2Editor) {
        pfm2Editor->setPresetName(programName[index].toRawUTF8());
    }
}

const String Pfm2AudioProcessor::getProgramName (int index)
{
    return programName[index];
}

void Pfm2AudioProcessor::changeProgramName (int index, const String& newName)
{
    programName[index] = newName;
    if (index == currentProgram) {
        if (pfm2Editor) {
            pfm2Editor->setPresetName(programName[index].toRawUTF8());
        }        
    }
}

//==============================================================================
void Pfm2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void Pfm2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}


void Pfm2AudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    handleIncomingMidiBuffer(midiMessages, buffer.getNumSamples());

    // Clear sound
    for (int i = 0; i < getNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // dispatch realtime events to non realtime observer
    parameterSet.processRealtimeEvents();
    midiMessageCollector.removeNextBlockOfMessages(midiMessages,  buffer.getNumSamples());
    /*
    if (midiMessages.getNumEvents() > 0) {
        printf("processBlock : %d midi messages \n", midiMessages.getNumEvents());
    }
     */
    if (parametersToUpdate.size() > 0 ) {
        std::unordered_set<const char*> newSet;
        newSet.swap(parametersToUpdate);
        if (pfm2Editor) {
            pfm2Editor->updateUIWith(newSet);
        }
    }
}

//==============================================================================
bool Pfm2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Pfm2AudioProcessor::createEditor()
{
    pfm2Editor = new Pfm2AudioProcessorEditor (this);
	pfm2Editor->setMidiChannel(currentMidiChannel);
    pfm2Editor->setMidiMessageCollector(midiMessageCollector);
    pfm2Editor->setPresetName(presetName);
    printf("createEditor : %u\n", parameterSet.size());
    return pfm2Editor;
}

//==============================================================================
void Pfm2AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // Here's an example of how you can use XML to make it easy and more robust:

    // Create an outer XML element..
    XmlElement xml ("PreenFM2AppStatus");

    xml.setAttribute("presetName", presetName);

    // add some attributes to it..
    for (int p=0; p< parameterSet.size(); p++) {
        xml.setAttribute(teragon::Parameter::makeSafeName(parameterSet[p]->getName()).c_str(), parameterSet[p]->getScaledValue());
    }

    for (int k=0; k<NUMBER_OF_PROGRAM; k++) {
        xml.setAttribute("Preset"+ String(k), programName[k]);
    }

    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}

void Pfm2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
    {
        String name = xmlState->getStringAttribute("presetName");
        for (int k=0; k<13; k++) {
            presetName[k] = 0;
        }
        for (int k=0; k<13 && name.toRawUTF8()[k] != 0; k++) {
            presetName[k] = name.toRawUTF8()[k];
        }
        if (pfm2Editor) {
            pfm2Editor->setPresetName(presetName);
        }
        printf(">>> PresetName : %s\n", presetName);
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("PreenFM2AppStatus")) {
            
            for (int k=0; k<NUMBER_OF_PROGRAM; k++) {
                if (xmlState->getStringAttribute("Preset" + String(k)) != String::empty){
                    programName[k] = xmlState->getStringAttribute("Preset" + String(k));
                }
            }
            
            double value;
            for (int p=0; p< parameterSet.size(); p++) {
                if (p == nrpmIndex[2047]) continue;

                if (xmlState->getStringAttribute(teragon::Parameter::makeSafeName(parameterSet[p]->getName()).c_str()) != String::empty) {
                    value  = (float) xmlState->getDoubleAttribute (teragon::Parameter::makeSafeName(parameterSet[p]->getName()).c_str(), value);
                    parameterSet.setScaled(p, value, this);
                } else {
                    printf("Cannot set %d : %s\n", p, parameterSet[p]->getName().c_str());
                }
            }
            parameterSet.processRealtimeEvents();
			// If no UI we must set current
			currentMidiChannel = parameterSet[nrpmIndex[2045]]->getValue();
			
            // REDRAW UI
            for (int p=0; p< parameterSet.size(); p++) {
                const MidifiedFloatParameter* midifiedFP = dynamic_cast<const MidifiedFloatParameter*>(parameterSet[p]);
                if (midifiedFP != nullptr) {
                    parametersToUpdate.insert(midifiedFP->getName().c_str());
                }
            }

            // Flush NRPN
            flushAllParametrsToNrpn();
        }
    }
}


void Pfm2AudioProcessor::flushAllParametrsToNrpn() {
    sendNrpnPresetName();

    printf("Pfm2AudioProcessor::flushAllParametrsToNrpn this : 0x%x \n", &midiMessageCollector);

    for (int p=0; p< parameterSet.size(); p++) {
        // Pull button
        if (p == nrpmIndex[2046] || p == nrpmIndex[2047]) {
            continue;
        }
        const MidifiedFloatParameter* midifiedFP = dynamic_cast<const MidifiedFloatParameter*>(parameterSet[p]);
        if (midifiedFP != nullptr) {
//            printf("%d %s Add Nrpn %d : %d\n", p, midifiedFP->getName().c_str(), midifiedFP->getNrpnParamMSB() * 127 + midifiedFP->getNrpnParamLSB(),
//                    midifiedFP->getNrpnValueMSB(midifiedFP->getValue()) * 127 + midifiedFP->getNrpnValueLSB(midifiedFP->getValue()));
            midifiedFP->addNrpn(midiMessageCollector, currentMidiChannel, midifiedFP->getValue());
//            usleep(5000);
        }
    }
}



bool Pfm2AudioProcessor::isRealtimePriority() const {
    return true;
}


// 
void Pfm2AudioProcessor::handleIncomingMidiBuffer(MidiBuffer &buffer, int numberOfSamples) {
    if (!buffer.isEmpty()) {
        MidiBuffer newBuffer;
        MidiMessage midiMessage;
        int samplePosition;
        MidiBuffer::Iterator midiIterator(buffer);
        while (midiIterator.getNextEvent(midiMessage, samplePosition)) {
            bool copyMessageInNewBuffer = true;

            if (midiMessage.isController() && midiMessage.getChannel() == currentMidiChannel) {
                switch (midiMessage.getControllerNumber()) {
                case 99:
                    currentNrpn.paramMSB = midiMessage.getControllerValue();
                    copyMessageInNewBuffer = false;
                    break;
                case 98:
                    currentNrpn.paramLSB = midiMessage.getControllerValue();
                    copyMessageInNewBuffer = false;
                    break;
                case 6:
                    currentNrpn.valueMSB = midiMessage.getControllerValue();
                    copyMessageInNewBuffer = false;
                    break;
                case 38:
                {
                    currentNrpn.valueLSB = midiMessage.getControllerValue();
                    copyMessageInNewBuffer = false;
                    int param = (int)(currentNrpn.paramMSB << 7) + currentNrpn.paramLSB;
                    int value = (int)(currentNrpn.valueMSB << 7) + currentNrpn.valueLSB;

                    const MessageManagerLock mmLock;
                    handleIncomingNrpn(param, value);
                    break;
                }
                }
            }
            if (copyMessageInNewBuffer) {
                newBuffer.addEvent(midiMessage, samplePosition);
            }
        }
        buffer.swapWith(newBuffer);
    }
}

/**
 * Values updated by the HOST
 * . Modify value
    . Send NRPN
	. Refresh UI
 */
void Pfm2AudioProcessor::setParameter (int index, float newValue)
{
    printf("Pfm2AudioProcessor::setParameter  %d : %f\r\n", index, newValue);

    parameterSet.setScaled(index, newValue, this);

    const MidifiedFloatParameter* midifiedFP = dynamic_cast<const MidifiedFloatParameter*>(parameterSet[index]);
    if (midifiedFP != nullptr) {
        // send nrpn
        midifiedFP->addNrpn(midiMessageCollector, currentMidiChannel, midifiedFP->getValue());
    } else {
        printf("Pfm2AudioProcessor::setParameter NULL midifiedFP \r\n");
    }
    // REDRAW UI : must be done in processblock after parameterSet is really udpated.
    parametersToUpdate.insert(midifiedFP->getName().c_str());
}


/**
 * Values updated by the PreenFM2 hardware
 * Here the values has to be modified
  . Modify Value
  . tell host
  . refresh UI
 */
void Pfm2AudioProcessor::handleIncomingNrpn(int param, int value, int forceIndex) {
    // NRPM from the preenFM2
    if (param >= PREENFM2_NRPN_LETTER1 && param <= PREENFM2_NRPN_LETTER12) {
        presetName[param - PREENFM2_NRPN_LETTER1] = value;
        if (pfm2Editor) {
            pfm2Editor->setPresetName(presetName);
        }
    }

    int index = (forceIndex == -1 ? nrpmIndex[param] : forceIndex);

    if (index == -1) {
        //printf("Pfm2AudioProcessor::handleIncomingNrpn NRPNparam %d not registered\r\n", param);
        return;
    }

    Parameter* parameter = parameterSet[index];

    /*
    if (forceIndex != -1) {
        printf("Pfm2AudioProcessor::handleIncomingNrpn redirected to combBox %s (%d, %d)\r\n", parameter->getName().c_str(), param, value);
    } else {
        printf("Pfm2AudioProcessor::handleIncomingNrpn(%d, %d) : %s\r\n", param, value, parameter->getName().c_str());
    }
     */
    MidifiedFloatParameter* midifiedFP = dynamic_cast<MidifiedFloatParameter*>(parameter);
    if (parameter != nullptr) {
        float newFloatValue = midifiedFP->getValueFromNrpn(value);
        // Redirect to combo ?
        if ((newFloatValue > midifiedFP->getMaxValue() || newFloatValue < midifiedFP->getMinValue()) && forceIndex == -1) {
            // First remove current Slider value
            parametersToUpdate.erase(midifiedFP->getName().c_str());
            if (pfm2Editor) {
                pfm2Editor->removeParamToUpdateUI(midifiedFP->getName().c_str());
            }

            // We redirect the Nrpn to previous param
            handleIncomingNrpn(param, value, index -1);
            return;
        }
        // Set the value but we don't want to be notified
        parameterSet.set(index, midifiedFP->getValueFromNrpn(value), this);
        // Notify host
        sendParamChangeMessageToListeners(index, midifiedFP->getScaledValueFromNrpn(value));
        // REDRAW UI : must be done in processblock after parameterSet is really udpated.
        if (pfm2Editor) {
            pfm2Editor->newNrpnParam(param, value);
        }
        parametersToUpdate.insert(midifiedFP->getName().c_str());
    }
}

/**
 * Values updated by the UI
 * Here values are already up to date
 . tell host
 . Send NRPN
 */
void Pfm2AudioProcessor::onParameterUpdated(const teragon::Parameter *parameter) {
//    printf("Pfm2AudioProcessor::onParameterUpdated %s = %f \n", parameter->getName().c_str(), parameter->getValue());

    const MidifiedFloatParameter* midifiedFP = dynamic_cast<const MidifiedFloatParameter*>(parameter);
    if (midifiedFP != nullptr) {
        int index = midifiedFP->getParamIndex();

        if (index == nrpmIndex[2046]) {
            // Push button
            flushAllParametrsToNrpn();
        } else if (index == nrpmIndex[2045]) {
            // Midi Channel changed
            currentMidiChannel = parameter->getValue();
            printf("New midi channel : %d\r\n", currentMidiChannel);
        } else {
            // Notify host
            sendParamChangeMessageToListeners(index, parameter->getScaledValue());
            // send nrpn
            if (!midifiedFP->getSendRealValue() || parameter->getValue() != 1) {
                midifiedFP->addNrpn(midiMessageCollector, currentMidiChannel, parameter->getValue());
            }
        }
    }
}



void Pfm2AudioProcessor::sendNrpnPresetName() {
    for (int k=0; k<12; k++) {
        double timeNow = Time::getMillisecondCounterHiRes() * .001;
        MidiMessage byte1 = MidiMessage::controllerEvent(currentMidiChannel, 99, 1);
        byte1.setTimeStamp(timeNow);
        midiMessageCollector.addMessageToQueue(byte1);

        MidiMessage byte2 = MidiMessage::controllerEvent(currentMidiChannel, 98, 100 + k);
        byte2.setTimeStamp(timeNow);
        midiMessageCollector.addMessageToQueue(byte2);

        int letter = presetName[k];

        MidiMessage byte3 = MidiMessage::controllerEvent(currentMidiChannel, 6, letter >> 7);
        byte3.setTimeStamp(timeNow);
        midiMessageCollector.addMessageToQueue(byte3);

        MidiMessage byte4 = MidiMessage::controllerEvent(currentMidiChannel, 38, letter & 0xff);
        byte4.setTimeStamp(timeNow);
        midiMessageCollector.addMessageToQueue(byte4);
    }

}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Pfm2AudioProcessor();
}

