#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
//#include <QOpenGLFunctions_3_2_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QBasicTimer>
#include "Camera.h"
#include "WorldObj.h"
#include "Pointer3d.h"
#include "Ref.h"

class Tile;
class SFile;
class Eng;
class GLUU;
class Route;
class Brush;
class PreciseTileCoordinate;
class CoordsMkr;
class MapWindow;


QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
    void cleanup();
    void enableTool(QString name);
    void setPaintBrush(Brush* brush);
    void jumpTo(PreciseTileCoordinate*);
    
    void msg(QString text);
    void msg(QString name, bool val);
    void msg(QString name, int val);
    void msg(QString name, float val);
    void msg(QString name, QString val);

signals:
    void routeLoaded(Route * a);
    void itemSelected(Ref::RefItem* pointer);
    void naviInfo(int all, int hidden);
    void posInfo(PreciseTileCoordinate* pos);
    void setToolbox(QString name);
    void setBrushTextureId(int val);
    void showProperties(WorldObj* obj);
    void flexData(int x, int z, float* p);
    void mkrList(std::unordered_map<std::string, CoordsMkr*> list);
    
    void sendMsg(QString name);
    void sendMsg(QString name, bool val);
    void sendMsg(QString name, int val);
    void sendMsg(QString name, float val);
    void sendMsg(QString name, QString val);
    

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent * event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent * event) Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
    void drawPointer();
private:
    void setupVertexAttribs();
    void setSelectedObj(WorldObj* o);
    QBasicTimer timer;
    unsigned long long int lastTime;
    unsigned long long int timeNow;
    bool m_core;
    int m_xRot;
    int m_yRot;
    int m_zRot;
    int fps;
    QPoint m_lastPos;
    SFile* sFile;
    Eng* eng;
    Tile* tile;
    Route* route;
    GLUU* gluu;
    bool m_transparent;
    Camera* camera;
    bool selection = false;
    int mousex, mousey;
    WorldObj* selectedObj = NULL;
    WorldObj* lastSelectedObj = NULL;
    Pointer3d* pointer3d;
    float lastPointerPos[3];
    float aktPointerPos[3];
    bool mousePressed = false;
    QString toolEnabled = "";
    float moveStep = 0.25;
    float moveMaxStep = 0.25;
    float moveMinStep = 0.01;
    bool resizeTool = false;
    bool rotateTool = false;
    bool translateTool = false;
    float lastNewObjPos[3];
    float lastNewObjPosT[2];
    
    Brush* defaultPaintBrush;
    MapWindow* mapWindow;

};

#endif
