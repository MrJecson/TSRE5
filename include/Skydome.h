/*  This file is part of TSRE5.
 *
 *  TSRE5 - train sim game engine and MSTS/OR Editors. 
 *  Copyright (C) 2016 Piotr Gadecki <pgadecki@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *
 *  See LICENSE.md or https://www.gnu.org/licenses/gpl.html
 */

#ifndef SKYDOME_H
#define	SKYDOME_H

#include "GameObj.h"

class GLUU;
class SFile;

class Skydome : public GameObj {
public:
    Skydome();
    Skydome(const Skydome& orig);
    virtual ~Skydome();
    
    void render(GLUU* gluu, int renderMode);

private:
    bool loaded = false;
    SFile* shapePointer = nullptr;
};

#endif	/* SKYDOME_H */
