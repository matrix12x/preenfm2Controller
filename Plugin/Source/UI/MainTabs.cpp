/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "JuceHeader.h"
#include "../PluginParameters/include/PluginParameters.h"
#include "PanelEngine.h"
#include "PanelModulation.h"
#include "PanelArpAndFilter.h"
//[/Headers]

#include "MainTabs.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
MainTabs::MainTabs ()
{
    addAndMakeVisible (tabbedComponent = new TabbedComponent (TabbedButtonBar::TabsAtTop));
    tabbedComponent->setTabBarDepth (30);
    tabbedComponent->addTab (TRANS("Engine"), Colour (0xffe5f9ff), new PanelEngine(), true);
    tabbedComponent->addTab (TRANS("Modulation"), Colour (0xffdeffe4), new PanelModulation(), true);
    tabbedComponent->addTab (TRANS("Arp & Filter"), Colour (0xfffffade), new PanelArpAndFilter(), true);
    tabbedComponent->setCurrentTabIndex (0);

    addAndMakeVisible (midiInputLabel = new Label ("midi input label",
                                                   TRANS("0")));
    midiInputLabel->setFont (Font (15.00f, Font::plain));
    midiInputLabel->setJustificationType (Justification::centredLeft);
    midiInputLabel->setEditable (false, false, false);
    midiInputLabel->setColour (Label::textColourId, Colours::blue);
    midiInputLabel->setColour (TextEditor::textColourId, Colours::black);
    midiInputLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (midiInputLabel2 = new Label ("midi input label",
                                                    TRANS("0")));
    midiInputLabel2->setFont (Font (15.00f, Font::plain));
    midiInputLabel2->setJustificationType (Justification::centredLeft);
    midiInputLabel2->setEditable (false, false, false);
    midiInputLabel2->setColour (Label::textColourId, Colours::blue);
    midiInputLabel2->setColour (TextEditor::textColourId, Colours::black);
    midiInputLabel2->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (pullButton = new TextButton ("pull button"));
    pullButton->setButtonText (TRANS("Pull"));
    pullButton->addListener (this);

    addAndMakeVisible (presetNameLabel = new Label ("preset name label",
                                                    TRANS("preset")));
    presetNameLabel->setTooltip (TRANS("Click to edit"));
    presetNameLabel->setFont (Font (25.90f, Font::bold));
    presetNameLabel->setJustificationType (Justification::centredLeft);
    presetNameLabel->setEditable (true, true, false);
    presetNameLabel->setColour (TextEditor::textColourId, Colours::black);
    presetNameLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    presetNameLabel->addListener (this);

    addAndMakeVisible (pushButton = new TextButton ("push button"));
    pushButton->setButtonText (TRANS("Push"));
    pushButton->addListener (this);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (900, 710);


    //[Constructor] You can add your own custom stuff here..
	panelEngine = ((PanelEngine*)tabbedComponent->getTabContentComponent(0));
	panelModulation = ((PanelModulation*)tabbedComponent->getTabContentComponent(1));
	panelArpAndFilter = ((PanelArpAndFilter*)tabbedComponent->getTabContentComponent(2));
	pullButtonValue = 0;
    // SET null !
    //[/Constructor]
}

