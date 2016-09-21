/*  This file is part of TSRE5.
 *
 *  TSRE5 - train sim game engine and MSTS/OR Editors. 
 *  Copyright (C) 2016 Piotr Gadecki <pgadecki@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *
 *  See LICENSE.md or https://www.gnu.org/licenses/gpl.html
 */

#ifndef SPEEDPOSTOBJ_H
#define	SPEEDPOSTOBJ_H

#include "WorldObj.h"
#include <QString>
#include "FileBuffer.h"
#include <vector>

class TrackItemObj;
class OglObj;

class SpeedpostObj : public WorldObj  {
public:
    SpeedpostObj();
    SpeedpostObj(const SpeedpostObj& orig);
    virtual ~SpeedpostObj();
    void load(int x, int y);
    void set(QString sh, int val);
    void set(int sh, FileBuffer* val);
    void set(QString sh, QString val);
    void set(QString sh, FileBuffer* data);
    bool allowNew();
    bool isTrackItem();
    void initTrItems(float* tpos);
    void deleteTrItems();
    void save(QTextStream* out);
    QString getSpeedpostType();
    float getSpeed();
    float getNumber();
    void setSpeed(float val);
    void setNumber(float val);
    void flip(bool flipShape);
    int getDefaultDetailLevel();
    void render(GLUU* gluu, float lod, float posx, float posz, float* playerW, float* target, float fov, int selectionColor);
private:
    int speedPostId = -1;
    int speedPostType = -1;
    QString speedDigitTex;
    float* speedSignShape;
    float speedTextSize[3];
    //int *trItemId = NULL;
    std::vector<int> trItemId;
    //int trItemIdCount = 0;
    TrackItemObj* pointer3d = NULL;
    float* drawPosition = NULL;
    OglObj* drawLine = NULL;
    
    void renderTritems(GLUU* gluu, int selectionColor);
    float* drawPositions = NULL;
    int selectionValue = 0;
    bool getSimpleBorder(float* border);
    bool getBoxPoints(QVector<float> &points);
};

#endif	/* SPEEDPOSTOBJ_H */

