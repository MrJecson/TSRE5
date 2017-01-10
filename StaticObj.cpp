/*  This file is part of TSRE5.
 *
 *  TSRE5 - train sim game engine and MSTS/OR Editors. 
 *  Copyright (C) 2016 Piotr Gadecki <pgadecki@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *
 *  See LICENSE.md or https://www.gnu.org/licenses/gpl.html
 */

#include "StaticObj.h"
#include "SFile.h"
#include "ShapeLib.h"
#include "GLMatrix.h"
#include <math.h>
#include "ParserX.h"
#include "TS.h"
#include "TrackItemObj.h"
#include <QDebug>
#include "Game.h"

StaticObj::StaticObj() {
    this->shape = -1;
    this->loaded = false;
    this->modified = false;
}

bool StaticObj::allowNew(){
    return true;
}

StaticObj::StaticObj(const StaticObj& orig) {
}

StaticObj::~StaticObj() {
}

void StaticObj::load(int x, int y) {
    this->shape = Game::currentShapeLib->addShape(resPath +"/"+ fileName);
    this->shapePointer = Game::currentShapeLib->shape[this->shape];
    this->x = x;
    this->y = y;
    this->position[2] = -this->position[2];
    this->qDirection[2] = -this->qDirection[2];
    this->loaded = true;
    this->size = -1;
    this->skipLevel = 1;
    this->box.loaded = false;
            
    setMartix();
}

void StaticObj::set(QString sh, QString val){
    if (sh == ("filename")) {
        fileName = val;
        return;
    }
    if (sh == ("ref_filename")) {
        fileName = val;
        return;
    }
    WorldObj::set(sh, val);
    return;
}

void StaticObj::set(int sh, FileBuffer* data) {
    if (sh == TS::FileName) {
        data->off++;
        int slen = data->getShort()*2;
        fileName = *data->getString(data->off, data->off + slen);
        data->off += slen;
        return;
    }
    if (sh == TS::NoDirLight) {
        //data->off++;
        return;
    }
    WorldObj::set(sh, data);
    return;
}

void StaticObj::set(QString sh, FileBuffer* data) {
    if (sh == ("filename")) {
        fileName = ParserX::GetString(data);
        return;
    }
    WorldObj::set(sh, data);
    return;
}

void StaticObj::render(GLUU* gluu, float lod, float posx, float posz, float* pos, float* target, float fov, int selectionColor) {
    if (!loaded) return;
    if (shape < 0) return;
    if (jestPQ < 2) return;
    //GLUU* gluu = GLUU::get();
    //if((this.position===undefined)||this.qDirection===undefined) return;
    
    if (size > 0) {
        if ((lod > size + 150)) {
            float v1[2];
            v1[0] = pos[0] - (target[0]);
            v1[1] = pos[2] - (target[2]);
            float v2[2];
            v2[0] = posx;
            v2[1] = posz;
            float iloczyn = v1[0] * v2[0] + v1[1] * v2[1];
            float d1 = sqrt(v1[0] * v1[0] + v1[1] * v1[1]);
            float d2 = sqrt(v2[0] * v2[0] + v2[1] * v2[1]);
            float zz = iloczyn / (d1 * d2);
            if (zz > 0) return;

            float ccos = cos(fov) + zz;
            float xxx = sqrt(2 * d2 * d2 * (1 - ccos));
            //if((ccos > 0) && (xxx > 200+50)) return;
            if ((ccos > 0) && (xxx > size) && (skipLevel == 1)) return;
        }
    } else {
        if (Game::currentShapeLib->shape[shape]->loaded){
            size = Game::currentShapeLib->shape[shape]->size;
            
            this->snapable = this->shapePointer->isSnapable();
            if(snapable)
                this->shapePointer->addSnapablePoints(this->snapablePoints);
        }
    }

    if(Game::viewSnapable)
        if(snapablePoints.size() == 6)
            renderSnapableEndpoints(gluu);  
    
    Mat4::multiply(gluu->mvMatrix, gluu->mvMatrix, matrix);
    gluu->currentShader->setUniformValue(gluu->currentShader->mvMatrixUniform, *reinterpret_cast<float(*)[4][4]> (gluu->mvMatrix));
    
    if(Game::showWorldObjPivotPoints){
        if(pointer3d == NULL){
            pointer3d = new TrackItemObj(1);
            pointer3d->setMaterial(0.9,0.9,0.7);
        }
        pointer3d->render(selectionColor);
    }
    
    if(selectionColor != 0){
        int wColor = (int)(selectionColor/65536);
        int sColor = (int)(selectionColor - wColor*65536)/256;
        int bColor = (int)(selectionColor - wColor*65536 - sColor*256);
        gluu->disableTextures((float)wColor/255.0f, (float)sColor/255.0f, (float)bColor/255.0f, 1);
    } else {
        gluu->enableTextures();
    }
    
    Game::currentShapeLib->shape[shape]->render();
    
    if(selected){
        drawBox();
    }
};

