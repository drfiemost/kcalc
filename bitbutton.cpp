/*
Copyright (C) 2012 - 2013 Evan Teran
                          evan.teran@gmail.com

Copyright (C) 2006        Michel Marti
                          mma@objectxp.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "bitbutton.h"
#include <QApplication>

#include "bitbutton.moc"

//------------------------------------------------------------------------------
// Name: BitButton
// Desc: constructor
//------------------------------------------------------------------------------
BitButton::BitButton(QWidget *parent) : QAbstractButton(parent), on_(false) , over_(false) {

	// too many bits for tab focus
	setFocusPolicy(Qt::ClickFocus);

	// size button by font
	QSize size = fontMetrics().size(0, QLatin1String("M"));
	
	if (size.width() < size.height()) {
		size.setHeight(size.width());
	} else {
		size.setWidth(size.height());
	}
	
	setFixedSize(size.expandedTo(QApplication::globalStrut()));

	this->setAttribute(Qt::WA_Hover, true);
}

//------------------------------------------------------------------------------
// Name: isOn
// Desc: returns true if this bit-button is "on"
//------------------------------------------------------------------------------
bool BitButton::isOn() const {

	return on_;
}

//------------------------------------------------------------------------------
// Name: setOn
// Desc: changes the "on" value for the bitset
//------------------------------------------------------------------------------
void BitButton::setOn(bool value) {

	on_ = value;
	update();
}

//------------------------------------------------------------------------------
// Name: enterEvent
// Desc: sets to true the "over" variable on Enter event
//------------------------------------------------------------------------------
void BitButton::enterEvent(QEvent * event) {

	if (event->type() == QEvent::Enter) {
		over_ = true;
		update();
	}
}

//------------------------------------------------------------------------------
// Name: leaveEvent
// Desc: sets to false the "over" variable on Leave event
//------------------------------------------------------------------------------
void BitButton::leaveEvent(QEvent * event) {

	if (event->type() == QEvent::Leave) {
		over_ = false;
		update();
	}
}
