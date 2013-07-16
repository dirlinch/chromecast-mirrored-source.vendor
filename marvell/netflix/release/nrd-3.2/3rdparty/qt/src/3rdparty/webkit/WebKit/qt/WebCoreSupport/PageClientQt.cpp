/*
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"

#include "PageClientQt.h"
#include "texmap/TextureMapperPlatformLayer.h"

#if defined(Q_WS_X11)
#include <QX11Info>
#endif

#ifdef QT_OPENGL_LIB
#include <QGLWidget>
#endif
namespace WebCore {
#if USE(ACCELERATED_COMPOSITING)

class PlatformLayerQWidget : public TextureMapperLayerClient, public QObject {
public:
    PlatformLayerQWidget(QWidget *widget, QWebFrame *frame, TextureMapperContentLayer *layer)
        : m_widget(widget)
        , m_frame(frame)
        , m_layer(layer)
    {
        if (m_layer)
            m_layer->setPlatformLayerClient(this);
        m_frame->d->rootGraphicsLayer = m_layer;
        m_widget->installEventFilter(this);
    }
    ~PlatformLayerQWidget()
    {
        if (m_layer)
            m_layer->setPlatformLayerClient(0);
        if (m_frame->d)
            m_frame->d->rootGraphicsLayer = 0;
    }

    // We don't want a huge region-clip on the compositing layers; instead we unite the rectangles together
    // and clear them when the paint actually occurs.
    bool eventFilter(QObject* object, QEvent* event)
    {
        if (object == m_widget && event->type() == QEvent::Paint)
            m_dirtyRect = QRect();
        return QObject::eventFilter(object, event);
    }

    void setNeedsDisplay()
    {
        m_widget->update();
    }

    bool canSyncNow() const
    {
        if (m_widget->testAttribute(Qt::WA_WState_InPaintEvent))
            return false;
        return TextureMapperLayerClient::canSyncNow();
    }

    void setNeedsDisplayInRect(const IntRect& rect)
    {
#if 1
        m_dirtyRect |= rect;
        m_widget->update(m_dirtyRect);
#else
        m_widget->update(rect);
#endif
    }

    void setSizeChanged(const IntSize& newSize)
    {
    }

    QRect m_dirtyRect;
    QWidget* m_widget;
    QWebFrame* m_frame;
    TextureMapperContentLayer* m_layer;
};

#if !defined(QT_NO_GRAPHICSVIEW)
class PlatformLayerQGraphicsItem : public TextureMapperLayerClient, public QGraphicsItem  {
public:
    PlatformLayerQGraphicsItem()
        : QGraphicsItem(0)
        , m_layer(0)
    {
        setAcceptedMouseButtons(Qt::NoButton);
        setEnabled(true);
        setFlag(ItemUsesExtendedStyleOption, true);
        setFlag(ItemClipsToShape, true);
    }

    virtual ~PlatformLayerQGraphicsItem()
    {
#ifdef QT_OPENGL_LIB
        if (scene()) {
            if (scene()->views().size()) {
                if (QGLWidget* glWidget = qobject_cast<QGLWidget*>(scene()->views()[0]))
                    glWidget->makeCurrent();
            }
        }
#endif
        if (m_layer) {
            m_layer->cleanupTextureMapper();
            m_layer->setPlatformLayerClient(0);
        }
    }
    QRectF boundingRect() const
    {
        return QRectF(0, 0, m_size.width(), m_size.height());
    }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* options, QWidget* widget)
    {
        if (!m_layer)
            return;

        GraphicsContext gc(painter);
        const QTransform transform = painter->transform();
        TransformationMatrix matrix(
                transform.m11(), transform.m12(), 0, transform.m13(),
                transform.m21(), transform.m22(), 0, transform.m23(),
                0, 0, 1, 0,
                transform.m31(), transform.m32(), 0, transform.m33()
            );
        painter->beginNativePainting();
        m_layer->paint(&gc, widget->size(), options->rect, options->exposedRect.toAlignedRect(), matrix, painter->opacity());
        painter->endNativePainting();
    }

    void setNeedsDisplay()
    {
        update();
    }

    void setNeedsDisplayInRect(const IntRect& rect)
    {
        update(FloatRect(rect));
    }

    void setSizeChanged(const IntSize& newSize)
    {
        prepareGeometryChange();
        m_size = newSize;
    }

    void setPlatformLayer(TextureMapperContentLayer* layer)
    {
        if (layer)
            layer->setPlatformLayerClient(this);
        m_layer = layer;
    }

    TextureMapperContentLayer* m_layer;
    IntSize m_size;
};
#endif

void
PageClientQWidget::setRootGraphicsLayer(TextureMapperPlatformLayer* layer)
{
    if (layer) {
        rootGraphicsLayer = new PlatformLayerQWidget(view, page->mainFrame(), static_cast<TextureMapperContentLayer*>(layer));
        return;
    }
    delete rootGraphicsLayer;
    rootGraphicsLayer = 0;
}

void
PageClientQWidget::markForSync(bool scheduleSync)
{
    syncTimer.startOneShot(0);
}

void
PageClientQWidget::syncLayers(Timer<PageClientQWidget>*)
{
    QWebFramePrivate::core(page->mainFrame())->view()->syncCompositingStateRecursive();
}
#endif

PageClientQWidget::~PageClientQWidget()
{
#if USE(ACCELERATED_COMPOSITING)
    delete rootGraphicsLayer;
#endif
}

void PageClientQWidget::scroll(int dx, int dy, const QRect& rectToScroll)
{
    view->scroll(qreal(dx), qreal(dy), rectToScroll);
}

void PageClientQWidget::update(const QRect & dirtyRect)
{
    view->update(dirtyRect);

}

void PageClientQWidget::setInputMethodEnabled(bool enable)
{
    view->setAttribute(Qt::WA_InputMethodEnabled, enable);
}

bool PageClientQWidget::inputMethodEnabled() const
{
    return view->testAttribute(Qt::WA_InputMethodEnabled);
}

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
void PageClientQWidget::setInputMethodHint(Qt::InputMethodHint hint, bool enable)
{
    view->setInputMethodHints(hint);
}
#endif

#ifndef QT_NO_CURSOR
QCursor PageClientQWidget::cursor() const
{
    return view->cursor();
}

void PageClientQWidget::updateCursor(const QCursor& cursor)
{
    view->setCursor(cursor);
}
#endif

QPalette PageClientQWidget::palette() const
{
    return view->palette();
}

int PageClientQWidget::screenNumber() const
{
#if defined(Q_WS_X11)
    return view->x11Info().screen();
#endif
    return 0;
}

QWidget* PageClientQWidget::ownerWidget() const
{
    return view;
}

QRect PageClientQWidget::geometryRelativeToOwnerWidget() const
{
    return view->geometry();
}

QObject* PageClientQWidget::pluginParent() const
{
    return view;
}

QStyle* PageClientQWidget::style() const
{
    return view->style();
}

QRectF PageClientQWidget::windowRect() const
{
    return QRectF(view->window()->geometry());
}

#if !defined(QT_NO_GRAPHICSVIEW)
PageClientQGraphicsWidget::~PageClientQGraphicsWidget()
{
#if USE(ACCELERATED_COMPOSITING)
    delete rootGraphicsLayer;
#endif
}

void PageClientQGraphicsWidget::scroll(int dx, int dy, const QRect& rectToScroll)
{
    view->scroll(qreal(dx), qreal(dy), rectToScroll);

#if USE(ACCELERATED_COMPOSITING)
    updateCompositingScrollPosition();
#endif
}

void PageClientQGraphicsWidget::update(const QRect& dirtyRect)
{
    view->update(dirtyRect);

    createOrDeleteOverlay();
    if (overlay)
        overlay->update(QRectF(dirtyRect));
#if USE(ACCELERATED_COMPOSITING)
    syncLayers();
    // This might be a slow-scroll. We ensure that the compositing layers are in the right position.
    updateCompositingScrollPosition();
#endif
}

void PageClientQGraphicsWidget::createOrDeleteOverlay()
{
    bool useOverlay = false;
    if (!viewResizesToContents) {
#if USE(ACCELERATED_COMPOSITING)
        useOverlay = useOverlay || rootGraphicsLayer;
#endif
#if ENABLE(TILED_BACKING_STORE)
        useOverlay = useOverlay || QWebFramePrivate::core(page->mainFrame())->tiledBackingStore();
#endif
    }
    if (useOverlay == !!overlay)
        return;
    if (useOverlay) {
        overlay = QSharedPointer<QGraphicsItemOverlay>(new QGraphicsItemOverlay(view, page));
        overlay->setZValue(OverlayZValue);
    } else
        overlay.clear();
}

#if USE(ACCELERATED_COMPOSITING)
void PageClientQGraphicsWidget::syncLayers()
{
    if (shouldSync) {
        QWebFramePrivate::core(page->mainFrame())->view()->syncCompositingStateRecursive();
        shouldSync = false;
    }
}

void PageClientQGraphicsWidget::setRootGraphicsLayer(TextureMapperPlatformLayer* layer)
{
    if (rootGraphicsLayer && !layer) {
        delete rootGraphicsLayer;
        rootGraphicsLayer = 0;
        QWebFramePrivate::core(page->mainFrame())->view()->syncCompositingStateRecursive();
        createOrDeleteOverlay();
        return;
    }

    if (!rootGraphicsLayer)
        rootGraphicsLayer = new PlatformLayerQGraphicsItem();

    rootGraphicsLayer->setPlatformLayer(static_cast<TextureMapperContentLayer*>(layer));
    rootGraphicsLayer->setParentItem(view);
    rootGraphicsLayer->setZValue(RootGraphicsLayerZValue);
    updateCompositingScrollPosition();
    createOrDeleteOverlay();
}

void PageClientQGraphicsWidget::markForSync(bool scheduleSync)
{
    shouldSync = true;
    if (scheduleSync)
        syncMetaMethod.invoke(view, Qt::QueuedConnection);
}

void PageClientQGraphicsWidget::updateCompositingScrollPosition()
{
    if (rootGraphicsLayer && page && page->mainFrame()) {
        const QPoint scrollPosition = page->mainFrame()->scrollPosition();
        rootGraphicsLayer->setPos(-scrollPosition);
    }
}
#endif

#if ENABLE(TILED_BACKING_STORE)
void PageClientQGraphicsWidget::updateTiledBackingStoreScale()
{
    WebCore::TiledBackingStore* backingStore = QWebFramePrivate::core(page->mainFrame())->tiledBackingStore();
    if (!backingStore)
        return;
    backingStore->setContentsScale(view->scale());
}
#endif

void PageClientQGraphicsWidget::setInputMethodEnabled(bool enable)
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    view->setFlag(QGraphicsItem::ItemAcceptsInputMethod, enable);
#endif
}

bool PageClientQGraphicsWidget::inputMethodEnabled() const
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    return view->flags() & QGraphicsItem::ItemAcceptsInputMethod;
#else
    return false;
#endif
}

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
void PageClientQGraphicsWidget::setInputMethodHints(Qt::InputMethodHints hints)
{
    view->setInputMethodHints(hints);
}
#endif

#ifndef QT_NO_CURSOR
QCursor PageClientQGraphicsWidget::cursor() const
{
    return view->cursor();
}

void PageClientQGraphicsWidget::updateCursor(const QCursor& cursor)
{
    view->setCursor(cursor);
}
#endif

QPalette PageClientQGraphicsWidget::palette() const
{
    return view->palette();
}

int PageClientQGraphicsWidget::screenNumber() const
{
#if defined(Q_WS_X11)
    if (QGraphicsScene* scene = view->scene()) {
        const QList<QGraphicsView*> views = scene->views();

        if (!views.isEmpty())
            return views.at(0)->x11Info().screen();
    }
#endif

    return 0;
}

QWidget* PageClientQGraphicsWidget::ownerWidget() const
{
    if (QGraphicsScene* scene = view->scene()) {
        const QList<QGraphicsView*> views = scene->views();
        return views.value(0);
    }
    return 0;
}

QRect PageClientQGraphicsWidget::geometryRelativeToOwnerWidget() const
{
    if (!view->scene())
        return QRect();

    QList<QGraphicsView*> views = view->scene()->views();
    if (views.isEmpty())
        return QRect();

    QGraphicsView* graphicsView = views.at(0);
    return graphicsView->mapFromScene(view->boundingRect()).boundingRect();
}

#if ENABLE(TILED_BACKING_STORE)
QRectF PageClientQGraphicsWidget::graphicsItemVisibleRect() const
{
    if (!view->scene())
        return QRectF();

    QList<QGraphicsView*> views = view->scene()->views();
    if (views.isEmpty())
        return QRectF();

    QGraphicsView* graphicsView = views.at(0);
    int xOffset = graphicsView->horizontalScrollBar()->value();
    int yOffset = graphicsView->verticalScrollBar()->value();
    return view->mapRectFromScene(QRectF(QPointF(xOffset, yOffset), graphicsView->viewport()->size()));
}
#endif

QObject* PageClientQGraphicsWidget::pluginParent() const
{
    return view;
}

QStyle* PageClientQGraphicsWidget::style() const
{
    return view->style();
}

QRectF PageClientQGraphicsWidget::windowRect() const
{
    if (!view->deviceSize().isEmpty())
        return QRectF(QRect(QPoint(0, 0), view->deviceSize()));

    if (!view->scene())
        return QRectF();

    // The sceneRect is a good approximation of the size of the application, in dependent of the view.
    return view->scene()->sceneRect();
}
#endif

} // namespace WebCore






