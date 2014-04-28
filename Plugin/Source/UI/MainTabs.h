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

#ifndef __JUCE_HEADER_707E53F8D6330206__
#define __JUCE_HEADER_707E53F8D6330206__

//[Headers]     -- You can add your own extra header files here --

struct Nrpn {
	char paramMSB;
	char paramLSB;
	char valueMSB;
	char valueLSB;
};

class PanelEngine;
class PanelModulation;
class PanelArpAndFilter;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MainTabs  : public Component,
                  public ButtonListener
{
public:
    //==============================================================================
    MainTabs ();
    ~MainTabs();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void handleIncomingMidiBuffer(MidiBuffer &buffer, int numberOfSamples);
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    int midiInfo;
    struct Nrpn currentNrpn;
    PanelEngine* panelEngine;
    PanelModulation* panelModulation;
    PanelArpAndFilter* panelArpAndFilter;
    MidiBuffer eventsToAdd;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<TabbedComponent> tabbedComponent;
    ScopedPointer<Label> midiInputLabel;
    ScopedPointer<Label> midiInputLabel2;
    ScopedPointer<TextButton> pullButton;
    ScopedPointer<TextButton> pushButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainTabs)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_707E53F8D6330206__