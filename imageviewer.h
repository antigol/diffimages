#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QGLWidget>
#include <QtOpenGL/QGLShaderProgram>

#include <QImage>
#include <QVector>

#include <GL/glext.h>

class ImageViewer : public QGLWidget
{
    Q_OBJECT
public:
    explicit ImageViewer(QWidget *parent = 0);
    ~ImageViewer();
    void setImage(const QImage &image);
    void setModificator(GLfloat mul, GLfloat add);
    const QVector<GLfloat> &data();
    
protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

private:
    void updateData();

    QGLShaderProgram *_shader;
    QImage _image;
    QVector<GLfloat> _data;
    GLfloat _mul, _add;
    GLuint _texture0;
    bool _newImage;
    bool _needUpdateData;

    PFNGLACTIVETEXTUREPROC glActiveTexture;
};

#endif // IMAGEVIEWER_H