MainTabs::~MainTabs()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    tabbedComponent = nullptr;
    midiInputLabel = nullptr;
    midiInputLabel2 = nullptr;
    pullButton = nullptr;
    presetNameLabel = nullptr;
    pushButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void MainTabs::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::aliceblue);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MainTabs::resized()
{
    tabbedComponent->setBounds (proportionOfWidth (0.0093f), proportionOfHeight (0.0251f), proportionOfWidth (0.9785f), proportionOfHeight (0.9694f));
    midiInputLabel->setBounds (552, 8, 48, 16);
    midiInputLabel2->setBounds (616, 8, 48, 16);
    pullButton->setBounds (getWidth() - 74, 8, 55, 24);
    presetNameLabel->setBounds (288, 8, 200, 32);
    pushButton->setBounds (getWidth() - 156, 8, 55, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MainTabs::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == pullButton)
    {
        //[UserButtonCode_pullButton] -- add your button handler code here..

        teragon::Parameter* param =  parameterSet->get("pull button");
        // Will remove that later but dont' BUG for the moment if that doesn't fit
        if (param == nullptr) {
            printf("PULL BUTTON does not exist...\r\n");
            return;
        }

        pullButtonValue = (pullButtonValue == 0 ? 1 : 0) ;
        printf("MainTabs::PULL BUTTON PRESSED\r\n");
        parameterSet->set(param, pullButtonValue, nullptr);

        //[/UserButtonCode_pullButton]
    }
    else if (buttonThatWasClicked == pushButton)
    {
        //[UserButtonCode_pushButton] -- add your button handler code here..
        teragon::Parameter* param =  parameterSet->get("push button");
        // Will remove that later but dont' BUG for the moment if that doesn't fit
        if (param == nullptr) {
            printf("PUSH BUTTON does not exist...\r\n");
            return;
        }

        pushButtonValue = (pushButtonValue == 0 ? 1 : 0) ;
        printf("MainTabs::PUSH BUTTON PRESSED\r\n");
        parameterSet->set(param, pushButtonValue, nullptr);

        //[/UserButtonCode_pushButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void MainTabs::labelTextChanged (Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == presetNameLabel)
    {
        //[UserLabelCode_presetNameLabel] -- add your label text handling code here..
        for (int k=0; k<12; k++) {
            int timeNow = Time::getMillisecondCounter();
            MidiMessage byte1 = MidiMessage::controllerEvent(1, 99, 1);
            byte1.setTimeStamp(timeNow);
            midiMessageCollector->addMessageToQueue(byte1);

            MidiMessage byte2 = MidiMessage::controllerEvent(1, 98, 100 + k);
            byte2.setTimeStamp(timeNow);
            midiMessageCollector->addMessageToQueue(byte2);

            int letter = presetNameLabel->getText().toRawUTF8()[k];

            MidiMessage byte3 = MidiMessage::controllerEvent(1, 6, letter >> 7);
            byte3.setTimeStamp(timeNow);
            midiMessageCollector->addMessageToQueue(byte3);

            MidiMessage byte4 = MidiMessage::controllerEvent(1, 38, letter & 0xff);
            byte4.setTimeStamp(timeNow);
            midiMessageCollector->addMessageToQueue(byte4);
        }
        //[/UserLabelCode_presetNameLabel]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...


void MainTabs::buildParameters(teragon::ConcurrentParameterSet& parameterSet) {
    this->parameterSet = &parameterSet;

    panelEngine->setParameterSet(parameterSet);
    panelEngine->buildParameters();

    panelModulation->setParameterSet(parameterSet);
    panelModulation->buildParameters();

    panelArpAndFilter->setParameterSet(parameterSet);
    panelArpAndFilter->buildParameters();
}

void MainTabs::updateUI(std::unordered_set<const char*> &paramSet) {
    /*
    printf("====================================================\r\n");
    printf("====> %d\r\n", paramSet.size());
    for(std::unordered_set<const char*>::iterator it = paramSet.begin(); it != paramSet.end(); ++it) {
        printf("'%s'\r\n", (*it));
    }
    printf("====================================================\r\n");
    */

    panelEngine->updateUI(paramSet);
    panelModulation->updateUI(paramSet);
    panelArpAndFilter->updateUI(paramSet);

}


void MainTabs::newNrpnParam(int nrpn, int value) {
    midiInputLabel->setText(String(nrpn), dontSendNotification);
    midiInputLabel2->setText(String(value), dontSendNotification);
}


void MainTabs::setPresetName(const char* presetName) {
    presetNameLabel->setText(String(presetName), dontSendNotification);
}

void MainTabs::setMidiMessageCollector(MidiMessageCollector &midiMessageCollector) {
    this->midiMessageCollector = &midiMessageCollector;
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MainTabs" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="900" initialHeight="710">
  <BACKGROUND backgroundColour="fff0f8ff"/>
  <TABBEDCOMPONENT name="new tabbed component" id="f175981f6c34a740" memberName="tabbedComponent"
                   virtualName="TabbedComponent" explicitFocusOrder="0" pos="0.931% 2.505% 97.851% 96.939%"
                   orientation="top" tabBarDepth="30" initialTab="0">
    <TAB name="Engine" colour="ffe5f9ff" useJucerComp="0" contentClassName="PanelEngine"
         constructorParams="" jucerComponentFile=""/>
    <TAB name="Modulation" colour="ffdeffe4" useJucerComp="0" contentClassName="PanelModulation"
         constructorParams="" jucerComponentFile=""/>
    <TAB name="Arp &amp; Filter" colour="fffffade" useJucerComp="0" contentClassName="PanelArpAndFilter"
         constructorParams="" jucerComponentFile=""/>
  </TABBEDCOMPONENT>
  <LABEL name="midi input label" id="f77b232960a175fb" memberName="midiInputLabel"
         virtualName="" explicitFocusOrder="0" pos="552 8 48 16" textCol="ff0000ff"
         edTextCol="ff000000" edBkgCol="0" labelText="0" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <LABEL name="midi input label" id="f5bde9938974ba9f" memberName="midiInputLabel2"
         virtualName="" explicitFocusOrder="0" pos="616 8 48 16" textCol="ff0000ff"
         edTextCol="ff000000" edBkgCol="0" labelText="0" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="pull button" id="9da85c0691256028" memberName="pullButton"
              virtualName="" explicitFocusOrder="0" pos="74R 8 55 24" buttonText="Pull"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="preset name label" id="4201f054ae2edbe" memberName="presetNameLabel"
         virtualName="" explicitFocusOrder="0" pos="288 8 200 32" tooltip="Click to edit"
         edTextCol="ff000000" edBkgCol="0" labelText="preset" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default font"
         fontsize="25.899999999999998579" bold="1" italic="0" justification="33"/>
  <TEXTBUTTON name="push button" id="52c3034a926a2609" memberName="pushButton"
              virtualName="" explicitFocusOrder="0" pos="156R 8 55 24" buttonText="Push"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
