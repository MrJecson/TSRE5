#ifndef GLSHAPEWIDGET_H
#define	GLSHAPEWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QBasicTimer>

class SFile;
class Eng;
class Consist;
class GLUU;
class Camera;

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GlShapeWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    GlShapeWidget(QWidget *parent = 0);
    virtual ~GlShapeWidget();
    
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    int heightForWidth(int w) const {
        return w/10;
    }
public slots:
    void showEng(int id);
    void showCon(int id);
    void cleanup();
signals:

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

private:
    void setupVertexAttribs();
    QBasicTimer timer;
    unsigned long long int lastTime;
    unsigned long long int timeNow;
    bool m_core;
    int m_xRot;
    int m_yRot;
    int m_zRot;
    int fps;
    QPoint m_lastPos;
    GLUU* gluu;
    int mousex, mousey;
    bool mousePressed = false;
    SFile* sFile = NULL;
    Eng* eng = NULL;
    Consist* con = NULL;
    Camera* camera;
    int renderItem = 0;
};

#endif	/* GLSHAPEWIDGET_H */