void StaticObj::renderSnapableEndpoints(GLUU* gluu) {
    if (snapableEndPoint == NULL) {
        snapableEndPoint = new OglObj();
        float *punkty = new float[3 * 2];
        int ptr = 0;
        punkty[ptr++] = 0;
        punkty[ptr++] = 0;
        punkty[ptr++] = 0;
        punkty[ptr++] = 0;
        punkty[ptr++] = 30;
        punkty[ptr++] = 0;
        snapableEndPoint->setMaterial(0.0, 1.0, 0.0);
        snapableEndPoint->init(punkty, ptr, snapableEndPoint->V, GL_LINES);
        delete[] punkty;
    }
    
    float vec[3];
    for(int i = 0; i < 6; i+=3){
        Vec3::transformQuat(vec, (float*)&snapablePoints[i], qDirection);
        Vec3::add(vec, vec, position);
        gluu->mvPushMatrix();
        Mat4::translate(gluu->mvMatrix, gluu->mvMatrix, vec);
        gluu->currentShader->setUniformValue(gluu->currentShader->mvMatrixUniform, *reinterpret_cast<float(*)[4][4]> (gluu->mvMatrix));
        snapableEndPoint->render();
        gluu->mvPopMatrix();
    }
}

void StaticObj::insertSnapablePoints(QVector<float>& points){
    if(snapablePoints.size() == 6){
        float vec[3];
        for(int i = 0; i < 6; i+=3){
            Vec3::transformQuat(vec, (float*)&snapablePoints[i], qDirection);
            points.push_back(vec[0]);
            points.push_back(vec[1]);
            points.push_back(vec[2]);
            Vec3::add(vec, vec, position);
            points.push_back(vec[0]);
            points.push_back(vec[1]);
            points.push_back(vec[2]);
            
            points.push_back(qDirection[0]);
            points.push_back(qDirection[1]);
            points.push_back(qDirection[2]);
            points.push_back(qDirection[3]);
        }
    }
    return;
}

bool StaticObj::getSimpleBorder(float* border){
    if (shapePointer == 0) return false;
    if (!shapePointer->loaded)
        return false;
    float* bound = shapePointer->bound;
    border[0] = bound[0];
    border[1] = bound[1];
    border[2] = bound[2];
    border[3] = bound[3];
    border[4] = bound[4];
    border[5] = bound[5];
    return true;
}

bool StaticObj::getBoxPoints(QVector<float>& points){
    if (shapePointer == 0) return false;
    if (!shapePointer->loaded)
        return false;
    return shapePointer->getBoxPoints(points);
}

QString StaticObj::getShapePath(){
    if (!loaded) return "";
    if (shapePointer == 0) return "";
    return shapePointer->pathid+"|"+shapePointer->texPath;
}

int StaticObj::getDefaultDetailLevel(){
    if (!loaded) return 0;
    if (shapePointer == 0) return 0;
    int esdDLevel = shapePointer->esdDetailLevel;
    if(esdDLevel >= 0) return esdDLevel;
    return 0;
}

int StaticObj::getCollisionType(){
    if(this->type == "collideobject"){
        bool enabled = ((this->collideFlags & 2) >> 1) == 0;
        if(enabled){
            if(this->collideFunction == 1 )
                return 2;
            else 
                return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

void StaticObj::setCollisionType(int val){
    this->modified = true;
    if(val < 0){
        if(this->type == "collideobject")
            collideFlags = collideFlags | (2);
        return;
    }
    if(this->type != "collideobject"){
        type = "collideobject";
        typeID = WorldObj::collideobject;
        collideFlags = 32;
    }
    collideFlags = collideFlags & (~2);
    collideFunction = val;
    return;
}

void StaticObj::removeCollisions(){
    if(type != "static"){
        type = "static";
        typeID = WorldObj::sstatic;
        collideFlags = 0;
        collideFunction = 0;
        modified = true;
    }
}

void StaticObj::save(QTextStream* out){
    if (!loaded) return;
    if (jestPQ < 2) return;
    
if(type == "static")
*(out) << "	Static (\n";
if(type == "gantry")
*(out) << "	Gantry (\n";
if(type == "collideobject")
*(out) << "	CollideObject (\n";

*(out) << "		UiD ( "<<this->UiD<<" )\n";
if(type == "collideobject"){
*(out) << "		CollideFlags ( "<<this->collideFlags<<" )\n";
if(this->collideFunction > 0 )
*(out) << "		CollideFunction ( "<<this->collideFunction<<" )\n";
}
*(out) << "		FileName ( "<<ParserX::AddComIfReq(this->fileName)<<" )\n";
if(this->staticFlags != 0)
*(out) << "		StaticFlags ( "<<ParserX::MakeFlagsString(this->staticFlags)<<" )\n";
*(out) << "		Position ( "<<this->position[0]<<" "<<this->position[1]<<" "<<-this->position[2]<<" )\n";
*(out) << "		QDirection ( "<<this->qDirection[0]<<" "<<this->qDirection[1]<<" "<<-this->qDirection[2]<<" "<<this->qDirection[3]<<" )\n";
*(out) << "		VDbId ( "<<this->vDbId<<" )\n";
if(this->staticDetailLevel > -1)
*(out) << "		StaticDetailLevel ( "<<this->staticDetailLevel<<" )\n";
*(out) << "	)\n";
}