#include "glwidget.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QMatrix4x4>
#include <QtAlgorithms>
#include <cmath>

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent),
      _vbo(QGLBuffer::VertexBuffer), _ibo(QGLBuffer::IndexBuffer),
      _texture0(0), _texture1(0)
{
    _subtraction = _sgfilter = _render = _render3D = 0;

    _fbo = 0;
    _backgroundMinusImage = true;
    _sgfilterEnabled = true;
    _needUpdateFBO = true;
    _3dEnabled = false;
    _needUpdateVBO = false;

    _backgroundFactor = 1.0;
    _colormapEnd = 0.2;
    _colormapBegin = 0.0;
    _zScale = 1.0;

    _beta = 0.0;
    _alpha = 0.0;
    _zoom = 1.0;
}

GLWidget::~GLWidget()
{
}

void GLWidget::setSubtractMode(bool backgroundMinusImage)
{
    if (_backgroundMinusImage != backgroundMinusImage) {
        _backgroundMinusImage = backgroundMinusImage;
        _needUpdateFBO = true;
    }
}

GLfloat GLWidget::backgroundFactor() const
{
    return _backgroundFactor;
}

void GLWidget::setBackgroundFactor(GLfloat factor)
{
    if (_backgroundFactor != factor) {
        _backgroundFactor = factor;
        _needUpdateFBO = true;
    }
}

bool GLWidget::isSGFilterEnabled() const
{
    return _sgfilterEnabled;
}

void GLWidget::setSGFilter(bool enabled)
{
    if (_sgfilterEnabled != enabled) {
        _sgfilterEnabled = enabled;
        _needUpdateFBO = true;
    }
}

GLfloat GLWidget::colormapEnd() const
{
    return _colormapEnd;
}

void GLWidget::setColormapEnd(GLfloat end)
{
    if (_colormapEnd != end) {
        _colormapEnd = end;
        _needUpdateFBO = true;
    }
}

GLfloat GLWidget::colormapBegin() const
{
    return _colormapBegin;
}

void GLWidget::setColormapBegin(GLfloat begin)
{
    if (_colormapBegin != begin) {
        _colormapBegin = begin;
        _needUpdateFBO = true;
    }
}

bool GLWidget::is3DMode() const
{
    return _3dEnabled;
}

void GLWidget::set3DMode(bool enabled)
{
    if (_3dEnabled != enabled) {
        _3dEnabled = enabled;

        if (_3dEnabled) {
        } else {
        }
    }
}

GLfloat GLWidget::zScale() const
{
    return _zScale;
}

void GLWidget::setZScale(GLfloat zScale)
{
    _zScale = zScale;
}

bool GLWidget::openImages(const QImage &background, const QImage &image)
{
    if (background.isNull() || image.isNull())
        return false;

    if (background.size() != image.size()) {
        qDebug("size error : the two images must have the same size");
        return false;
    }

    // align_image_stack -a align 1.bmp 2.bmp
    _background = background;
    _image = image;

    makeCurrent();
    _texture0 = bindTexture(_background);
    _texture1 = bindTexture(_image);

    _width = _background.width();
    _height = _background.height();

    updateViewportInfos();

    if (_fbo && _fbo->size() != _background.size()) {
        delete _fbo;
    }
    if (!_fbo) {
        _fbo = new QGLFramebufferObject(_background.size(), QGLFramebufferObject::NoAttachment, GL_TEXTURE_2D, GL_R32F);
    }

    _sgfilter->bind();
    _sgfilter->setUniformValue("dPixel", GLfloat(1.0 / GLfloat(_width)), GLfloat(1.0 / GLfloat(_height)));
    _sgfilter->release();
    doneCurrent();

    _needUpdateFBO = true;

    return true;
}

const QVector<float> &GLWidget::data() const
{
    return _data;
}

QSize GLWidget::imagesSize() const
{
    return QSize(_width, _height);
}

QImage GLWidget::capture()
{
    return grabFrameBuffer();
}

