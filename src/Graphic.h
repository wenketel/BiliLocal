/*=======================================================================
*
*   Copyright (C) 2013 Lysine.
*
*   Filename:    Graphic.h
*   Time:        2013/10/19
*   Author:      Lysine
*
*   Lysine is a student majoring in Software Engineering
*   from the School of Software, SUN YAT-SEN UNIVERSITY.
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.

*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
=========================================================================*/

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <QtGui>
#include <QtCore>
#include "Utils.h"
#include "Printer.h"

class Graphic
{
public:
	virtual bool move(qint64 time)=0;
	virtual void draw(QPainter *painter)=0;
	virtual bool intersects(Graphic *other)=0;
	virtual ~Graphic();
	int getMode(){return mode;}

protected:
	int mode=0;
	QPixmap cache;
};

class Mode1:public Graphic
{
public:
	Mode1(const Comment &comment,QList<Graphic *> &current,QSize size);
	bool move(qint64 time);
	void draw(QPainter *painter);
	bool intersects(Graphic *other);

private:
	QRectF rect;
	double speed;
};

class Mode4:public Graphic
{
public:
	Mode4(const Comment &comment,QList<Graphic *> &current,QSize size);
	bool move(qint64 time);
	void draw(QPainter *painter);
	bool intersects(Graphic *other);

private:
	QRectF rect;
	double life;
};

class Mode5:public Graphic
{
public:
	Mode5(const Comment &comment,QList<Graphic *> &current,QSize size);
	bool move(qint64 time);
	void draw(QPainter *painter);
	bool intersects(Graphic *other);

private:
	QRectF rect;
	double life;
};

#endif // GRAPHIC_H
