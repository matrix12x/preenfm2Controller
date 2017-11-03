/*
 * Copyright 2017 Xavier Hosxe
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
#include "UI/PreenLookAndFeel.h"



//==============================================================================
Pfm2AudioProcessor::Pfm2AudioProcessor()
{

	pfm2MidiOut = nullptr;
	pfm2MidiIn = nullptr;

	StringArray devices = MidiOutput::getDevices();

	for (int d = 0; d < devices.size(); d++) {
		DBG("Output : " << devices[d]);
		if (devices[d] == "PreenFM mk2") {
			pfm2MidiOut = MidiOutput::openDevice(d);
			DBG("Output found :)");
			break;
		}
	}

	devices = MidiInput::getDevices();
	for (int d = 0; d < devices.size(); d++) {
		DBG("Input : " << devices[d]);
		if (devices[d] == "PreenFM mk2") {
			DBG("Input found :)" );
			if ((pfm2MidiIn = MidiInput::openDevice(d, this)) != nullptr) {
				pfm2MidiIn->start();
			}
			break;
		}
	}

	if (pfm2MidiIn == nullptr || pfm2MidiOut == nullptr) {
		AlertWindow::showMessageBox(AlertWindow::WarningIcon,
			":-(",
			"Cannot find your preenfm2 on the USB port");
	}
    myLookAndFeel = new preenfmLookAndFeel();
	LookAndFeel::setDefaultLookAndFeel(myLookAndFeel);

    // Important !!!! reset paramIndexCounter.
	MidifiedFloatParameter::resetParamIndexCounter();

    pfm2Editor = nullptr;
	MidifiedFloatParameter* newParam;
    parameterIndex = 0;

    for (int k=0; k<2048; k++) {
        nrpmIndex[k] = -1;
    }

    // Algo
    int nrpmParam = PREENFM2_NRPN_ALGO;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Algo"), nrpmParam, 1, 1, 28,1);
	addMidifiedParameter(newParam);
    nrpmIndex[nrpmParam] = parameterIndex++;

    // Velocity
    nrpmParam = PREENFM2_NRPN_VELOCITY;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Velocity"), nrpmParam, 1, 0, 16,1);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;

    // Voices
    nrpmParam = PREENFM2_NRPN_VOICE;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Voices"), nrpmParam, 1, 0, 16,1);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;

    // Velocity
    nrpmParam = PREENFM2_NRPN_GLIDE;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Glide"), nrpmParam, 1, 0, 10,1);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;

    for (int k=0; k<6; k++) {
        nrpmParam = PREENFM2_NRPN_MIX1 + k * 2;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Volume "+String(k+1)), nrpmParam, 100, 0,1,1);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;
    }
    for (int k=0; k<6; k++) {
        nrpmParam = PREENFM2_NRPN_PAN1 + k * 2;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Pan "+String(k+1)), nrpmParam, 100, -1, 1, 0);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;
    }
    for (int k=0; k<5; k++) {
        nrpmParam = PREENFM2_NRPN_IM1 + k * 2;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("IM "+String(k+1)), nrpmParam, 100, 0,16,1.5);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;
    }
    for (int k=0; k<5; k++) {
        nrpmParam = PREENFM2_NRPN_IM1_VELOCITY + k * 2;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("IM Velocity "+String(k+1)), nrpmParam, 100, 0,16,1);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;
    }
    // OPERATOR
    for (int k=0; k<6; k++) {
        //        opShape[k] = new ComboBox("OP"+String(k+1)+" Shape");
        //		0, 7,
        nrpmParam = PREENFM2_NRPN_OPERATOR1_SHAPE + k * 4;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Op"+String(k+1)+" Shape"), nrpmParam, 1, 1, 14, 1);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;

    }
    for (int k=0; k<6; k++) {
        //        opFrequencyType[k] = new ComboBox("Op"+ String(1) + " Freq Type");
        //        0, 1
        nrpmParam = PREENFM2_NRPN_OPERATOR1_FREQUENCY_TYPE + k * 4;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Op"+String(k+1)+" Freq Type"), nrpmParam, 1, 1, 3, 1);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;
    }
    for (int k=0; k<6; k++) {
        //
        //        opFrequency[k] = new Slider("Op" + String(k+1) + " Frequency");
        //        opFrequency[k]->setRange (0, 16, 1.0f / 12.0f);
        nrpmParam = PREENFM2_NRPN_OPERATOR1_FREQUENCY + k * 4;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Op" + String(k+1) + " Frequency"), nrpmParam, 100, 0, 16, 1.0);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;
    }
    for (int k=0; k<6; k++) {
        //        opFrequencyFineTune[k] = new Slider("Op"+ String(k+1)+ " Fine Tune");
        //        opFrequencyFineTune[k]->setRange (-1.0f, 1.0f, .01f);
        nrpmParam = PREENFM2_NRPN_OPERATOR1_FREQUENCY_FINE_TUNE + k * 4;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Op"+ String(k+1)+ " Fine Tune"), nrpmParam, 100, -9, 9, 0);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;

    }
    for (int k=0; k<6; k++) {
        for (int p=0; p<4; p++) {
            const char* pointName[] = { " Attk", " Deca", " Sust", " Rele" };
            nrpmParam = PREENFM2_NRPN_ENV1_ATTK + k * 8 + p * 2;
            newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Op"+String(k+1)+" Env"+ pointName[p]), nrpmParam, 100, 0, 16, 1);
			addMidifiedParameter(newParam);
			nrpmIndex[nrpmParam] = parameterIndex++;
        }
        for (int p=0; p<4; p++) {
            const char* pointName[] = { " Attk lvl", " Deca lvl", " Sust lvl", " Rele lvl" };
            float level[] = { 1.0, 1.0, 1.0, 0.0 };
            nrpmParam = PREENFM2_NRPN_ENV1_ATTK_LEVEL + k * 8 + p * 2;
            newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Op"+String(k+1)+" Env"+ pointName[p]), nrpmParam, 100, 0, 1, level[p]);
			addMidifiedParameter(newParam);
			nrpmIndex[nrpmParam] = parameterIndex++;
        }
        //        enveloppe[k]->setName (TRANS("enveloppe " + String(k+1)));
        //
    }
    for (int k=0; k<12; k++) {
        nrpmParam = PREENFM2_NRPN_MTX1_SOURCE + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Mtx"+String(k+1)+" Source"), nrpmParam, 1, 1, 19, 1);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;

        nrpmParam = PREENFM2_NRPN_MTX1_MULTIPLIER + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Mtx"+String(k+1)+" Multiplier"), nrpmParam, 100, -10, 10, 0);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;

        nrpmParam = PREENFM2_NRPN_MTX1_DESTINATION + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Mtx"+String(k+1)+" Destination"), nrpmParam, 1, 1, 47, 1);
		addMidifiedParameter(newParam);
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
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("LFO"+ String(k+1)+ " Shape"), nrpmParam, 1, 1, 5, 1);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;

        nrpmParam = PREENFM2_NRPN_LFO1_FREQUENCY + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("LFO"+ String(k+1) + " External Sync"), nrpmParam, 1, 9990, 10080, 9990);
        ((MidifiedFloatParameter*)newParam)->setSendRealValue(true);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;




        //        addAndMakeVisible(lfoFrequency[k] = new Slider("LFO"+ String(k+1) + " Frequency"));
        //        lfoFrequency[k]->setRange (0, 24.0f, .01f);
        nrpmParam = PREENFM2_NRPN_LFO1_FREQUENCY + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("LFO"+ String(k+1) + " Frequency"), nrpmParam, 100, 0, 99.9, 1);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;



        //        addAndMakeVisible(lfoBias[k] = new Slider("LFO"+ String(k+1) + " Bias"));
        //        lfoBias[k]->setRange (-1.0f, 1.0f, .01f);
        nrpmParam = PREENFM2_NRPN_LFO1_BIAS + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("LFO"+ String(k+1) + " Bias"), nrpmParam, 100, -1, 1, 0);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;



        //        addAndMakeVisible(lfoKsynOnOff[k] = new ComboBox("LFO"+ String(k+1) + " KeySync"));
        //        lfoKsynOnOff[k]->addItem("Off", 1);
        //        lfoKsynOnOff[k]->addItem("On", 2);
        nrpmParam = PREENFM2_NRPN_LFO1_KSYN + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("LFO"+ String(k+1) + " KeySync"), nrpmParam, 1, 0, 1, 0);
        ((MidifiedFloatParameter*)newParam)->setBias(-1);
        ((MidifiedFloatParameter*)newParam)->setSendRealValue(true);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;

        //        addAndMakeVisible(lfoKSync[k] = new Slider("LFO"+ String(k+1) + " KeySync time"));
        //        lfoKSync[k]->setRange (0.0f, 16.0f, .01f);
        nrpmParam = PREENFM2_NRPN_LFO1_KSYN + k *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("LFO"+ String(k+1) + " KeySync time"), nrpmParam, 100, 0, 16, 0.01);
        ((MidifiedFloatParameter*)newParam)->setBias(.01);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;
    }

    for (int p=0; p<4; p++) {
        const char* pointName[] = { " Attk", " Deca", " Sust", " Rele" };
        nrpmParam = PREENFM2_NRPN_FREE_ENV1_ATTK + p;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String(String("Free Env 1") + pointName[p]), nrpmParam, 100, 0, 16, 1);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;
    }

    for (int p=0; p<3; p++) {
        const char* pointName[] = { " Sile", " Attk", " Deca" };
        nrpmParam = PREENFM2_NRPN_FREE_ENV2_SILENCE + p;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String(String("Free Env 2") + pointName[p]), nrpmParam, 100, 0, 16, 1);
		addMidifiedParameter(newParam);
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
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Step Seq " + String(seq+1) + " External Sync"), nrpmParam, 1, 240, 2450, 240);
        ((MidifiedFloatParameter*)newParam)->setSendRealValue(true);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;


        //        addAndMakeVisible(stepSeqBPM[k] = new Slider("SEQ"+ String(k+1) + " BPM"));
        //        stepSeqBPM[k]->setRange (10, 240.0f, 1.0f);
        //        stepSeqBPM[k]->setSliderStyle (Slider::RotaryVerticalDrag);
        //        stepSeqBPM[k]->setTextBoxStyle (Slider::TextBoxLeft, false, 35, 16);
        //        stepSeqBPM[k]->setDoubleClickReturnValue(true, 3.0f);
        //        stepSeqBPM[k]->setValue(3.0f, dontSendNotification);
        //        stepSeqBPM[k]->addListener (this);

        nrpmParam = PREENFM2_NRPN_STEPSEQ1_BPM + seq *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Step Seq " + String(seq+1) + " BPM"), nrpmParam, 1, 10, 240, 1);
        ((MidifiedFloatParameter*)newParam)->setBias(10);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;


        //        addAndMakeVisible(stepSeqGate[k] = new Slider("SEQ"+ String(k+1) + " Gate"));
        //        stepSeqGate[k]->setRange (0.0f, 1.0f, 0.01f);
        //        stepSeqGate[k]->setSliderStyle (Slider::LinearHorizontal);
        //        stepSeqGate[k]->setTextBoxStyle (Slider::TextBoxBelow, false, 35, 16);
        //        stepSeqGate[k]->setDoubleClickReturnValue(true, 0.5f);
        //        stepSeqGate[k]->setValue(0.5f, dontSendNotification);
        //        stepSeqGate[k]->addListener (this);

        nrpmParam = PREENFM2_NRPN_STEPSEQ1_GATE + seq *4 ;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Step Seq " + String(seq+1) + " Gate"), nrpmParam, 100, 0, 1, .5);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;


        for (int step= 0; step <16; step ++) {
            nrpmParam = PREENFM2_NRPN_STEPSEQ1_STEP1 + (seq * 128) + step;
            newParam = new MidifiedFloatParameter(&nrpmParameterMap, String(String("Step Seq ") + String(seq + 1) + " Step " + String(step  + 1 )), nrpmParam, 1, 0, 16, 16 - step);
			addMidifiedParameter(newParam);
			nrpmIndex[nrpmParam] = parameterIndex++;
        }
    }

    nrpmParam = PREENFM2_NRPN_FREE_ENV2_LOOP;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, "Free Env 2 Loop", nrpmParam, 1, 1, 3, 1);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;



    nrpmParam = PREENFM2_NRPN_ARP_CLOCK ;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Clock Combo"), nrpmParam, 1, 1, 3, 1);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_ARP_BPM ;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("arp bpm slider"), nrpmParam, 1, 10, 240, 60);
    ((MidifiedFloatParameter*)newParam)->setBias(10);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_ARP_DIRECTION ;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("arp dir combo box"), nrpmParam, 1, 1, 13, 1);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_ARP_OCTAVE ;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("arp octave slider"), nrpmParam, 1, 1, 3, 1);
    ((MidifiedFloatParameter*)newParam)->setBias(1);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_ARP_PATTERN;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("arp pattern combo box"), nrpmParam, 1, 1, 25, 1);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_ARP_DIVISION;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("arp division combo box"), nrpmParam, 1, 1, 17, 1);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_ARP_DURATION;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("arp duration combo box"), nrpmParam, 1, 1, 17, 1);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_ARP_LATCH;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("arp latch combo box"), nrpmParam, 1, 1, 2, 1);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_FILTER_TYPE;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Filter Combo"), nrpmParam, 1, 1, 7, 1);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_FILTER_PARAM1;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("filter param1 slider"), nrpmParam, 100, 0, 1, .5);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_FILTER_PARAM2;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("filter param2 slider"), nrpmParam, 100, 0, 1, .5);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;

    nrpmParam = PREENFM2_NRPN_FILTER_GAIN;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("filter gain slider"), nrpmParam, 100, 0, 2, .9);
	addMidifiedParameter(newParam);
	nrpmIndex[nrpmParam] = parameterIndex++;


    for (int k=0; k<3; k++) {
        // addAndMakeVisible(lfoPhase[k] = new SliderPfm2("LFO"+ String(k+1) + " Phase"));
        // lfoPhase[k]->setRange (0, 1.0f, .01f);
        nrpmParam = PREENFM2_NRPN_LFO_PHASE1 + k;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("LFO"+ String(k+1) + " Phase"), nrpmParam, 100, 0, 1, 0);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;
    }

    for (int n=0; n<2; n++) {

        //    addAndMakeVisible(noteBefore[n] = new ComboBox("Note"+ String(n + 1)+" before"));
        //    noteBefore[n]->setEditableText (false);
        //    noteBefore[n]->setJustificationType (Justification::left);
        //    noteBefore[n]->setColour (ComboBox::buttonColourId, Colours::blue);
        //    noteBefore[n]->addItem("Flat", 1);
        //    noteBefore[n]->addItem("+Linear", 2);
        //    noteBefore[n]->addItem("+Linear*8", 3);
        //    noteBefore[n]->addItem("+Exp", 4);
        //    noteBefore[n]->addItem("-Linear", 5);
        //    noteBefore[n]->addItem("-Linear*8", 6);
        //    noteBefore[n]->addItem("-Exp", 7);
        //    noteBefore[n]->setSelectedId(1);
        //    noteBefore[n]->addListener (this);
        nrpmParam = PREENFM2_NRPN_NOTE1_BEFORE + n * 4;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Note"+ String(n + 1)+" before"), nrpmParam, 1, 1, 7, 5);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;


        //    addAndMakeVisible(noteBreak[n] = new SliderPfm2("Note"+ String(n + 1)+" break"));
        //    noteBreak[n]->setRange (0, 127.0f, 1.0f);
        //    noteBreak[n]->setSliderStyle (Slider::RotaryVerticalDrag);
        //    noteBreak[n]->setTextBoxStyle (Slider::TextBoxBelow, false, 35, 16);
        //    noteBreak[n]->setDoubleClickReturnValue(true, 3.0f);
        //    noteBreak[n]->setValue(3.0f, dontSendNotification);
        //    noteBreak[n]->addListener (this);
        nrpmParam = PREENFM2_NRPN_NOTE1_BREAKNOTE + n * 4;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Note"+ String(n + 1)+" break"), nrpmParam, 1, 0, 127, 60);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;

        //    addAndMakeVisible(noteAfter[n] = new ComboBox("Note"+ String(n + 1)+" after"));
        //    noteAfter[n]->setEditableText (false);
        //    noteAfter[n]->setJustificationType (Justification::left);
        //    noteAfter[n]->setColour (ComboBox::buttonColourId, Colours::blue);
        //    noteAfter[n]->addItem("Flat", 1);
        //    noteAfter[n]->addItem("+Linear", 2);
        //    noteAfter[n]->addItem("+Linear*8", 3);
        //    noteAfter[n]->addItem("+Exp", 4);
        //    noteAfter[n]->addItem("-Linear", 5);
        //    noteAfter[n]->addItem("-Linear*8", 6);
        //    noteAfter[n]->addItem("-Exp", 7);
        //    noteAfter[n]->setSelectedId(1);
        //    noteAfter[n]->addListener (this);
        nrpmParam = PREENFM2_NRPN_NOTE1_AFTER + n * 4;
        newParam = new MidifiedFloatParameter(&nrpmParameterMap, String("Note"+ String(n + 1)+" after"), nrpmParam, 1, 1, 7, 1);
		addMidifiedParameter(newParam);
		nrpmIndex[nrpmParam] = parameterIndex++;

    }


	presetName = "pfm2 init";


    // Midi Channel
    currentMidiChannel = 1;
    nrpmParam = 127 * 128 + 126;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, "Midi Channel", nrpmParam, 1, 0, 16,1);
	addMidifiedParameter(newParam);
	nrpmIndex[2045] = parameterIndex++;
    
    nrpmParam = 127 * 128 + 126;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, "push button", nrpmParam, 1, 0, 127, 0);
	addMidifiedParameter(newParam);
	// Put in last slot
    nrpmIndex[2046] = parameterIndex++;

    nrpmParam = 127 * 128 + 127;
    newParam = new MidifiedFloatParameter(&nrpmParameterMap, "pull button", nrpmParam, 1, 0, 127, 0);
	addMidifiedParameter(newParam);
	// Put in last slot
    nrpmIndex[2047] = parameterIndex++;
   
    midiMessageCollector.reset(44100);


}

Pfm2AudioProcessor::~Pfm2AudioProcessor()
{
   delete myLookAndFeel;

   if (pfm2MidiOut != nullptr) {
	   delete pfm2MidiOut;
   }
   if (pfm2MidiIn != nullptr) {
	   pfm2MidiIn->stop();
	   delete pfm2MidiIn;
   }

}

//==============================================================================
const String Pfm2AudioProcessor::getName() const
{
	return "preenfm2 Editor"; // JucePlugin_Name;
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
    return 1;
}

int Pfm2AudioProcessor::getCurrentProgram()
{
    return 1;
}

void Pfm2AudioProcessor::setCurrentProgram (int index)
{
    // Nothing to do;
}

const String Pfm2AudioProcessor::getProgramName (int index)
{
    return presetName;
}

void Pfm2AudioProcessor::changeProgramName (int index, const String& newName)
{
	setPresetName(newName);
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

    // Clear sound
	for (int i = 0; i < getNumOutputChannels(); ++i) {
		buffer.clear(i, 0, buffer.getNumSamples());
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
    pfm2Editor->setMidiOutBuffer(&midiOutBuffer);
    pfm2Editor->setPresetName(presetName);

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
	const OwnedArray< AudioProcessorParameter >&parameterSet = getParameters();
    for (int p=0; p< parameterSet.size(); p++) {
		MidifiedFloatParameter* midifiedFP = (MidifiedFloatParameter*)parameterSet[p];
		xml.setAttribute(midifiedFP->getNameForXML(), midifiedFP->getRealValue());
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
        presetName = xmlState->getStringAttribute("presetName");

		if (pfm2Editor) {
            pfm2Editor->setPresetName(presetName);
        }


        if (xmlState->hasTagName ("PreenFM2AppStatus")) {
			const OwnedArray< AudioProcessorParameter >&parameterSet = getParameters();

            double value;
            for (int p=0; p< parameterSet.size(); p++) {
				// End ?
                if (p == nrpmIndex[2046]) break;

				MidifiedFloatParameter* midifiedFP = (MidifiedFloatParameter*)parameterSet[p];
				if (xmlState->hasAttribute(midifiedFP->getNameForXML())) {
                    value  = (float) xmlState->getDoubleAttribute (midifiedFP->getNameForXML());
					midifiedFP->setRealValue(value);
                }
            }

			// If no UI we must set current
			MidifiedFloatParameter* midifiedFP = (MidifiedFloatParameter*)parameterSet[nrpmIndex[2045]];
			currentMidiChannel = midifiedFP->getRealValue();
			
            // REDRAW UI
            for (int p=0; p< parameterSet.size(); p++) {
				parameterUpdatedForUI(p);
            }

			// Start Flushing NRPN
			flushAllParametrsToNrpn();
        }
    }
}

void Pfm2AudioProcessor::parameterUpdatedForUI(int p) {
	if (pfm2Editor) {
		MidifiedFloatParameter* midifiedFP = (MidifiedFloatParameter*)getParameters()[p];
		pfm2Editor->parametersToUpdate.insert(midifiedFP->getName());
	}	
}

void Pfm2AudioProcessor::flushAllParametrsToNrpn() {
	sendNrpnPresetName();

	const OwnedArray< AudioProcessorParameter >&parameterSet = getParameters();
	int p;
    for (p = 0; p < parameterSet.size(); p++) {
        // Pull/push button midi channel
        if (p > nrpmIndex[2045]) {
            continue;
        }
         MidifiedFloatParameter* midifiedFP = (MidifiedFloatParameter*)parameterSet[p];
        if (midifiedFP != nullptr) {
			//DBG(String(p) << " " << parameterSet[p]->getName(256) << " Add Nrpn " << String(midifiedFP->getNrpnParamMSB() * 127 + midifiedFP->getNrpnParamLSB()) << " value " << String(midifiedFP->getNrpnValueMSB(midifiedFP->getValue()) * 127 + midifiedFP->getNrpnValueLSB(midifiedFP->getValue())));
            midifiedFP->addNrpn(midiOutBuffer, currentMidiChannel);
        }
    }
	flushMidiOut();

}



bool Pfm2AudioProcessor::isRealtimePriority() const {
    return true;
}



/**
 * Values updated by the HOST
 * . Modify value
    . Send NRPN
	. Refresh UI
 */