void GLWidget::initializeGL()
{
    glActiveTexture = (PFNGLACTIVETEXTUREPROC)context()->getProcAddress("glActiveTexture");

    _subtraction = new QGLShaderProgram(this);
    _subtraction->addShaderFromSourceFile(QGLShader::Vertex, ":/flat.vert");
    _subtraction->addShaderFromSourceFile(QGLShader::Fragment, ":/subtraction.frag");
    _subtraction->bindAttributeLocation("vertex", 0);
    _subtraction->bindAttributeLocation("texcoord", 1);
    _subtraction->link();
    _subtraction->bind();
    _subtraction->setUniformValue("texture0", 0);
    _subtraction->setUniformValue("texture1", 1);
    _subtraction->release();

    _sgfilter = new QGLShaderProgram(this);
    _sgfilter->addShaderFromSourceFile(QGLShader::Vertex, ":/flat.vert");
    _sgfilter->addShaderFromSourceFile(QGLShader::Fragment, ":/sgfilter.frag");
    _sgfilter->bindAttributeLocation("vertex", 0);
    _sgfilter->bindAttributeLocation("texcoord", 1);
    _sgfilter->link();
    _sgfilter->bind();
    _sgfilter->setUniformValue("texture0", 0);
    _sgfilter->release();


    _render = new QGLShaderProgram(this);
    _render->addShaderFromSourceFile(QGLShader::Vertex, ":/flat.vert");
    _render->addShaderFromSourceFile(QGLShader::Fragment, ":/render.frag");
    _render->bindAttributeLocation("vertex", 0);
    _render->bindAttributeLocation("texcoord", 1);
    _render->link();
    _render->bind();
    _render->setUniformValue("texture0", 0);
    _render->release();


    _render3D = new QGLShaderProgram(this);
    _render3D->addShaderFromSourceFile(QGLShader::Vertex, ":/render3D.vert");
    _render3D->addShaderFromSourceFile(QGLShader::Fragment, ":/render3D.frag");
    _render3D->bindAttributeLocation("zvalue", 0);
    _render3D->bindAttributeLocation("vertex", 1);
    _render3D->link();
    _render3D->bind();
    _render3D->setUniformValue("texture0", 0);
    _render3D->release();
}

void GLWidget::resizeGL(int, int)
{
    updateViewportInfos();
}

