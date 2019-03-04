/*  This file is part of TSRE5.
 *
 *  TSRE5 - train sim game engine and MSTS/OR Editors. 
 *  Copyright (C) 2016 Piotr Gadecki <pgadecki@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *
 *  See LICENSE.md or https://www.gnu.org/licenses/gpl.html
 */

#ifndef PLATFORMOBJ_H
#define	PLATFORMOBJ_H

#include "WorldObj.h"
#include <QString>
#include "FileBuffer.h"

class TrackItemObj;
class OglObj;

class PlatformObj : public WorldObj  {
public:
    PlatformObj();
    PlatformObj(const PlatformObj& o);
    WorldObj* clone();
    virtual ~PlatformObj();
    void load(int x, int y);
    bool allowNew();
    void set(int sh, FileBuffer* val);
    void set(QString sh, FileBuffer* data);
    void save(QTextStream* out);
    bool select(int value);
    bool isTrackItem();
    void initTrItems(float* tpos);
    void translate(float px, float py, float pz);
    void setPosition(int x, int z, float* p);
    QString getStationName();
    QString getPlatformName();
    int getPlatformMinWaitingTime();
    int getPlatformNumPassengersWaiting();
    void setStationName(QString name);
    void setPlatformName(QString name);
    void setPlatformMinWaitingTime(int val);
    void setPlatformNumPassengersWaiting(int val);
    bool getSideLeft();
    bool getSideRight();
    bool getDisabled();
    void setSideLeft(bool val);
    void setSideRight(bool val);
    void setDisabled(bool val);
    float getLength();
    void deleteTrItems();
    int getTrackBegItemId();
    int getDefaultDetailLevel();
    void render(GLUU* gluu, float lod, float posx, float posz, float* playerW, float* target, float fov, int selectionColor, int renderMode);
private:
    unsigned int platformData = 0;
    int trItemId[4];
    int trItemIdCount = 0;
    TrackItemObj* spointer3d = nullptr;
    TrackItemObj* spointer3dSelected = nullptr;
    OglObj* line = nullptr;
    float* drawPositionB = nullptr;
    float* drawPositionE = nullptr;
    int rotB = 0;
    int rotE = 0;
    int selectionValue = 0;
    void renderTritems(GLUU* gluu, int selectionColor);
    void makelineShape();
};


#endif	/* PLATFORMOBJ_H */