void Pfm2AudioProcessor::setParameter (int index, float newValue)
{
     MidifiedFloatParameter* midifiedFP = (MidifiedFloatParameter*)getParameters()[index];

	midifiedFP->setValue(newValue);
    // send nrpn
    midifiedFP->addNrpn(midiOutBuffer, currentMidiChannel);
	flushMidiOut();
	// REDRAW UI 
	parameterUpdatedForUI(index);
}


/**
 * Values updated by the PreenFM2 hardware
 * Here the values has to be modified
  . Modify Value
  . tell host
  . refresh UI
 */
void Pfm2AudioProcessor::handleIncomingNrpn(int param, int nrpnValue, int forceIndex) {
    // NRPM from the preenFM2

    if (param >= PREENFM2_NRPN_LETTER1 && param <= PREENFM2_NRPN_LETTER12) {
		char newName[13] = "\0\0\0\0\0\0\0\0\0\0\0\0";
		for (int k = 0; k < presetName.length(); k++) {
			newName[k] = presetName[k];
		}
		newName[(int)(param - PREENFM2_NRPN_LETTER1)] = nrpnValue;
		presetName = String(newName);
        if (pfm2Editor) {
            pfm2Editor->setPresetName(presetName);
			return;
        }
    }

    int index = (forceIndex == -1 ? nrpmIndex[param] : forceIndex);

    if (index == -1) {
		// NRN Param not registered
        return;
    }
	
	const OwnedArray< AudioProcessorParameter >&parameters = getParameters();
	MidifiedFloatParameter* midifiedFP = (MidifiedFloatParameter*)parameters[index];
    if (midifiedFP != nullptr) {
        float newFloatValue = midifiedFP->getValueFromNrpn(nrpnValue);
        // Redirect to combo ?
		float end = midifiedFP->getMax();
		float start = midifiedFP->getMin();
		if ((newFloatValue > end || newFloatValue < start) && forceIndex == -1) {
            // First remove current Slider value
            if (pfm2Editor) {
                pfm2Editor->removeParamToUpdateUI(midifiedFP->getName());
            }

            // We redirect the Nrpn to previous param
            handleIncomingNrpn(param, nrpnValue, index -1);
            return;
        }
        // Set the value 
		midifiedFP->setValueFromNrpn(nrpnValue);
        // Notify host
        sendParamChangeMessageToListeners(index, midifiedFP->getValue());
       
		// update nrpn labels
		if (pfm2Editor) {
            pfm2Editor->newNrpnParam(param, nrpnValue);
        }

		parameterUpdatedForUI(index);
    }
}

