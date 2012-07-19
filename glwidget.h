#ifndef WIDGET_H
#define WIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLShaderProgram>
#include <QtOpenGL/QGLFramebufferObject>
#include <QtOpenGL/QGLBuffer>

class GLWidget : public QGLWidget
{
    Q_OBJECT
    
public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    GLfloat backgroundFactor() const;
    void setBackgroundFactor(GLfloat factor);

    bool isSGFilterEnabled() const;
    void setSGFilter(bool enabled);

    GLfloat colormapEnd() const;
    void setColormapEnd(GLfloat end);

    GLfloat colormapBegin() const;
    void setColormapBegin(GLfloat begin);

    bool is3DMode() const;
    void set3DMode(bool enabled);

    GLfloat zScale() const;
    void setZScale(GLfloat zScale);

    bool openImages(const QImage &background, const QImage &image);

    const QVector<float> &data() const;

    QSize imagesSize() const;

    QImage capture();

signals:
    void mousePressChangeBackgroundFactor();
    void dataChanged();

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void wheelEvent(QWheelEvent *e);
    virtual void timerEvent(QTimerEvent *);

private:
    void updateViewportInfos();

    QGLShaderProgram *_subtraction;
    QGLShaderProgram *_sgfilter;
    QGLShaderProgram *_render;
    QGLShaderProgram *_render3D;
    QGLFramebufferObject *_fbo;
    QGLBuffer _vbo, _ibo;

    GLuint _texture0;
    GLuint _texture1;

    QImage _background;
    QImage _image;
    int _xviewport, _yviewport;
    int _wviewport, _hviewport;
    int _width, _height;

    GLfloat _backgroundFactor;
    bool _sgfilterEnabled;
    GLfloat _colormapEnd;
    GLfloat _colormapBegin;
    GLfloat _zScale;
    bool _3dEnabled;
    bool _needUpdateFBO;
    bool _needUpdateVBO;

    GLfloat _alpha, _beta;
    GLfloat _zoom;
    QPointF _lastMousePosition;

    QVector<float> _data;
};

#endif // WIDGET_H
