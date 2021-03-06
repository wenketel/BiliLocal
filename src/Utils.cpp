/*=======================================================================
*
*   Copyright (C) 2013 Lysine.
*
*   Filename:    Utils.cpp
*   Time:        2013/05/10
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

#include "Utils.h"

QJsonObject Utils::config;

Utils::Site Utils::getSite(QString url)
{
	if(url.startsWith("http://www.bilibili.tv")||url.startsWith("http://bilibili.kankanews.com")){
		return Bilibili;
	}
	else if(url.startsWith("http://www.acfun.tv")||url.startsWith("http://api.acfun.tv")){
		return AcFun;
	}
	else if(url.startsWith("http://comic.letv.com")){
		return Letv;
	}
	else if(url.startsWith("http://api.acplay.net")){
		return AcPlay;
	}
	else{
		return Unknown;
	}
}

void Utils::setCenter(QWidget *widget)
{
	QPoint center;
	QWidget *parent=widget->parentWidget();
	if(parent==NULL){
		center=QApplication::desktop()->screenGeometry(widget).center();
	}
	else if(widget->isWindow()){
		center=parent->geometry().center();
	}
	else{
		center=parent->rect().center();
	}
	QRect rect=widget->geometry();
	rect.moveCenter(center);
	widget->setGeometry(rect);
}

void Utils::setGround(QWidget *widget,QColor color)
{
	widget->setAutoFillBackground(true);
	QPalette palette=widget->palette();
	palette.setColor(QPalette::Window,color);
	widget->setPalette(palette);
}

void Utils::setSelection(QAbstractItemView *view)
{
	QObject::connect(view->selectionModel(),
					 &QItemSelectionModel::selectionChanged,
					 [view](QItemSelection selected){
		if(selected.isEmpty()){
			view->setCurrentIndex(QModelIndex());
		}
	});
}

QString Utils::defaultPath()
{
	QStringList paths=QStandardPaths::standardLocations(QStandardPaths::MoviesLocation);
	return getConfig("/Playing/Path",paths.isEmpty()?QDir::homePath():paths.last());
}

QString Utils::defaultFont(bool monospace)
{
	if(monospace){
#ifdef Q_OS_LINUX
		return "文泉驿等宽正黑";
#endif
#ifdef Q_OS_WIN32
#ifdef Q_CC_MSVC
		return QString::fromLocal8Bit("黑体");
#else
		return "黑体";
#endif
#endif
#ifdef Q_OS_MAC
		return "";
#endif
	}
	else{
#ifdef Q_OS_LINUX
		return "文泉驿正黑";
#endif
#ifdef Q_OS_WIN32
#ifdef Q_CC_MSVC
		return QString::fromLocal8Bit("微软雅黑");
#else
		return "微软雅黑";
#endif
#endif
#ifdef Q_OS_MAC
		return "华文黑体";
#endif
	}
}

static QString decodeXml(QString string)
{
	string.replace("&lt;","<");
	string.replace("&gt;",">");
	string.replace("&amp;","&");
	string.replace("&quot;","\"");
	QString fixed;
	for(QChar c:string){
		if(c>=' '){
			fixed+=c;
		}
	}
	return fixed;
}

QList<Comment> Utils::parseComment(QByteArray data,Site site,bool isSync)
{
	QList<Comment> list;
	switch(site){
	case Bilibili:
	{
		QStringList l=QString(data).split("<d p=\"");
		l.removeFirst();
		for(QString &item:l){
			Comment comment;
			int sta=0;
			int len=item.indexOf("\"");
			QStringList args=item.mid(sta,len).split(',');
			sta=item.indexOf(">")+1;
			len=item.indexOf("<",sta)-sta;
			comment.time=args[0].toDouble()*1000;
			comment.date=args[4].toInt();
			comment.mode=args[1].toInt();
			comment.font=args[2].toInt();
			comment.color=args[3].toInt();
			comment.sender=args[6];
			comment.string=decodeXml(item.mid(sta,len));
			list.append(comment);
			if(!isSync&&list.size()%50){
				qApp->processEvents();
			}
		}
		break;
	}
	case AcFun:
	{
		QJsonArray a=QJsonDocument::fromJson(data).array();
		for(QJsonValue i:a){
			Comment comment;
			QJsonObject item=i.toObject();
			QStringList args=item["c"].toString().split(',');
			comment.time=args[0].toDouble()*1000;
			comment.date=args[5].toInt();
			comment.mode=args[2].toInt();
			comment.font=args[3].toInt();
			comment.color=args[1].toInt();
			comment.sender=args[4];
			comment.string=item["m"].toString();
			list.append(comment);
			if(!isSync&&list.size()%50){
				qApp->processEvents();
			}
		}
		break;
	}
	case AcPlay:
	{
		QJsonArray a=QJsonDocument::fromJson(data).object()["Comments"].toArray();
		for(QJsonValue i:a){
			Comment comment;
			QJsonObject item=i.toObject();
			comment.time=item["Time"].toDouble()*1000;
			comment.date=item["Timestamp"].toInt();
			comment.mode=item["Mode"].toInt();
			comment.font=25;
			comment.color=item["Color"].toInt();
			comment.sender=QString::number(item["UId"].toInt());
			comment.string=item["Message"].toString();
			list.append(comment);
			if(!isSync&&list.size()%50){
				qApp->processEvents();
			}
		}
		break;
	}
	case AcfunLocalizer:
	{
		QStringList l=QString(data).split("<l i=\"");
		l.removeFirst();
		for(QString &item:l){
			Comment comment;
			int sta=0;
			int len=item.indexOf("\"");
			QStringList args=item.mid(sta,len).split(',');
			sta=item.indexOf("<![CDATA[")+9;
			len=item.indexOf("]]>",sta)-sta;
			comment.time=args[0].toDouble()*1000;
			comment.date=args[5].toInt();
			comment.mode=args[3].toInt();
			comment.font=args[1].toInt();
			comment.color=args[2].toInt();
			comment.sender=args[4];
			comment.string=decodeXml(item.mid(sta,len));
			list.append(comment);
			if(!isSync&&list.size()%50){
				qApp->processEvents();
			}
		}
		break;
	}
	default:
		break;
	}
	return list;
}

void Utils::loadConfig()
{
	QFile conf("./Config.txt");
	conf.open(QIODevice::ReadOnly|QIODevice::Text);
	config=QJsonDocument::fromJson(conf.readAll()).object();
	conf.close();
}

void Utils::saveConfig()
{
	QFile conf("./Config.txt");
	conf.open(QIODevice::WriteOnly|QIODevice::Text);
	conf.write(QJsonDocument(config).toJson());
	conf.close();
}

