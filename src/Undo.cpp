/*  This file is part of TSRE5.
 *
 *  TSRE5 - train sim game engine and MSTS/OR Editors. 
 *  Copyright (C) 2016 Piotr Gadecki <pgadecki@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *
 *  See LICENSE.md or https://www.gnu.org/licenses/gpl.html
 */

#include "Undo.h"
#include <QDebug>
#include "TerrainLib.h"
#include "TexLib.h"
#include "GLMatrix.h"
#include <QDateTime>
#include "WorldObj.h"
#include "TDB.h"
#include "Game.h"
#include "Route.h"
#include "GroupObj.h"

UndoState* Undo::currentState = nullptr;
QVector<UndoState*> Undo::undoStates;
unsigned long long int Undo::undoTime;

UndoState::~UndoState(){
    QMapIterator<int, UndoState::TerrainData*> i(terrainData);
    while (i.hasNext()) {
        i.next();
        UndoState::TerrainData* tdata = i.value();
        if(tdata != nullptr)
            delete tdata;
    }
    QMapIterator<int, unsigned char *> i1(texData);
    while (i1.hasNext()) {
        i1.next();
        unsigned char* tdata = i1.value();
        if(tdata != nullptr)
           delete[] tdata;
    }
    QMapIterator<long long int, UndoState::WorldObjInfo*> i2(objData);
    while (i2.hasNext()) {
        i2.next();
        UndoState::WorldObjInfo* tdata = i2.value();
        if(tdata != nullptr){
            //delete tdata->data;
            delete tdata;
        }
    }
    
    if(trackDB != nullptr)
        delete trackDB;
    if(roadDB != nullptr)
        delete roadDB;

    terrainData.clear();
    texData.clear();
    objData.clear();
}

void Undo::Clear(){
    currentState = nullptr;
    for(int i = 0; i < undoStates.size();){
        delete undoStates.last();
        undoStates.removeLast();
    }
}

void Undo::UndoLast(){
    if(currentState != nullptr)
        StateEnd();
    
    if(undoStates.length() == 0){
        qDebug() << "nothing to undo";
        return;
    }
    UndoState *state = undoStates.back();

    qDebug() << "undo";
    
    QMapIterator<int, UndoState::TerrainData*> i(state->terrainData);
    while (i.hasNext()) {
        i.next();
        UndoState::TerrainData* tdata = i.value();
        if(tdata != nullptr)
            Game::terrainLib->fillHeightMap(tdata->x, tdata->z, tdata->data);
    }
    QMapIterator<int, unsigned char *> i1(state->texData);
    while (i1.hasNext()) {
        i1.next();
        unsigned char* tdata = i1.value();
        if(tdata != nullptr){
            //qDebug() << i1.key();
            //qDebug() <<TexLib::mtex[i.key()]->editable;
            TexLib::mtex[i1.key()]->fillData(tdata);
        }
    }
    QMapIterator<long long int, UndoState::WorldObjInfo*> i2(state->objData);
    while (i2.hasNext()) {
        i2.next();
        UndoState::WorldObjInfo* tdata = i2.value();
        if(tdata != nullptr){
            if(tdata->action == "data"){
                if(Game::currentRoute != nullptr){
                    tdata->data->unselect();
                    Game::currentRoute->replaceWorldObjPointer(tdata->obj, tdata->data);
                }
            }
            if(tdata->action == "remove"){
                tdata->data->loaded = true;
                tdata->data->modified = true;
                if(Game::currentRoute != nullptr){
                    tdata->data->unselect();
                    Game::currentRoute->replaceWorldObjPointer(tdata->obj, tdata->data);
                }
            }
            if(tdata->action == "place"){
                if(Game::currentRoute != nullptr)
                    Game::currentRoute->undoPlaceObj(tdata->obj->x, tdata->obj->y, tdata->obj->UiD);
            }
        }
    }
    
    if(state->trackDB != nullptr){
        if(Game::currentRoute != nullptr){
            Game::currentRoute->setTDB(state->trackDB, false);
            state->trackDB = nullptr;
        }
    }
    
    if(state->roadDB != nullptr){
        if(Game::currentRoute != nullptr){
            Game::currentRoute->setTDB(state->roadDB, true);
            state->roadDB = nullptr;
        }
    }
    
    delete state;
    undoStates.removeLast();
}

void Undo::StateBeginIfNotExist(){
    if(currentState != nullptr)
        return;
    
    currentState = new UndoState();
    undoTime = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "undo begin";
}

void Undo::StateBegin(){
    if(currentState != nullptr)
        StateEnd();
    
    currentState = new UndoState();
    undoTime = QDateTime::currentMSecsSinceEpoch();
    //qDebug() << "undo begin";
}