void GLWidget::paintGL()
{
    static GLfloat const vertices[] = {
        -1, -1, 1, -1, 1, 1, -1, 1
    };
    static GLfloat const texcoord[] = {
        0, 0, 1, 0, 1, 1, 0, 1
    };


    if (!_fbo) {
        qglClearColor(Qt::darkGreen);
        glClear(GL_COLOR_BUFFER_BIT);
        return;
    }

    if (_needUpdateFBO) {
        _needUpdateFBO = false;
        _needUpdateVBO = true;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _texture1);

        glViewport(0, 0, _width, _height);

        _subtraction->bind();
        _subtraction->setUniformValue("postfactor", GLfloat(_backgroundMinusImage ? 1.0 : -1.0));
        _subtraction->setUniformValue("backgroundFactor", _backgroundFactor);

        _subtraction->setAttributeArray(0, vertices, 2);
        _subtraction->setAttributeArray(1, texcoord, 2);

        _fbo->bind();
        _subtraction->enableAttributeArray(0);
        _subtraction->enableAttributeArray(1);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        _subtraction->disableAttributeArray(0);
        _subtraction->disableAttributeArray(1);
        _fbo->release();

        _subtraction->release();



        if (_sgfilterEnabled) {
            // filtre S-G
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _fbo->texture());

            glViewport(0, 0, _width, _height);

            _sgfilter->bind();
            _sgfilter->setAttributeArray(0, vertices, 2);
            _sgfilter->setAttributeArray(1, texcoord, 2);

            _fbo->bind();
            _sgfilter->enableAttributeArray(0);
            _sgfilter->enableAttributeArray(1);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            _sgfilter->disableAttributeArray(0);
            _sgfilter->disableAttributeArray(1);
            _fbo->release();

            _sgfilter->release();
        }

        _data.resize(_width * _height);
        _fbo->bind();
        glReadPixels(0, 0, _width, _height, GL_RED, GL_FLOAT, _data.data());
        _fbo->release();

        emit dataChanged();
    }



    if (_3dEnabled) {
        if (_needUpdateVBO) {
            _needUpdateVBO = false;
            if (!_vbo.isCreated())
                _vbo.create();
            if (!_ibo.isCreated())
                _ibo.create();

            QVector<GLfloat> vertices;
            for (GLfloat y = 0; y < _height; ++y) {
                for (GLfloat x = 0; x < _width; ++x) {
                    vertices << (x) / GLfloat(_width)
                             << (y) / GLfloat(_height);
                }
            }

            _vbo.bind();
            _vbo.allocate((_data.size() + vertices.size()) * sizeof (GLfloat));
            _vbo.write(0, _data.constData(), _data.size() * sizeof (GLfloat));
            _vbo.write(_data.size() * sizeof (GLfloat), vertices.constData(), vertices.size() * sizeof (GLfloat));
            _vbo.release();

            QVector<GLuint> indices;
            indices.reserve(2 * _width * (_height-1));
            for (int y = 1; y < _height; ++y) {
                for (int x = 0; x < _width; ++x) {
                    indices << y * _width + x;
                    indices << (y-1) * _width + x;
                }
            }
            _ibo.bind();
            _ibo.allocate(indices.constData(), indices.size() * sizeof (GLuint));
            _ibo.release();
        }

        QMatrix4x4 m;
        float aspect = float(width()) / float(height());
        m.ortho(-_zoom * aspect, _zoom * aspect, -_zoom, _zoom, 0.0001, 1000.0);
        //        m.perspective(70.0, qreal(width())/qreal(height()), 0.01, 1000.0);
        m.translate(0.0, 0.0, -2);
        m.rotate(_beta, 1.0, 0.0, 0.0);
        m.rotate(_alpha, 0.0, 0.0, 1.0);
        m.translate(0.0, 0.0, -(_colormapBegin + _colormapEnd) / 2.0);

        glViewport(0, 0, width(), height());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _fbo->texture());

        qglClearColor(Qt::black);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _render3D->bind();
        _render3D->setUniformValue("cmend", _colormapEnd);
        _render3D->setUniformValue("cmbeg", _colormapBegin);
        _render3D->setUniformValue("zscale", _zScale);
        _render3D->setUniformValue("matrix", m);
        _render3D->setUniformValue("aspect", GLfloat(_width) / GLfloat(_height));

        _vbo.bind();
        _render3D->setAttributeBuffer(0, GL_FLOAT, 0, 1);
        _render3D->setAttributeBuffer(1, GL_FLOAT, _width*_height*sizeof (GLfloat), 2); // coord
        _vbo.release();

        _render3D->enableAttributeArray(0);
        _render3D->enableAttributeArray(1);

        _ibo.bind();
        for (int y = 0; y < _height-1; ++y) {
            glDrawElements(GL_TRIANGLE_STRIP, 2*_width, GL_UNSIGNED_INT, BUFFER_OFFSET(y * 2*_width * sizeof (GLuint)));
        }
        _ibo.release();

        _render3D->disableAttributeArray(0);
        _render3D->disableAttributeArray(1);

        _render3D->release();
    } else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _fbo->texture());

        glDisable(GL_DEPTH_TEST);

        qglClearColor(Qt::gray);
        glClear(GL_COLOR_BUFFER_BIT);

        glViewport(_xviewport, _yviewport, _wviewport, _hviewport);

        _render->bind();

        _render->setAttributeArray(0, vertices, 2);
        _render->setAttributeArray(1, texcoord, 2);

        _render->setUniformValue("cmend", _colormapEnd);
        _render->setUniformValue("cmbeg", _colormapBegin);

        _render->enableAttributeArray(0);
        _render->enableAttributeArray(1);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        _render->disableAttributeArray(0);
        _render->disableAttributeArray(1);

        _render->release();
    }
}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
    if (_3dEnabled) {
        _lastMousePosition = e->posF();
    } else if (!_image.isNull()) {
        int x = (e->x() - _xviewport) * _width / _wviewport;
        int y = (e->y() - _yviewport) * _height / _hviewport;

        if (x < 0 || x >= _width || y < 0 || y >= _height)
            return;

        GLfloat g1 = qGray(_image.pixel(x, y));
        GLfloat g0 = qGray(_background.pixel(x, y));

        setBackgroundFactor(g1 / g0);

        emit mousePressChangeBackgroundFactor();

        updateGL();
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (_3dEnabled) {
        QPointF d = e->posF() - _lastMousePosition;
        _lastMousePosition = e->posF();

        _alpha += d.x();
        _beta += d.y();
        if (_beta < -90.0)
            _beta = -90.0;
        if (_beta > 0.0)
            _beta = 0.0;
        updateGL();
    }
}

void GLWidget::wheelEvent(QWheelEvent *e)
{
    _zoom *= std::pow(0.9995, e->delta());
    if (_zoom < 0.01)
        _zoom = 0.01;
    updateGL();
}

void GLWidget::timerEvent(QTimerEvent *)
{
    updateGL();
}

void GLWidget::updateViewportInfos()
{
    qreal imgaspect = qreal(_width) / qreal(_height);
    qreal widaspect = qreal(width()) / qreal(height());

    if (imgaspect > widaspect) {
        _wviewport = width();
        _xviewport = 0;
        _hviewport = _wviewport / imgaspect;
        _yviewport = (height() - _hviewport) / 2;
    } else {
        _hviewport = height();
        _yviewport = 0;
        _wviewport = _hviewport * imgaspect;
        _xviewport = (width() - _wviewport) / 2;
    }
}
