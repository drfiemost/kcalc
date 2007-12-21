/*
    KCalc, a scientific calculator for the X window system using the
    Qt widget libraries, available at no cost at http://www.troll.no

    Copyright (C) 2004-2005 Klaus Niederkruger
                       kniederk@ulb.ac.be

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#ifndef _KCALC_BUTTON_H
#define _KCALC_BUTTON_H

#include <QHash>
#include <QLabel>
#include <kpushbutton.h>

// The class KCalcButton is an overridden KPushButton. It offers extra
// functionality e.g. labels can be richtext, and the button can be
// told to display its shortcuts in the label, but the most important
// thing is that the button may have several modes with corresponding
// labels and tooltips. When one switches modes, the corresponding
// label is displayed.


enum ButtonModeFlags {ModeNormal = 0, ModeInverse = 1, ModeHyperbolic = 2};


// Each kcalc button can be in one of several modes.
// The following class describes label, tooltip etc. for each mode...
class ButtonMode
{
public:
  ButtonMode(void) {}
  ButtonMode(const QString &label,
             const QString &tooltip,
             bool is_label_richtext,
             const KIcon &icon)
      : is_label_richtext(is_label_richtext), tooltip(tooltip), icon(icon)
  {
    if (is_label_richtext)
      this->label = "<qt type=\"page\"><center>" + label + "</center></qt>";
    else
      this->label = label;
  }

  QString label;
  bool is_label_richtext;
  QString tooltip;
  KIcon icon;
};


class KCalcButton : public KPushButton
{
Q_OBJECT

public:
 KCalcButton(QWidget *parent); 
 KCalcButton(const QString &label, QWidget *parent,
	     const QString &tooltip = QString());

 void addMode(ButtonModeFlags mode, const QString &label,
	      const QString &tooltip, bool is_label_richtext = false,
              const KIcon &icon = KIcon());


 virtual QSize sizeHint() const; // reimp

 void setText(const QString &text); // reimp
 void setToolTip(const QString &tip); // reimp

public slots: 
  void slotSetMode(ButtonModeFlags mode, bool flag); 
  void slotSetAccelDisplayMode(bool flag);

protected:
  virtual void paintEvent(QPaintEvent *p);

  bool _show_shortcut_mode;

  ButtonModeFlags _mode_flags;

  QHash<ButtonModeFlags, ButtonMode> _mode;
  QLabel _label;
};

#endif  // _KCALC_BUTTON_H