void Undo::StateEndIfLongTime(){
    if(currentState == nullptr)
        return;
    unsigned long long int timeNow = QDateTime::currentMSecsSinceEpoch();
    
    if(undoTime < timeNow - 2000)
        StateEnd();
    
}

void Undo::StateEnd(){
    if(currentState != nullptr){
        if(currentState->modified == true){
            undoStates.push_back(currentState);
            if(undoStates.size() > 50){
                delete undoStates.first();
                undoStates.removeFirst();
            }
        } else {
            delete currentState;
        }
        
    }
    currentState = nullptr;
    //qDebug() << "undo end";
}

void Undo::PushTerrainHeightMap(int x, int z, float** data, int samples){
    if(currentState == nullptr)
        return;
    
    samples += 1;
    UndoState::TerrainData * tdata = currentState->terrainData[x*10000+z];
    if(tdata == nullptr){
        currentState->terrainData[x*10000+z] = new UndoState::TerrainData();
        tdata = currentState->terrainData[x*10000+z];
        tdata->x = x;
        tdata->z = z;
        for (int i = 0; i < samples; i++)
            for (int j = 0; j < samples; j++) {
                    tdata->data[i*samples+j] = data[i][j];
            }
        currentState->modified = true;
    }
    return;
}

void Undo::PushTextureData(int id, unsigned char* data, unsigned int size){
    if(currentState == nullptr)
        return;
    
    unsigned char * tdata = currentState->texData[id];
    if(tdata == nullptr){
        currentState->texData[id] = new unsigned char[size];
        tdata = currentState->texData[id];
        memcpy(tdata, data, size);
        currentState->modified = true;
    }
    return;
}

void Undo::SinglePushWorldObjData(WorldObj* obj){
    StateBegin();
    PushWorldObjData(obj);
    StateEnd();
}

void Undo::PushGameObjData(GameObj* obj){
    if(currentState == nullptr)
        return;
    if(obj == nullptr)
        return;
    if(obj->typeObj != WorldObj::worldobj)
        return;
    WorldObj* wobj = (WorldObj*)obj;
    
    PushWorldObjData(wobj);
}

void Undo::PushWorldObjData(WorldObj* obj){
    if(currentState == nullptr)
        return;
    if(obj == nullptr)
        return;

    if(obj->typeID == obj->groupobject) {
        GroupObj *gobj = (GroupObj*)obj;
        for(int i = 0; i < gobj->objects.size(); i++ ){
            PushWorldObjDataInfo(gobj->objects[i]);
        }
        return;
    } else {
        PushWorldObjDataInfo(obj);
    }
}

void Undo::PushWorldObjDataInfo(WorldObj* obj){
    UndoState::WorldObjInfo * tdata = currentState->objData[(long long int)obj];
    if(tdata == nullptr){
        currentState->objData[(long long int)obj] = new UndoState::WorldObjInfo();
        tdata = currentState->objData[(long long int)obj];
        tdata->obj = obj;
        tdata->data = obj->clone();
        tdata->data->unselect();
        tdata->action = "data";
        currentState->modified = true;
     }
}

void Undo::PushWorldObjRemoved(WorldObj* obj){
    if(currentState == nullptr)
        return;
    if(obj == nullptr)
        return;
    if(obj->typeObj != WorldObj::worldobj)
        return;
    
    UndoState::WorldObjInfo * tdata = currentState->objData[(long long int)obj];
    if(tdata == nullptr){
        currentState->objData[(long long int)obj] = new UndoState::WorldObjInfo();
        tdata = currentState->objData[(long long int)obj];
        tdata->obj = obj;
        tdata->data = obj->clone();
        tdata->data->unselect();
        tdata->action = "remove";
        currentState->modified = true;
    }
}

void Undo::PushWorldObjPlaced(WorldObj* obj){
    if(currentState == nullptr)
        return;
    if(obj == nullptr)
        return;
    if(obj->typeObj != WorldObj::worldobj)
        return;
    
    UndoState::WorldObjInfo * tdata = currentState->objData[(long long int)obj];
    if(tdata == nullptr){
        currentState->objData[(long long int)obj] = new UndoState::WorldObjInfo();
        tdata = currentState->objData[(long long int)obj];
        tdata->obj = obj;
        tdata->action = "place";
        currentState->modified = true;
    }
}

void Undo::PushTrackDB(TDB* tdb, bool road){
    if(currentState == nullptr)
        return;
    
    if(road && currentState->roadDB == nullptr ){
        currentState->roadDB = new TDB(*tdb);
        currentState->modified = true;
    } else if(currentState->trackDB == nullptr) {
        currentState->trackDB = new TDB(*tdb);
        currentState->modified = true;
    }
}