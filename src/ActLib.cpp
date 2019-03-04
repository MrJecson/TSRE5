/*  This file is part of TSRE5.
 *
 *  TSRE5 - train sim game engine and MSTS/OR Editors. 
 *  Copyright (C) 2016 Piotr Gadecki <pgadecki@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *
 *  See LICENSE.md or https://www.gnu.org/licenses/gpl.html
 */

#include "ActLib.h"
#include "Activity.h"
#include "Service.h"
#include "Traffic.h"
#include "Path.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include "Game.h"

int ActLib::jestact = 0;
int ActLib::jestservice = 0;
int ActLib::jesttraffic = 0;
int ActLib::jestpath = 0;
QHash<int, Activity*> ActLib::Act;
QHash<int, Service*> ActLib::Services;
QHash<int, Traffic*> ActLib::Traffics;
QHash<int, Path*> ActLib::Paths;
QHash<QString, QVector<int>> ActLib::route;
 
ActLib::ActLib() {
}

ActLib::~ActLib() {
}

int ActLib::GetAct(QString path, QString name){
    QString pathid = (path + "/" + name).toLower();
    pathid.replace("\\", "/");
    pathid.replace("//", "/");
    //qDebug() << pathid;
    QHashIterator<int, Activity*> i(Act);
    while (i.hasNext()) {
        i.next();
        if(i.value() == nullptr) continue;
        if (i.value()->pathid.length() == pathid.length())
            if (i.value()->pathid == pathid) {
                i.value()->ref++;
                qDebug() <<"actid "<< pathid;
                return (int)i.key();
            }
    }
    return -1;
}

int ActLib::AddAct(QString path, QString name, bool nowe) {
    QString pathid = (path + "/" + name).toLower();
    pathid.replace("\\", "/");
    pathid.replace("//", "/");
    //qDebug() << pathid;
    QHashIterator<int, Activity*> i(Act);
    while (i.hasNext()) {
        i.next();
        if(i.value() == nullptr) continue;
        if (i.value()->pathid.length() == pathid.length())
            if (i.value()->pathid == pathid) {
                i.value()->ref++;
                qDebug() <<"actid "<< pathid;
                return (int)i.key();
            }
    }
    qDebug() << "Nowy " << jestact << " act: " << pathid;
    Act[jestact] = new Activity(pathid, path, name, nowe);
    if(!nowe)
        route[Act[jestact]->header->routeid].push_back(jestact);
    return jestact++;
}

void ActLib::UpdateServiceChanges(QString serviceNameId){
    QHashIterator<int, Activity*> i(Act);
    while (i.hasNext()) {
        i.next();
        if(i.value() == nullptr) continue;
        i.value()->updateService(serviceNameId);
    }
}

int ActLib::AddService(QString path, QString name, bool nowe) {
    QString pathid = (path + "/" + name).toLower();
    pathid.replace("\\", "/");
    pathid.replace("//", "/");
    //qDebug() << pathid;
    QHashIterator<int, Service*> i(Services);
    while (i.hasNext()) {
        i.next();
        if(i.value() == nullptr) continue;
        if (i.value()->pathid.length() == pathid.length())
            if (i.value()->pathid == pathid) {
                //i.value()->ref++;
                qDebug() <<"serviceid "<< pathid;
                return (int)i.key();
            }
    }
    qDebug() << "Nowy " << jestservice << " service: " << pathid;
    Services[jestservice] = new Service(path, name, nowe);
    return jestservice++;
}

int ActLib::AddTraffic(QString path, QString name, bool nowe) {
    QString pathid = (path + "/" + name).toLower();
    pathid.replace("\\", "/");
    pathid.replace("//", "/");
    //qDebug() << pathid;
    QHashIterator<int, Traffic*> i(Traffics);
    while (i.hasNext()) {
        i.next();
        if(i.value() == nullptr) continue;
        if (i.value()->pathid.length() == pathid.length())
            if (i.value()->pathid == pathid) {
                i.value()->ref++;
                qDebug() <<"trafficid "<< pathid;
                return (int)i.key();
            }
    }
    qDebug() << "Nowy " << jesttraffic << " traffic: " << pathid;
    Traffics[jesttraffic] = new Traffic(path, name, nowe);
    return jesttraffic++;
}

bool ActLib::IsServiceInUse(QString n){
    QHashIterator<int, Activity*> i(Act);
    while (i.hasNext()) {
        i.next();
        if(i.value() == nullptr) continue;
        if(i.value()->isServiceInUse(n))
            return true;
    }
    return false;
}

bool ActLib::IsTrafficInUse(QString name){
    QHashIterator<int, Traffic*> i(Traffics);
    while (i.hasNext()) {
        i.next();
        if(i.value() == nullptr) continue;
        if(i.value()->nameId.toLower() == name.toLower())
            return true;
    }
    return false;
}

