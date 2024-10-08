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

#ifndef BITBUTTON_H_20120104_
#define BITBUTTON_H_20120104_

#include <QAbstractButton>

class BitButton : public QAbstractButton {
	Q_OBJECT

public:
	explicit BitButton(QWidget *parent = 0);
	bool isOn() const;
	void setOn(bool value);

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	void enterEvent(QEvent * event) override;
	void leaveEvent(QEvent * event) override;
	bool on_;
	bool over_;
};

#endif
