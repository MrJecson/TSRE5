/*  This file is part of TSRE5.
 *
 *  TSRE5 - train sim game engine and MSTS/OR Editors. 
 *  Copyright (C) 2016 Piotr Gadecki <pgadecki@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *
 *  See LICENSE.md or https://www.gnu.org/licenses/gpl.html
 */


#ifndef ENVIRONMENT_H
#define	ENVIRONMENT_H

#include <QString>

class FileBuffer;

class Environment {
public:
    Environment(QString path);
    virtual ~Environment();
    void renderWater();
private:
    bool loaded;
    void loadWaterLayer(FileBuffer* data);
};

#endif	/* ENVIRONMENT_H */

