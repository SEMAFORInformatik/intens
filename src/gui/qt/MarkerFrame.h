
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#ifndef MARKER_FRAME_H
#define MARKER_FRAME_H

class GuiQtImage;

class MarkerFrame: public QWidget {
 public:
    MarkerFrame( QWidget *parent, int height, GuiQtImage *img ): QWidget( parent ){
	setMinimumSize(4,height);
	setMaximumSize(4,height);
	this -> img = img;
    }
    virtual void paintEvent( QPaintEvent *e ) {
	img -> drawMarker();
    }
 private:
    GuiQtImage *img;
};

#endif