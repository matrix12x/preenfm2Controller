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


#include "../JuceLibraryCode/JuceHeader.h"
#include "EnveloppeAbstract.h"

//==============================================================================
EnveloppeAbstract::EnveloppeAbstract()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    this->scaleX = 1;
    this->scaleY = 1;
    this->draggingPointIndex = -1;
    this->overPointIndex = -1;
    this->xMax = 4.0;
}

EnveloppeAbstract::~EnveloppeAbstract()
{
}

void EnveloppeAbstract::updatePointPositions() {
    float allX = 0;
    for (int p=0; p < pointList.size(); p++) {
        allX += pointList[p].get()->getX();
    }

    allX = allX < this->xMax ? this->xMax : allX;
    scaleX = (getWidth() - MARGIN_LEFT - MARGIN_RIGHT) / allX;
    // y is always 0 to 1
    scaleY = getHeight() - MARGIN_TOP - MARGIN_BOTTOM;
    float offsetX = MARGIN_LEFT;
    for (int k = 0; k < pointList.size(); k++ ) {
        float xPositionOnScreen = offsetX + pointList[k].get()->getX() * scaleX;
        float yPositionOnScreen = getHeight() - pointList[k].get()->getY() * scaleY - MARGIN_BOTTOM;
        pointList[k].get()->setPositionOnScreen(xPositionOnScreen, yPositionOnScreen);
        offsetX = xPositionOnScreen;
    }
}

void EnveloppeAbstract::paint (Graphics& g)
{
    updatePointPositions();

    float lineX = scaleX;
    g.setColour (Colours::grey);
    while (lineX < getWidth() - MARGIN_RIGHT) {
        g.drawVerticalLine((int)(lineX + MARGIN_LEFT), MARGIN_TOP, getHeight()-MARGIN_BOTTOM);
        lineX += scaleX;
    }
    g.drawVerticalLine(MARGIN_LEFT, MARGIN_TOP, getHeight() - MARGIN_BOTTOM);
    g.drawVerticalLine(getWidth() - MARGIN_RIGHT, MARGIN_TOP, getHeight() - MARGIN_BOTTOM);
    g.drawHorizontalLine(MARGIN_TOP, MARGIN_LEFT, getWidth() - MARGIN_RIGHT);
    g.drawHorizontalLine(getHeight() - MARGIN_BOTTOM, MARGIN_LEFT, getWidth() - MARGIN_RIGHT);

    // Draw main enveloppe shapr
    g.setColour (Colours::whitesmoke);
	Path path;
	path.startNewSubPath(pointList[0].get()->getPositionOnScreenX(),
		pointList[0].get()->getPositionOnScreenY() );
	for (int p = 1; p < pointList.size() ; p++) {
		path.lineTo(pointList[p].get()->getPositionOnScreenX(),
			pointList[p].get()->getPositionOnScreenY());
    }
	g.setColour(Colours::whitesmoke);
	g.strokePath(path, PathStrokeType(1.8f, PathStrokeType::beveled, PathStrokeType::rounded));

	// Back to bottom
	path.lineTo(pointList[pointList.size()-1].get()->getPositionOnScreenX(),
		pointList[0].get()->getPositionOnScreenY());

	// Back to first point
	path.lineTo(pointList[0].get()->getPositionOnScreenX(),
		pointList[0].get()->getPositionOnScreenY());
	g.setColour(Colour(0x33bbbbbb));
	g.fillPath(path);




    for (int p = 0; p < pointList.size(); p++) {
        if (draggingPointIndex == p) {
            g.setColour (Colours::yellow);
            g.fillEllipse((int)pointList[p].get()->getPositionOnScreenX() - CIRCLE_RAY,
                    (int)pointList[p].get()->getPositionOnScreenY() - CIRCLE_RAY ,
                    CIRCLE_RAY*2, CIRCLE_RAY*2);
//            g.setColour (Colours::black);
            if (!pointList[p].get()->isYConstrained()) {
            	g.drawVerticalLine((int)pointList[p].get()->getPositionOnScreenX(), MARGIN_TOP, getHeight() - MARGIN_BOTTOM);
            }
            if (!pointList[p].get()->isXConstrained()) {
            	g.drawHorizontalLine((int)pointList[p].get()->getPositionOnScreenY(), MARGIN_LEFT, getWidth() - MARGIN_RIGHT);
            }
        } else {
            if (overPointIndex == p) {
                g.setColour (Colours::yellow);
                if (!pointList[p].get()->isYConstrained()) {
                	g.drawVerticalLine((int)pointList[p].get()->getPositionOnScreenX(), MARGIN_TOP, getHeight() - MARGIN_BOTTOM);
                }
                if (!pointList[p].get()->isXConstrained()) {
                	g.drawHorizontalLine((int)pointList[p].get()->getPositionOnScreenY(), MARGIN_LEFT, getWidth() - MARGIN_RIGHT);
                }
            } else {
                g.setColour (Colours::grey);
            }
            g.drawEllipse((int)pointList[p].get()->getPositionOnScreenX() - CIRCLE_RAY,
                    (int)pointList[p].get()->getPositionOnScreenY() - CIRCLE_RAY ,
                    CIRCLE_RAY*2, CIRCLE_RAY*2, 1);
        }
    }

}