/**
 * Values updated by the UI
 * Here values are already up to date
 . tell host
 . Send NRPN
 */
void Pfm2AudioProcessor::onParameterUpdated(AudioProcessorParameter *parameter) {
    

    MidifiedFloatParameter* midifiedFP = (MidifiedFloatParameter*)parameter;
    if (midifiedFP != nullptr) {
        int index = midifiedFP->getParamIndex();
		
        if (index == nrpmIndex[2046]) {
            // Push button
			flushAllParametrsToNrpn();
        } else if (index == nrpmIndex[2045]) {
            // Midi Channel changed
            currentMidiChannel = midifiedFP->getRealValue();
		}
		else if (index == nrpmIndex[2047]) {
			// Don't notify host
			// send nrpn
			midifiedFP->addNrpn(midiOutBuffer, currentMidiChannel);
			flushMidiOut();
		} else {
            // Notify host
            sendParamChangeMessageToListeners(index, midifiedFP->getValue());

			// send nrpn
            midifiedFP->addNrpn(midiOutBuffer, currentMidiChannel);
			flushMidiOut();
        }
    }
}

void Pfm2AudioProcessor::flushMidiOut() {
	if (pfm2MidiOut != nullptr) {
		pfm2MidiOut->sendBlockOfMessagesNow(midiOutBuffer);
	}
	midiOutBuffer.clear();
}

