#include "imageviewer.h"

ImageViewer::ImageViewer(QWidget *parent) :
    QGLWidget(parent)
{
    _texture0 = 0;
    _newImage = false;
}

ImageViewer::~ImageViewer()
{
    makeCurrent();
    if (_texture0 != 0)
        deleteTexture(_texture0);
}

void ImageViewer::setImage(const QImage &image)
{
    _image = image;
    _newImage = true;
    _needUpdateData = true;
    updateGL();
}

void ImageViewer::setModificator(GLfloat mul, GLfloat add)
{
    _mul = mul;
    _add = add;
    _needUpdateData = true;
}

const QVector<GLfloat> &ImageViewer::data()
{
    updateData();
    return _data;
}

void ImageViewer::initializeGL()
{
    _shader = new QGLShaderProgram(this);
    _shader->addShaderFromSourceFile(QGLShader::Vertex, ":/flat.vert");
    _shader->addShaderFromSourceFile(QGLShader::Fragment, ":/viewer.frag");
    _shader->bindAttributeLocation("vertex", 0);
    _shader->bindAttributeLocation("texcoord", 1);
    _shader->link();
    _shader->bind();
    _shader->setUniformValue("texture0", 0);
    _shader->setUniformValue("mul", _mul);
    _shader->setUniformValue("add", _add);
    _shader->release();
}

void ImageViewer::resizeGL(int, int)
{
}

void ImageViewer::paintGL()
{
    glViewport(0, 0, width(), height());
    qglClearColor(Qt::darkGreen);
    glClear(GL_COLOR_BUFFER_BIT);

    if (_image.isNull()) {
        return;
    }

    static GLfloat const vertices[] = {
        -1, -1, 1, -1, 1, 1, -1, 1
    };
    static GLfloat const texcoord[] = {
        0, 0, 1, 0, 1, 1, 0, 1
    };

    if (_newImage) {
        _newImage = false;
        if (_texture0 != 0)
            deleteTexture(_texture0);
        _texture0 = bindTexture(_image);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture0);

    _shader->bind();
    _shader->setAttributeArray(0, vertices, 2);
    _shader->setAttributeArray(1, texcoord, 2);

    _shader->setUniformValue("mul", _mul);
    _shader->setUniformValue("add", _add);

    _shader->enableAttributeArray(0);
    _shader->enableAttributeArray(1);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    _shader->disableAttributeArray(0);
    _shader->disableAttributeArray(1);

    _shader->release();
}

void ImageViewer::updateData()
{
    if (_needUpdateData) {
        _needUpdateData = false;
        _data.clear();

        for (int y = 0; y < _image.height(); ++y) {
            for (int x = 0; x < _image.width(); ++x) {
                _data << qGray(_image.pixel(x, y)) * _mul + _add;
            }
        }
    }
}