void EnveloppeAbstract::mouseMove(const MouseEvent &event)  {

    for (int p = pointList.size() - 1; p >= 0; p--) {
        if (abs(event.x - pointList[p].get()->getPositionOnScreenX()) < (CIRCLE_RAY+1)
                && abs(event.y - pointList[p].get()->getPositionOnScreenY()) < (CIRCLE_RAY+1)) {
            if (overPointIndex != p) {
                overPointIndex = p;
                setMouseCursor(MouseCursor::PointingHandCursor);
                repaint();
            }
            return;
        }
    }
    if (overPointIndex != -1) {
        overPointIndex = -1;
        setMouseCursor(MouseCursor::NormalCursor);
        repaint();
    }
    return;

}

void EnveloppeAbstract::mouseDown (const MouseEvent &event)  {
    for (int p = pointList.size() - 1; p >= 0; p--) {
        
        if (abs(event.x - pointList[p].get()->getPositionOnScreenX()) < 5
                && abs(event.y - pointList[p].get()->getPositionOnScreenY()) < 5) {
            draggingPointIndex = p  ;
            startDragX = event.x;
            startDragY = event.y;
            repaint();
            return;
        }
    }
}

void EnveloppeAbstract::mouseUp (const MouseEvent &event)  {
    if (draggingPointIndex != -1) {
        Desktop::setMousePosition(Point<int>(getScreenX() + pointList[draggingPointIndex].get()->getPositionOnScreenX(),
                getScreenY() + pointList[draggingPointIndex].get()->getPositionOnScreenY()));
        draggingPointIndex = -1;
    }
    setMouseCursor(MouseCursor::NormalCursor);
    repaint();
}

void EnveloppeAbstract::mouseDrag (const MouseEvent &event)  {
    if (draggingPointIndex == -1) {
        return;
    }
    float slowMoveRatio = (event.mods.isCtrlDown() || event.mods.isRightButtonDown() ? .05f : 1.0f);
    float oldVX = pointList[draggingPointIndex].get()->getX();
    pointList[draggingPointIndex].get()->setX(pointList[draggingPointIndex].get()->getX() + (float)(event.x - startDragX) / scaleX * slowMoveRatio);
    float oldVY = pointList[draggingPointIndex].get()->getY();
    pointList[draggingPointIndex].get()->setY(pointList[draggingPointIndex].get()->getY() - (float)(event.y - startDragY) / scaleY * slowMoveRatio);
    if (oldVX != pointList[draggingPointIndex].get()->getX()) {
    	// Tell listeners
    	notifyObservers(draggingPointIndex, true);
    	startDragX = event.x;
    }
    if (oldVY != pointList[draggingPointIndex ].get()->getY()) {
    	// Tell listeners
    	notifyObservers(draggingPointIndex, false);
    	startDragY = event.y;
    }
    repaint();
}

void EnveloppeAbstract::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}

int EnveloppeAbstract::getNumberOfPoints() const { return pointList.size(); }
float EnveloppeAbstract::getX(int index) const { return pointList[index].get()->getX(); }
float EnveloppeAbstract::getY(int index) const { return pointList[index].get()->getY(); }
void EnveloppeAbstract::setX(int index, float x) { pointList[index].get()->setX(x); }
void EnveloppeAbstract::setY(int index, float y) { pointList[index].get()->setY(y); }