void Pfm2AudioProcessor::setPresetName(String newName) {
		presetName = newName;
}


void Pfm2AudioProcessor::sendNrpnPresetName() {
    for (int k=0; k<12; k++) {
        double timeNow = Time::getMillisecondCounterHiRes() * .001;
        MidiMessage byte1 = MidiMessage::controllerEvent(currentMidiChannel, 99, 1);
        byte1.setTimeStamp(timeNow);
		midiOutBuffer.addEvent(byte1, 512);

        MidiMessage byte2 = MidiMessage::controllerEvent(currentMidiChannel, 98, 100 + k);
        byte2.setTimeStamp(timeNow);
		midiOutBuffer.addEvent(byte2, 512);

        int letter = presetName[k];

        MidiMessage byte3 = MidiMessage::controllerEvent(currentMidiChannel, 6, letter >> 7);
        byte3.setTimeStamp(timeNow);
		midiOutBuffer.addEvent(byte3, 512);

        MidiMessage byte4 = MidiMessage::controllerEvent(currentMidiChannel, 38, letter & 0xff);
        byte4.setTimeStamp(timeNow);
		midiOutBuffer.addEvent(byte4, 512);
	}

	flushMidiOut();
}

void Pfm2AudioProcessor::addMidifiedParameter(MidifiedFloatParameter *param) {
	addParameter(param);
	param->setProcessor(this);
}


