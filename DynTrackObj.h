/*  This file is part of TSRE5.
 *
 *  TSRE5 - train sim game engine and MSTS/OR Editors. 
 *  Copyright (C) 2016 Piotr Gadecki <pgadecki@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *
 *  See LICENSE.md or https://www.gnu.org/licenses/gpl.html
 */

#ifndef DYNTRACKOBJ_H
#define	DYNTRACKOBJ_H

#include "WorldObj.h"
#include <QString>

class DynTrackObj : public WorldObj{
public:
    struct Section{
        int type;
        unsigned int sectIdx;
        float a;
        float r;
    };
    struct Shape{
        int iloscv;
        QOpenGLBuffer VBO;
        QOpenGLVertexArrayObject VAO;
    };
    
    Section* sections;
    Shape shape[2];
    int sectionIdx;
    DynTrackObj();
    DynTrackObj(const DynTrackObj& orig);
    virtual ~DynTrackObj();
    bool allowNew();
    void load(int x, int y);
    void set(int sh, FileBuffer* val);
    void set(QString sh, FileBuffer* data);
    void set(QString sh, float* val);
    void save(QTextStream* out);
    void resize(float x, float y, float z);
    void deleteVBO();
    int getDefaultDetailLevel();
    void render(GLUU* gluu, float lod, float posx, float posz, float* playerW, float* target, float fov, int selectionColor);
private:
    int tex1;
    int tex2;
    bool init;
    float elevation;
    float* jNodePosn = NULL;
    float bound[6];
    int sidxSelected = 0;
    void drawShape();
    void genShape();
    void createVBO(Shape* shape, int ptr, float * data);
    bool getSimpleBorder(float* border);
    bool getBoxPoints(QVector<float> &points);
};

#endif	/* DYNTRACKOBJ_H */

