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

#ifndef SLIDERPFM2_H_INCLUDED
#define SLIDERPFM2_H_INCLUDED

#include "JuceHeader.h"


class SliderPfm2 : public  Slider
{
public:
	SliderPfm2(const String& componentName) : Slider(componentName) {}
	~SliderPfm2() {}

    void mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel) override
    {
        if (!isEnabled() || !isVisible()) {
            return;
        }
    	if (wheel.deltaY != 0) {
			float y = getValue() + (wheel.deltaY > 0 ? getInterval() : -1 * getInterval() ) * (wheel.isReversed ? -1.0f : 1.0f);

			setValue (y);
    	}
    }



private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderPfm2)

};



class SliderPfm2Always2Decimals : public  SliderPfm2
{
public:
	SliderPfm2Always2Decimals(const String& componentName) : SliderPfm2(componentName) {}
	~SliderPfm2Always2Decimals(){ }


	String getTextFromValue (double v)
	{
		return String (v, 2) + getTextValueSuffix();
	}
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderPfm2Always2Decimals)


};



#endif  // SLIDERPFM2_H_INCLUDED