void Pfm2AudioProcessor::handleIncomingMidiMessage(MidiInput *source, const MidiMessage &midiMessage) {

	if (midiMessage.isController() && midiMessage.getChannel() == currentMidiChannel) {
//		DBG("Midi message " << midiMessage.getControllerNumber() << " , " << midiMessage.getControllerValue() << "\n");
		switch (midiMessage.getControllerNumber()) {
		case 99:
			currentNrpn.paramMSB = midiMessage.getControllerValue();
			break;
		case 98:
			currentNrpn.paramLSB = midiMessage.getControllerValue();
			break;
		case 6:
			currentNrpn.valueMSB = midiMessage.getControllerValue();
			break;
		case 38:
		{
			currentNrpn.valueLSB = midiMessage.getControllerValue();
			int param = (int)(currentNrpn.paramMSB << 7) + currentNrpn.paramLSB;
			int value = (int)(currentNrpn.valueMSB << 7) + currentNrpn.valueLSB;

			const MessageManagerLock mmLock;
			handleIncomingNrpn(param, value);
			break;
		}
		}
	}
}

void Pfm2AudioProcessor::handlePartialSysexMessage(MidiInput *source, const uint8 *messageData, int numBytesSoFar, double timestamp) {

}



//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Pfm2AudioProcessor();
}