QVector<QString> ActLib::GetServiceInUseList(QString n){
    QVector<QString> list;
    QHashIterator<int, Activity*> i(Act);
    while (i.hasNext()) {
        i.next();
        if(i.value() == nullptr) continue;
        if(i.value()->isServiceInUse(n)){
            list.push_back(QString("Activity:") + i.value()->header->name);
        }
    }
    return list;
}
Service* ActLib::GetServiceByName(QString name){
    QHashIterator<int, Service*> i(Services);
    while (i.hasNext()) {
        i.next();
        if(i.value() == nullptr) continue;
        if(i.value()->nameId.toLower() == name.toLower())
            return i.value();
    }
    return nullptr;
}

Traffic* ActLib::GetTrafficByName(QString name){
    QHashIterator<int, Traffic*> i(Traffics);
    while (i.hasNext()) {
        i.next();
        if(i.value() == nullptr) continue;
        if(i.value()->nameId.toLower() == name.toLower())
            return i.value();
    }
    return nullptr;
}

Path* ActLib::GetPathByName(QString name){
    QHashIterator<int, Path*> i(Paths);
    while (i.hasNext()) {
        i.next();
        if(i.value() == nullptr) continue;
        if(i.value()->trPathName.toLower() == name.toLower())
            return i.value();
    }
    return nullptr;
}

int ActLib::AddPath(QString path, QString name) {
    QString pathid = (path + "/" + name).toLower();
    pathid.replace("\\", "/");
    pathid.replace("//", "/");
    //qDebug() << pathid;
    QHashIterator<int, Path*> i(Paths);
    while (i.hasNext()) {
        i.next();
        if(i.value() == nullptr) continue;
        ///qDebug() << i.value()->pathid << pathid; 
        if (i.value()->pathid.length() == pathid.length())
            if (i.value()->pathid == pathid) {
                i.value()->ref++;
                qDebug() <<"pathid "<< pathid;
                return (int)i.key();
            }
    }
    qDebug() << "Nowy " << jestpath << " path: " << pathid;
    Paths[jestpath] = new Path(path, name);
    return jestpath++;
}

int ActLib::LoadAllAct(QString gameRoot){
    QString path;
    path = gameRoot + "/routes";
    QDir dir(path);
    qDebug() << path;
    dir.setFilter(QDir::Dirs);
    
    foreach(QString dirFile, dir.entryList()){
        if(dirFile == "." || dirFile == "..")   
            continue;
        
        dirFile += "/activities";
        //qDebug() <<dirFile;
        QDir aDir(path+"/"+dirFile);
        if(!aDir.exists()) continue;
        
        aDir.setFilter(QDir::Files);
        aDir.setNameFilters(QStringList()<<"*.act");
        foreach(QString actfile, aDir.entryList()){
            ActLib::AddAct(path+"/"+dirFile, actfile);
        }
    }
    qDebug() << "loaded";
    return 0;
}

void ActLib::GetUnsavedInfo(QVector<QString>& items){
    Activity * e;
    for (int i = 0; i < ActLib::jestact; i++){
        e = ActLib::Act[i];
        if(e == nullptr) continue;
        if(e->loaded != 1) continue;
        if(e->isUnSaved()){
            items.push_back("[A] " + e->name );
        }
    }
    Service * s;
    for (int i = 0; i < ActLib::jestservice; i++){
        s = ActLib::Services[i];
        if(s == nullptr) continue;
        if(s->loaded != 1) continue;
        if(s->isModified()){
            items.push_back("[S] " + s->name );
        }
    }
    Traffic * t;
    for (int i = 0; i < ActLib::jesttraffic; i++){
        t = ActLib::Traffics[i];
        if(t == nullptr) continue;
        if(t->loaded != 1) continue;
        if(t->isModified()){
            items.push_back("[T] " + t->name );
        }
    }
    Path * p;
    for (int i = 0; i < ActLib::jestpath; i++){
        p = ActLib::Paths[i];
        if(p == nullptr) continue;
        if(p->loaded != 1) continue;
        if(p->isModified()){
            items.push_back("[P] " + p->name );
        }
    }
}

void ActLib::SaveAll(){
    Activity * e;
    for (int i = 0; i < ActLib::jestact; i++){
        e = ActLib::Act[i];
        if(e == nullptr) continue;
        if(e->loaded != 1) continue;
        if(e->isUnSaved()){
            e->save();
        }
    }
    Service * s;
    for (int i = 0; i < ActLib::jestservice; i++){
        s = ActLib::Services[i];
        if(s == nullptr) continue;
        if(s->loaded != 1) continue;
        if(s->isModified()){
            s->save();
        }
    }
    Traffic * t;
    for (int i = 0; i < ActLib::jesttraffic; i++){
        t = ActLib::Traffics[i];
        if(t == nullptr) continue;
        if(t->loaded != 1) continue;
        if(t->isModified()){
            t->save();
        }
    }
    Path * p;
    for (int i = 0; i < ActLib::jestpath; i++){
        p = ActLib::Paths[i];
        if(p == nullptr) continue;
        if(p->loaded != 1) continue;
        if(p->isModified()){
            //p->save();
        }
    }
}