/*
    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "GraphicsLayerTextureMapper.h"

#include "Logging.h"
#include "CurrentTime.h"
#include "FloatRect.h"
#include "GraphicsContext.h"
#include "HashMap.h"
#include "Image.h"
#include "RefCounted.h"
#include "TextureMapper.h"
#include "TextStream.h"
#include "CString.h"
#include "TextureMapperPlatformLayer.h"
#include "Timer.h"
#include "TransformOperations.h"
#include "TranslateTransformOperation.h"
#include "UnitBezier.h"
#include <stdio.h>

#if PLATFORM(QT)
#include "QtCore/qdebug.h"
#endif

#define DEBUG_TEXMAP_FPS 1

#if 0
# define DEBUG_STATE_CHANGES
# define DEBUG_SET_NEEDSDISPLAY
#endif
#ifdef DEBUG_STATE_CHANGES
# define NOTIFY_CHANGE(node, args_format)    \
    if(LogRendering.state == WTFLogChannelOn) \
        WTFLog(&LogRendering, "NotifyChange(%p): %s %s", (TextureMapperNode*)node, __func__, String::format args_format .latin1().data());
#else
# define NOTIFY_CHANGE(node, x)
#endif

namespace WebCore {

class TexmapAnimationBase;

struct TexmapPaintOptions {
    BitmapTexture* surface;
    TextureMapper* textureMapper;
    GraphicsContext* gc;
    TextureMapperNode* rootLayer;
    float opacity;
    IntRect scissorRect;
    IntRect visibleRect;
};
class TextureMapperCache {
public:
    void mark(BitmapTexture *texture);

    class Entry {
    public:
        RefPtr<BitmapTexture> texture;
        Entry() : previousCost(0) { }
        inline int calculateCost() const
        {
            if (!texture || !texture->isValid())
                return 0;
            const IntSize sz = texture->size();
            return sz.width() * sz.height() * 4;
        }
        Entry(BitmapTexture* newTexture)
            : texture(newTexture)
        {
        }
        bool operator==(const Entry& other) const { return texture == other.texture; }
        int previousCost;
    };

    TextureMapperCache() : m_totalCost(0) {}

    void purge();
    Vector<Entry> m_data;
    int m_totalCost;
#ifndef TEXMAP_TEXTURE_CACHE_KBS
#define TEXMAP_TEXTURE_CACHE_KBS 24 * 1024
#endif
    static const int MaxCost = TEXMAP_TEXTURE_CACHE_KBS * 1024;
    static const int PurgeAmount = MaxCost / 4;
};

enum ShowLayers {
    SHOW_NONE    = 0,
    SHOW_RENDER  = 1,
    SHOW_BLT     = 2,
    SHOW_BOUNDS  = 3,
};
static ShowLayers textureMapperShowLayers()
{
    static int ret = -1;
    if(ret == -1) {
        ret = SHOW_NONE;
        if(const char *show_layers = getenv("QT_WEBKIT_SHOW_LAYERS")) {
            if(!strcasecmp(show_layers, "none") || !strcmp(show_layers, "0"))
                ret = SHOW_NONE;
            else if(!strcasecmp(show_layers, "blit") || !strcasecmp(show_layers, "blt"))
                ret = SHOW_BLT;
            else if(!strcasecmp(show_layers, "bounds"))
                ret = SHOW_BOUNDS;
            else if(!strcasecmp(show_layers, "render") || !strcmp(show_layers, "1"))
                ret = SHOW_RENDER;
        }
    }
    return (ShowLayers)ret;
}

void TextureMapperCache::purge()
{
    // needs active context: texture.destroy
    const int size = m_data.size();

    if (m_totalCost <= TextureMapperCache::MaxCost)
        return;

    // Ensure that we have the right count. It might be inaccurate if content changed size.
    // We only do this when we're actually ready to purge.
    m_totalCost = 0;
    for (int i = 0; i < size; ++i)
        m_totalCost += m_data[i].calculateCost();

    int before = m_totalCost;
    for (int i = size-1; i >= 0 && m_totalCost > TextureMapperCache::MaxCost - TextureMapperCache::PurgeAmount; --i) {
        Entry& entry = m_data[i];
        if (entry.texture->isLocked() || !entry.texture->isValid())
            continue;
        m_totalCost -= entry.previousCost;
        entry.texture->destroy();
        m_data.remove(i);
    }

    if(LogRendering.state == WTFLogChannelOn)
        WTFLog(&LogRendering, "[TextureMapper] Purged %2.4fK of textures(%d) [%2.4fK/%d]", float(before - m_totalCost) / 1024,
               int(size-m_data.size()), float(m_totalCost) / 1024, m_data.size());
}

void TextureMapperCache::mark(BitmapTexture *texture)
{
    if (!texture || !texture->isValid())
        return;

    Entry entry(texture);
    size_t index = m_data.find(entry);
    if (!index)
        return;

    if (index < m_data.size())
        m_data.remove(index);
    const int cost = entry.calculateCost();
    m_totalCost -= entry.previousCost;
    m_totalCost += (entry.previousCost = cost);
    m_data.prepend(entry);

    if(LogRendering.state == WTFLogChannelOn)
        WTFLog(&LogRendering, "[TextureMapper] Marked texture [%2.4fK/%d]", float(m_totalCost) / 1024, m_data.size());
}

TextureMapperCache gTextureMapperCache;

class TextureMapperCacheLock {
public:
    TextureMapperCacheLock(BitmapTexture* texture) : m_texture(texture)
    {
        if (m_texture)
            m_texture->lock();
    }
    ~TextureMapperCacheLock()
    {
        if (m_texture)
            m_texture->unlock();
    }

private:
    RefPtr<BitmapTexture> m_texture;
};

class TextureMapperNode : public TextureMapperContentLayer {

public:
    // This set of flags help us defer which properties of the layer have been
    // modified by the compositor, so we can know what to look for in the next flush.
    enum ChangeMask {
        NoChanges =                 0,

        ParentChange =              (1L << 0),
        ChildrenChange =            (1L << 1),
        MaskLayerChange =           (1L << 2),
        PositionChange =            (1L << 3),

        AnchorPointChange =         (1L << 4),
        SizeChange  =               (1L << 5),
        TransformChange =           (1L << 6),
        ContentChange =             (1L << 7),

        GeometryOrientationChange = (1L << 8),
        ContentsOrientationChange = (1L << 9),
        OpacityChange =             (1L << 10),
        ContentsRectChange =        (1L << 11),

        Preserves3DChange =         (1L << 12),
        MasksToBoundsChange =       (1L << 13),
        DrawsContentChange =        (1L << 14),
        ContentsOpaqueChange =      (1L << 15),

        BackfaceVisibilityChange =  (1L << 16),
        ChildrenTransformChange =   (1L << 17),
        DisplayChange =             (1L << 18),
        BackgroundColorChange =     (1L << 19),

        ReplicaLayerChange =        (1L << 20)
    };

    enum LayerType {
        DefaultLayer,
        RootLayer,
        ScissorLayer,
        ClipLayer,
        TransparencyLayer
    };

    // The compositor lets us special-case images and colors, so we try to do so.
    enum StaticContentType { HTMLContentType, DirectImageContentType, ColorContentType, MediaContentType, Canvas3DContentType};

    TextureMapperNode* rootLayer();

    TextureMapperNode(GraphicsLayerTextureMapper* newLayer);
    virtual ~TextureMapperNode();

#if DEBUG_TEXMAP_FPS
    void beginAnimation(AnimatedPropertyID property);
    void endAnimation(AnimatedPropertyID property);
#endif

    void clearDirectImage();
    void computeTransformations();
    IntSize nearestSurfaceSize() const;
    void computeReplicaTransform();
    LayerType computeLayerType() const;
    void computeLocalTransform();
    void flattenTo2DSpaceIfNecessary();
    void initializeTextureMapper(TextureMapper*);
    void invalidateTransform(bool recurse);
    void notifyChange(ChangeMask);
    enum {
        SyncFlags_None    = 0x00,
        SyncFlags_Recurse = 0x01,
        SyncFlags_Force   = 0x02
    };
    int syncCompositingState(unsigned char flags);
    void performPostSyncOperations();
    void setNeedsDisplay();
    void setNeedsDisplayInRect(IntRect);
    virtual void cleanupTextureMapper();

#if ENABLE_TEXMAP_ANIMATION
    void setTransform(const TransformationMatrix&);
    void setOpacityFromAnimationFrame(float opacity);
    void setTransformFromAnimationFrame(const TransformationMatrix&);
    void notifyAnimationStarted();
    inline void notifyAnimationStartedTimerFired(Timer<TextureMapperNode>*) { notifyAnimationStarted(); }
    inline void notifyAnimationStartedAsync() { m_notifyAnimationStartedTimer.startOneShot(0); }
    void notifyTransformAnimationRunning(bool r);
    void notifyOpacityAnimationRunning(bool r);
#endif

    inline bool isVisible() const {
#if 0
        if(!m_state.visible || (m_anim.opacityAnimationRunning ? m_anim.opacity : m_state.opacity) < 0.01)
            return false;
        if(m_size.isEmpty() || (m_state.masksToBounds || m_children.isEmpty()))
            return false;
        return true;
#else
        return !((m_size.isEmpty() && (m_state.masksToBounds || m_children.isEmpty())) ||
                 !m_state.visible || (m_anim.opacityAnimationRunning ? m_anim.opacity : m_state.opacity) < 0.01);
#endif
    }
    inline bool hasContent() const { return (m_state.drawsContent || m_currentContent.contentType != HTMLContentType); }
    void paintRecursive(TexmapPaintOptions options);
    void paintSelf(TextureMapper* textureMapper, float opacity, BitmapTexture* maskTexture, BitmapTexture* replicaMaskTexture, bool isSurface);
    void uploadTextureFromContent(TextureMapper* textureMapper);

    void setSurfaceDescendantsNeedsDisplay();

    inline IntSize size() const { return m_size; }

    virtual void setPlatformLayerClient(TextureMapperLayerClient*);
    inline TextureMapperLayerClient *getPlatformLayerClient() const {
        for(const TextureMapperNode *n = this; n; n = n->m_parent) {
            if(n->m_platformClient)
                return n->m_platformClient;
        }
        return 0;
    }

    virtual void paint(GraphicsContext*, const IntSize&, const IntRect& targetRect, const IntRect& exposedRect, const TransformationMatrix& transform, float opacity);

    static TextureMapperNode* toTextureMapperNode(const GraphicsLayer*);
public:
    GraphicsLayerTextureMapper* m_layer;
    const char* m_lastTextureMapperType;
    RefPtr<TextureMapper> m_lastTextureMapper;
    struct TransformData {
        TransformationMatrix base, target, replica, forDescendants, perspective, local;
        IntRect targetBoundingRect;
        float centerZ;
        bool dirty, localDirty, perspectiveDirty;
        IntRect boundingRectFromRoot;
        TransformData() : dirty(true), localDirty(true), perspectiveDirty(true)
        {
        }
    };

    TransformData m_transforms;

    LayerType m_layerType;
    int m_show_color;

#if ENABLE_TEXMAP_ANIMATION
    struct GraphicsLayerTextureMapperAnim {
        inline GraphicsLayerTextureMapperAnim() : transformAnimationRunning(false),
                                        opacityAnimationRunning(false) { }
        ~GraphicsLayerTextureMapperAnim() {}
        Vector<OwnPtr<TexmapAnimationBase> > active;
        bool transformAnimationRunning;
        TransformationMatrix transform;
        bool opacityAnimationRunning;
        float opacity;
    } m_anim;
    Timer<TextureMapperNode> m_notifyAnimationStartedTimer;
#endif
    struct ContentData {
        IntRect needsDisplayRect;
        bool needsDisplay;
        Color backgroundColor;

        StaticContentType contentType;
        RefPtr<Image> image;
        TextureMapperMediaLayer* media;

        ContentData()
            : needsDisplay(false)
            , contentType(HTMLContentType)
            , image(0)
            , media(0)
        {
        }

    };

    inline IntRect targetRect() const
    {
        return m_currentContent.contentType == HTMLContentType ? fullRect() : m_state.contentsRect;
    }

    inline IntRect fullRect() const
    {
        return IntRect(0, 0, m_size.width(), m_size.height());
    }

    inline IntRect replicaRect() const
    {
        return m_layerType == TransparencyLayer ? IntRect(0, 0, m_nearestSurfaceSize.width(), m_nearestSurfaceSize.height()) : fullRect();
    }

    RefPtr<BitmapTexture> m_texture;
    RefPtr<BitmapTexture> m_surface, m_replicaSurface;

    ContentData m_pendingContent;
    ContentData m_currentContent;

    Vector<TextureMapperNode*> m_children;
    TextureMapperNode* m_parent;
    TextureMapperNode* m_effectTarget;
#if DEBUG_TEXMAP_FPS
    struct GraphicsLayerAnimation {
        GraphicsLayerAnimation() : frames(0) { last = started = WTF::currentTime(); }
        int frames;
        double last, started;
    };
    HashMap<int, GraphicsLayerAnimation*> m_anim_fps;
#endif

    int m_changeMask;
    IntSize m_size, m_nearestSurfaceSize;
    String m_name;
    TextureMapperLayerClient* m_platformClient;

    struct State {
        FloatPoint pos;
        FloatPoint3D anchorPoint;
        FloatSize size;
        TransformationMatrix transform;
        TransformationMatrix childrenTransform;
        Color backgroundColor;
        Color currentColor;
        GraphicsLayer::CompositingCoordinatesOrientation geoOrientation;
        GraphicsLayer::CompositingCoordinatesOrientation contentsOrientation;
        float opacity;
        IntRect contentsRect;
        int descendantsWithContent;
        TextureMapperNode* maskLayer;
        TextureMapperNode* replicaLayer;
        bool preserves3D;
        bool masksToBounds;
        bool drawsContent;
        bool contentsOpaque;
        bool backfaceVisibility;
        bool visible;
        bool dirty;
        bool tiled;
        bool hasSurfaceDescendants;

        State()
            : opacity(1.f)
            , descendantsWithContent(0)
            , maskLayer(0)
            , replicaLayer(0)
            , preserves3D(false)
            , masksToBounds(false)
            , drawsContent(false)
            , contentsOpaque(false)
            , backfaceVisibility(false)
            , visible(true)
            , dirty(true)
            , tiled(false)
            , hasSurfaceDescendants(false)
        {
        }
    };
    State m_state;
};

void TextureMapperNode::setNeedsDisplayInRect(IntRect rect)
{
    if(!isVisible())
        return;
    if (m_platformClient) {
        rect.intersect(IntRect(0, 0, m_size.width(), m_size.height()));
        if (!rect.isEmpty()) {
#ifdef DEBUG_SET_NEEDSDISPLAY
            if(LogRendering.state == WTFLogChannelOn)
                WTFLog(&LogRendering, "Client::setNeedsDisplayInRect: %d %d %d %d", rect.x(), rect.y(), rect.width(), rect.height());
#endif
            m_platformClient->setNeedsDisplayInRect(rect);
        }
    } else if (m_parent) {
        m_parent->setNeedsDisplayInRect(rect);
    }
}

void TextureMapperNode::setSurfaceDescendantsNeedsDisplay()
{
    if (m_layerType == ClipLayer || m_layerType == TransparencyLayer || m_state.replicaLayer || !isVisible())
        return;
    setNeedsDisplay();
    const int size = m_children.size();
    for (int i = 0; i < size; ++i)
        if (TextureMapperNode* child = m_children[i])
            child->setSurfaceDescendantsNeedsDisplay();
}

void TextureMapperNode::setNeedsDisplay()
{
#ifdef DEBUG_SET_NEEDSDISPLAY
    if(LogRendering.state == WTFLogChannelOn)
        WTFLog(&LogRendering, "setNeedsDisplay(%p) TRY", this);
#endif
    if (!isVisible())
        return;
    if (m_effectTarget)
        m_effectTarget->setNeedsDisplay();
    if (m_transforms.targetBoundingRect.isEmpty())
        return;
    if (m_state.descendantsWithContent || hasContent()) {
#ifdef DEBUG_SET_NEEDSDISPLAY
        if(LogRendering.state == WTFLogChannelOn)
            WTFLog(&LogRendering, "setNeedsDisplay(%p)", this);
#endif
#if 1
        if(TextureMapperLayerClient *client = getPlatformLayerClient()) {
#ifdef DEBUG_SET_NEEDSDISPLAY
            if(LogRendering.state == WTFLogChannelOn)
                WTFLog(&LogRendering, "Client::setNeedsDisplay()");
#endif
            client->setNeedsDisplay();
        } else
#endif
        {
            if(m_state.hasSurfaceDescendants) {
                const int size = m_children.size();
                for (int i = 0; i < size; ++i)
                    if (TextureMapperNode* child = m_children[i])
                        child->setSurfaceDescendantsNeedsDisplay();
            }
            if (m_transforms.dirty)
                computeTransformations();
            setNeedsDisplayInRect(m_transforms.targetBoundingRect);
        }
    }
}

void TextureMapperNode::setPlatformLayerClient(TextureMapperLayerClient* client)
{
    m_platformClient = client;
}

static int compareGraphicsLayersZValue(const void* pa, const void* pb)
{
    typedef const TextureMapperNode* ptr;
    const ptr* a = static_cast<const ptr*>(pa);
    const ptr* b = static_cast<const ptr*>(pb);
    return int(((*a)->m_transforms.centerZ - (*b)->m_transforms.centerZ)*1000);
}
inline static void sortByZOrder(Vector<TextureMapperNode* >& array, int, int)
{
    qsort(array.data(), array.size(), sizeof(TextureMapperNode*), compareGraphicsLayersZValue);
}

void TextureMapperNode::paint(GraphicsContext* gc, const IntSize& size, const IntRect& targetRect, const IntRect& exposedRect,
                              const TransformationMatrix& transform, float opacity)
{
    // needs active context
    ASSERT(m_layerType == RootLayer);
    if (m_size.isEmpty())
        return;

    WTF::StopWatch stopWatch(LogRendering.state == WTFLogChannelOn);
    if(LogRendering.state == WTFLogChannelOn)
        WTFLog(&LogRendering, "[TextureMapper] RootPaint!! [%d %d %d %d]", exposedRect.x(), exposedRect.y(), exposedRect.width(), exposedRect.height());

    RefPtr<TextureMapper> textureMapper = TextureMapper::create(gc);

    if (textureMapper->type() != m_lastTextureMapperType)
        gTextureMapperCache.m_data.clear();

    m_lastTextureMapper = textureMapper;
    TexmapPaintOptions opt;
    opt.opacity = 1;
    opt.rootLayer = this;
    opt.scissorRect = targetRect;
    opt.visibleRect = exposedRect;
    opt.textureMapper = textureMapper.get();
    opt.gc = textureMapper->graphicsContext();
    opt.surface = 0;
    paintRecursive(opt);

    if (textureMapper->allowSurfaceForRoot() || m_state.hasSurfaceDescendants) {
        textureMapper->bindSurface(0);
        textureMapper->paintToTarget(*m_surface.get(), size, transform, opacity * m_state.opacity, targetRect);
    }
    gTextureMapperCache.purge();
#if 0
    if(LogRendering.state == WTFLogChannelOn) {
        static int rootCount = 0;
        WTFLog(&LogRendering, "[TextureMapper] RootPaint(%d) %d layer(%p) (%d+%d %dx%d)", ++rootCount, textureMapper->allowSurfaceForRoot(),
               this, exposedRect.x(), exposedRect.y(), exposedRect.width(), exposedRect.height());
#if 1
        TextStream s;
        m_layer->dumpLayer(s);
        WTFLog(&LogRendering, "%s", s.release().latin1().data());
#endif
#if 0
        if (textureMapper->allowSurfaceForRoot())
            m_surface->save(String().format("/tmp/root_%d.png", rootCount));
#endif
    }
#endif

    if(LogRendering.state == WTFLogChannelOn)
        WTFLog(&LogRendering, "[TextureMapper] ~RootPaint!! [%d %d %d %d]  [%dms]",
               exposedRect.x(), exposedRect.y(), exposedRect.width(), exposedRect.height(), int(stopWatch.elapsed() * 1000));
}

inline TextureMapperNode* TextureMapperNode::toTextureMapperNode(const GraphicsLayer* layer)
{
    return layer ? static_cast<const GraphicsLayerTextureMapper*>(layer)->m_node.get() : 0;
}

TextureMapperNode::LayerType TextureMapperNode::computeLayerType() const
{
    /* calculate layer type. A layer can be one of the following:
        RootLayer: the top level. Draws to a framebuffer, and the target texture draws into the viewport.
                   only one layer is the root layer.
        ScissorLayer: draws to the current framebuffer, and applies an extra scissor before drawing its children.
                      A scissor layer is a layer with children that masks to bounds, is not a transparency layer, and has a rectangular clip.
        ClipLayer: creates a new framebuffer, the size of the layer, and then paints it to the enclosing BitmapTexture with the layer's transform/opacity.
                    A clip layer is a layer that masks to bounds, doesn't preserve 3D, has children, and has a transparency/mask or a non-rectangular transform.
        TransparencyLayer: creates a new framebuffer idetical in size to the current framebuffer. Then draws the fb's texture to the current framebuffer with identity transform.
                           Used for layers with children and transparency/mask that preserve 3D or don't mask to bounds.
        DefaultLayer: draws itself and its children directly to the current framebuffer.
                      any layer that doesn't conform to the other rules is a DefaultLayer.
    */

    const bool selfHasContent = hasContent();
    const bool hasDescendantsWithContent = m_state.descendantsWithContent - (selfHasContent ? 1 : 0);
    const bool hasTransparency = m_state.opacity < 0.99 || m_state.maskLayer;
    const bool hasReplica = m_state.replicaLayer;

    // Layer has no parent, it must be a root layer.
    if (!m_parent && !m_effectTarget)
        return RootLayer;

    // A layer with no contents is always a default layer.
    if (!m_state.descendantsWithContent)
        return DefaultLayer;

    // A layer with content-descendants and a mask is always a clip layer.
    if (hasDescendantsWithContent && m_state.maskLayer)
        return ClipLayer;

    // A masks-to bounds layer can be a clip or a scissor layer. It's a scissor layer only if it has a trivial clip (identity or translation), or if it has transparency.
    // That's because a ClipLayer would create an intermediate drawing surface (FB) - we want to limit it to when it's actually necessary, i.e. transparency or non-trivial clip.
    if (m_state.masksToBounds && hasDescendantsWithContent) {
        if (hasTransparency || !m_state.transform.isIdentityOrTranslation() || m_parent->m_state.preserves3D)
            return ClipLayer;
        return ScissorLayer;
    }

    // We use a transparency layer when we have two of the following 3: replica, transparency, descendants with contents.
    if ((hasReplica && hasDescendantsWithContent) || (hasReplica && hasTransparency) || (hasTransparency && m_state.descendantsWithContent > 1))
        return TransparencyLayer;
    return DefaultLayer;
}
void TextureMapperNode::initializeTextureMapper(TextureMapper* textureMapper)
{
    // needs active context
    if (textureMapper->type() == m_lastTextureMapperType)
        return;
#if 0
    if (m_layerType == ClipLayer || m_layerType == TransparencyLayer
        || (m_layerType == RootLayer && textureMapper->allowSurfaceForRoot()))
        m_surface = textureMapper->createTexture();
    else if (m_surface)
        m_surface->destroy();
    if (m_state.replicaLayer)
        m_replicaSurface = textureMapper->createTexture();
    else if (m_replicaSurface)
        m_replicaSurface->destroy();
#else
    m_surface = textureMapper->createTexture();
    m_replicaSurface = textureMapper->createTexture();
#endif
    m_texture = textureMapper->createTexture();
    gTextureMapperCache.mark(m_texture.get());
    m_lastTextureMapperType = textureMapper->type();
}

TextureMapperNode::TextureMapperNode(GraphicsLayerTextureMapper* newLayer)
    : m_layer(newLayer)
    , m_lastTextureMapperType(0)
    , m_lastTextureMapper(0)
    , m_layerType(DefaultLayer)
    , m_show_color(0)
#if ENABLE_TEXMAP_ANIMATION
    , m_notifyAnimationStartedTimer(this, &TextureMapperNode::notifyAnimationStartedTimerFired)
#endif
    , m_surface(0)
    , m_parent(0)
    , m_effectTarget(0)
    , m_changeMask(NoChanges)
    , m_platformClient(0)
{
    NOTIFY_CHANGE(this, ("BORN"));
}

TextureMapperNode* TextureMapperNode::rootLayer()
{
    if (m_effectTarget)
        return m_effectTarget->rootLayer();
    if (m_parent)
        return m_parent->rootLayer();
    return this;
}

void TextureMapperNode::invalidateTransform(bool recurse)
{
    NOTIFY_CHANGE(this, ("%d", recurse));
    m_transforms.dirty = true;
    if (m_layerType != ClipLayer)
        m_state.dirty = true;
    if (m_state.replicaLayer)
        m_state.replicaLayer->invalidateTransform(recurse);
    if(recurse) {
        const int size = m_children.size();
        for (int i = 0; i < size; ++i)
            if (TextureMapperNode* layer = m_children[i])
                layer->invalidateTransform(recurse);
    }
}

void TextureMapperNode::computeLocalTransform()
{
    if (!m_transforms.localDirty)
        return;
    m_transforms.localDirty = false;

    const float originX = m_state.anchorPoint.x() * m_size.width();
    const float originY = m_state.anchorPoint.y() * m_size.height();
    m_transforms.local =
        TransformationMatrix()
        .translate3d(originX + m_state.pos.x(), originY + m_state.pos.y(), m_state.anchorPoint.z())
#if ENABLE_TEXMAP_ANIMATION
        .multLeft(m_anim.transformAnimationRunning ? m_anim.transform : m_state.transform)
#else
        .multLeft(m_state.transform)
#endif
        .translate3d(-originX, -originY, -m_state.anchorPoint.z());
    NOTIFY_CHANGE(this, ("(%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f)",
                         m_transforms.local.m11(), m_transforms.local.m12(), m_transforms.local.m13(), m_transforms.local.m14(),
                         m_transforms.local.m21(), m_transforms.local.m22(), m_transforms.local.m23(), m_transforms.local.m24(),
                         m_transforms.local.m31(), m_transforms.local.m32(), m_transforms.local.m33(), m_transforms.local.m34(),
                         m_transforms.local.m41(), m_transforms.local.m42(), m_transforms.local.m43(), m_transforms.local.m44(),
                         m_transforms.local.a(), m_transforms.local.b(), m_transforms.local.c(), m_transforms.local.d(), m_transforms.local.e(), m_transforms.local.f()));
}

void TextureMapperNode::flattenTo2DSpaceIfNecessary()
{
    if (m_state.preserves3D)
        return;
    m_transforms.forDescendants.setM13(0);
    m_transforms.forDescendants.setM23(0);
    m_transforms.forDescendants.setM31(0);
    m_transforms.forDescendants.setM32(0);
    m_transforms.forDescendants.setM33(1);
    m_transforms.forDescendants.setM34(0);
    m_transforms.forDescendants.setM43(0);
}

IntSize TextureMapperNode::nearestSurfaceSize() const
{
    if (m_layerType == ClipLayer || m_layerType == RootLayer)
        return m_surface && !m_surface->size().isEmpty() ? m_surface->size() : m_size;
    return m_parent->nearestSurfaceSize();
}

void TextureMapperNode::computeReplicaTransform()
{
    if (!m_state.replicaLayer)
        return;

    m_nearestSurfaceSize = nearestSurfaceSize();

    if (m_layerType != TransparencyLayer) {
        m_transforms.replica = TransformationMatrix(m_transforms.target).multLeft(m_state.replicaLayer->m_transforms.local);
        return;
    }

    const float originX = m_transforms.target.m41();
    const float originY = m_transforms.target.m42();
    m_transforms.replica =
            TransformationMatrix()
                .translate(originX, originY)
                .multLeft(m_state.replicaLayer->m_transforms.local)
                .translate(-originX, -originY);
}

void TextureMapperNode::computeTransformations()
{
    if (!m_transforms.dirty)
        return;
    m_transforms.dirty = false;

    computeLocalTransform();

    m_transforms.target = TransformationMatrix(m_parent ? m_parent->m_transforms.forDescendants : TransformationMatrix()).multLeft(m_transforms.local);
    m_transforms.forDescendants = (m_layerType == ClipLayer ? TransformationMatrix() : m_transforms.target);

    if (!m_effectTarget) {
        m_transforms.targetBoundingRect = IntRect(m_transforms.target.mapRect(fullRect()));
        if (m_state.replicaLayer)
            m_state.replicaLayer->computeTransformations();

        flattenTo2DSpaceIfNecessary();

        if (!m_state.backfaceVisibility && m_transforms.target.inverse().m33() < 0) {
            m_state.visible = false;
        } else {
            m_state.visible = true;

            if (m_parent && m_parent->m_state.preserves3D)
                m_transforms.centerZ = m_transforms.target.mapPoint(FloatPoint3D(m_size.width() / 2, m_size.height() / 2, 0)).z();

            if (m_children.size() && !m_state.childrenTransform.isIdentity()) {
                const FloatPoint centerPoint = FloatPoint(m_size.width() / 2, m_size.height() / 2);
                if (m_transforms.perspectiveDirty)
                    m_transforms.perspective = TransformationMatrix()
                                               .translate(centerPoint.x(), centerPoint.y())
                                               .multLeft(m_state.childrenTransform)
                                               .translate(-centerPoint.x(), -centerPoint.y());
                m_transforms.perspectiveDirty = false;
                m_transforms.forDescendants.multLeft(m_transforms.perspective);
            }
        }
    }
    setNeedsDisplay();
    NOTIFY_CHANGE(this, ("%p (%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f)",
                         m_parent, m_transforms.target.m11(), m_transforms.target.m12(), m_transforms.target.m13(), m_transforms.target.m14(),
                         m_transforms.target.m21(), m_transforms.target.m22(), m_transforms.target.m23(), m_transforms.target.m24(),
                         m_transforms.target.m31(), m_transforms.target.m32(), m_transforms.target.m33(), m_transforms.target.m34(),
                         m_transforms.target.m41(), m_transforms.target.m42(), m_transforms.target.m43(), m_transforms.target.m44(),
                         m_transforms.target.a(), m_transforms.target.b(), m_transforms.target.c(), m_transforms.target.d(), m_transforms.target.e(), m_transforms.target.f()));

}

#if DEBUG_TEXMAP_FPS
void TextureMapperNode::beginAnimation(AnimatedPropertyID property)
{
    ASSERT(!m_anim_fps.get(property));
    if(LogRendering.state == WTFLogChannelOn)
        WTFLog(&LogRendering, "[TextureMapper] AnimationStarted(%p): %d", this, property);
    m_anim_fps.set(property, new GraphicsLayerAnimation());
}

void TextureMapperNode::endAnimation(AnimatedPropertyID property)
{
    if(GraphicsLayerAnimation* anim = m_anim_fps.take(property)) {
        const double duration = WTF::currentTime()-anim->started;
        if(LogRendering.state == WTFLogChannelOn)
            WTFLog(&LogRendering, "[TextureMapper] AnimationEnded(%p): %d: [%f] [%d frames] [%f FPS]", this, property, duration,
                   anim->frames, (1000 / ((duration*1000) / anim->frames)) );
        delete anim;
    }
}
#endif

void TextureMapperNode::uploadTextureFromContent(TextureMapper* textureMapper)
{
    // needs active context
    if (m_size.isEmpty() || !m_layer) {
        m_texture->destroy();
        return;
    }

    if (m_currentContent.contentType == DirectImageContentType) {
        if (m_currentContent.image)
            m_texture->setContentsToImage(m_currentContent.image.get());
        return;
    }

    if (m_currentContent.contentType == MediaContentType) {
        if (!m_currentContent.media)
            return;
        m_texture->reset(m_size, true);
        GraphicsContext gc(m_texture->beginPaintMedia());
        m_currentContent.media->paint(&gc);
        m_texture->endPaint();
    }

    const bool needsReset = (m_texture->contentSize() != m_size) || !m_texture->isValid();
    if ((m_currentContent.contentType != HTMLContentType)
        || (!m_currentContent.needsDisplay && m_currentContent.needsDisplayRect.isEmpty() && !needsReset))
        return;

    WTF::StopWatch stopWatch(LogRendering.state == WTFLogChannelOn);
    IntRect dirtyRect = IntRect(0, 0, m_size.width(), m_size.height());
    if (!needsReset && !m_currentContent.needsDisplay)
        dirtyRect.intersect(m_currentContent.needsDisplayRect);
    if (needsReset)
        m_texture->reset(m_size, m_state.contentsOpaque);

    if(!dirtyRect.isEmpty()) {
        {
            GraphicsContext gc(m_texture->beginPaint(textureMapper ? textureMapper->graphicsContext() : 0, dirtyRect));
            m_layer->paintGraphicsLayerContents(gc, dirtyRect);
#if 1
            if(textureMapperShowLayers() == SHOW_RENDER) {
                static int colors[] = { 0x33FF0000, 0x3300FF00, 0x330000FF };
                gc.fillRect(dirtyRect, Color(colors[m_show_color]), sRGBColorSpace);
                m_show_color = (m_show_color+1) % 3;
            }
            if(m_layer->showDebugBorders() || textureMapperShowLayers() == SHOW_BOUNDS) {
                gc.save();
                gc.setAlpha(1.0);
                gc.setCompositeOperation(CompositeSourceOver);
                gc.setStrokeColor(Color(0xFFFF0000), sRGBColorSpace);
                gc.strokeRect(IntRect(0, 0, m_size.width(), m_size.height()), 2);
                gc.restore();
            }
#endif
            m_texture->endPaint();
        }
#if 1
        if(LogRendering.state == WTFLogChannelOn) {
            static int renderCount = 0;
            WTFLog(&LogRendering, "[TextureMapper] Re-render(%d) %d layer(%p) %d::%d::%d::%d (%dx%d) (%dx%d) [%dms]", ++renderCount, m_layerType, this,
                   needsReset, m_currentContent.needsDisplay, !m_currentContent.needsDisplayRect.isEmpty(), m_state.descendantsWithContent,
                   dirtyRect.width(), dirtyRect.height(), m_size.width(), m_size.height(), int(stopWatch.elapsed() * 1000));
            static const char *dump_layers = getenv("QT_WEBKIT_SAVE_LAYERS");
            if(dump_layers)
                m_texture->save(String().format("/tmp/layer_%p_%d.png", this, renderCount));
        }
#endif
    }

    m_currentContent.needsDisplayRect = IntRect();
    m_currentContent.needsDisplay = false;
}

void TextureMapperNode::paintSelf(TextureMapper* textureMapper, float opacity, BitmapTexture* maskTexture, BitmapTexture* replicaMaskTexture, bool isSurface)
{
    // needs active context
    if (!m_layer || (!m_state.drawsContent && m_currentContent.contentType == HTMLContentType) /*|| !isVisible()*/)
        return;

    uploadTextureFromContent(textureMapper);
    if (m_state.replicaLayer && !isSurface)
        textureMapper->drawTexture(*m_texture.get(), replicaRect(), m_transforms.replica,
                         opacity * m_state.replicaLayer->m_state.opacity,
                         replicaMaskTexture ? replicaMaskTexture : maskTexture);

    const IntRect rect = m_layerType == ClipLayer ? fullRect() : targetRect();
    const TransformationMatrix transform = m_layerType == ClipLayer ? TransformationMatrix() : m_transforms.target;

    WTF::StopWatch stopWatch(LogRendering.state == WTFLogChannelOn);
#if 0
    WTFLog(&LogRendering, "PaintSelf(%p/%d): %d %d [%d %d %d %d] [%d %d %d %d] (%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f)",
           this, m_layerType, m_size.width(), m_size.height(),
           rect.x(), rect.y(), rect.width(), rect.height(),
           m_transforms.targetBoundingRect.x(), m_transforms.targetBoundingRect.y(), m_transforms.targetBoundingRect.width(), m_transforms.targetBoundingRect.height(),
           transform.m11(), transform.m12(), transform.m13(), transform.m14(),
           transform.m21(), transform.m22(), transform.m23(), transform.m24(),
           transform.m31(), transform.m32(), transform.m33(), transform.m34(),
           transform.m41(), transform.m42(), transform.m43(), transform.m44(),
           transform.a(), transform.b(), transform.c(), transform.d(), transform.e(), transform.f());
#endif
    textureMapper->drawTexture(*m_texture.get(), rect, transform, opacity, isSurface ? 0 : maskTexture);

#if 0
    if(textureMapperShowLayers() == SHOW_BLT) {
        static int colors[] = { 0x33FF0000, 0x3300FF00, 0x330000FF };
        textureMapper->drawRect(rect, Color(colors[m_show_color]), sRGBColorSpace, transform);
        m_show_color = (m_show_color+1) % 3;
    }
#endif

#if 0
    if(LogRendering.state == WTFLogChannelOn) {
        WTFLog(&LogRendering, "PaintSelf(%p/%d): %d %d [%d %d %d %d] [%d %d %d %d] (%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f) %dms",
               this, m_layerType, m_size.width(), m_size.height(),
               rect.x(), rect.y(), rect.width(), rect.height(),
               m_transforms.targetBoundingRect.x(), m_transforms.targetBoundingRect.y(), m_transforms.targetBoundingRect.width(), m_transforms.targetBoundingRect.height(),
               transform.m11(), transform.m12(), transform.m13(), transform.m14(),
               transform.m21(), transform.m22(), transform.m23(), transform.m24(),
               transform.m31(), transform.m32(), transform.m33(), transform.m34(),
               transform.m41(), transform.m42(), transform.m43(), transform.m44(),
               transform.a(), transform.b(), transform.c(), transform.d(), transform.e(), transform.f(), int(stopWatch.elapsed() * 1000));
    }
#endif
}

void TextureMapperNode::paintRecursive(TexmapPaintOptions options)
{
    // needs active context
    WTF::StopWatch stopWatch(LogRendering.state == WTFLogChannelOn);

#if DEBUG_TEXMAP_FPS
    if (!m_anim_fps.isEmpty()) {
        const double now = WTF::currentTime();
        HashMap<int, GraphicsLayerAnimation*>::iterator end = m_anim_fps.end();
        for (HashMap<int, GraphicsLayerAnimation*>::iterator it = m_anim_fps.begin(); it != end; ++it) {
            // WTFLog(&LogRendering, "FRAME(%d): %dms [%p]", it->second->frames, int((now-it->second->last)*1000), this);
            it->second->last = now;
            ++it->second->frames;
        }
    }
#endif
    bool wasDirty = m_state.dirty;
    m_state.dirty = false;

    if (options.opacity < 0.01 || !isVisible())
        return;
#if 0
    if (!options.scissorRect.intersects(m_transforms.targetBoundingRect))
        return;
#endif

    initializeTextureMapper(options.textureMapper);
    if (m_state.replicaLayer)
        computeReplicaTransform();

    if (m_state.maskLayer) {
        m_state.maskLayer->initializeTextureMapper(options.textureMapper);
        m_state.maskLayer->m_state.dirty = false;
    }

    if (m_state.replicaLayer) {
        m_state.replicaLayer->initializeTextureMapper(options.textureMapper);
        m_state.replicaLayer->m_state.dirty = false;
        if (m_state.replicaLayer->m_state.maskLayer) {
            m_state.replicaLayer->m_state.maskLayer->initializeTextureMapper(options.textureMapper);
            m_state.replicaLayer->m_state.maskLayer->m_state.dirty = false;
        }
    }

    TextureMapperNode* replica = m_state.replicaLayer;
    const bool isSurface = (m_layerType == ClipLayer
                            || m_layerType == TransparencyLayer
                            || (m_layerType == RootLayer
                                && (options.textureMapper->allowSurfaceForRoot() || m_state.hasSurfaceDescendants)
                                ));
    if (isSurface)
        uploadTextureFromContent(options.textureMapper);
    const IntRect boundingRectfromNearestSurface = m_transforms.targetBoundingRect;

#if ENABLE_TEXMAP_ANIMATION
    if (m_anim.opacityAnimationRunning)
        options.opacity *= m_anim.opacity;
    else
#endif
        options.opacity *= m_state.opacity;
    float contentOpacity = isSurface ? 1 : options.opacity;

    TexmapPaintOptions optionsForDescendants(options);
    optionsForDescendants.opacity = contentOpacity;
    if (m_layerType == ClipLayer) {
        optionsForDescendants.visibleRect = TransformationMatrix().translate(-boundingRectfromNearestSurface.x(),
                                                                             -boundingRectfromNearestSurface.y()).mapRect(options.visibleRect);
        optionsForDescendants.scissorRect = IntRect(0, 0, m_size.width(), m_size.height());
    }
    if (m_layerType == ScissorLayer)
        optionsForDescendants.scissorRect.intersect(m_transforms.targetBoundingRect);
    options.textureMapper->setClip(optionsForDescendants.scissorRect);

    static int spaces = 1;
#if 1
    if(LogRendering.state == WTFLogChannelOn) {
        for(int i = 0; i < spaces; ++i )
            fprintf(stderr, "  ");
        fprintf(stderr, "[TextureMapper] %d Rendering(%p/%d): %d [%dx%d] [%d %d %d %d]\n", spaces, this, isSurface, m_layerType, m_size.width(), m_size.height(),
                optionsForDescendants.scissorRect.x(), optionsForDescendants.scissorRect.y(), optionsForDescendants.scissorRect.width(),
                optionsForDescendants.scissorRect.height());
    }
#endif

    TextureMapperCacheLock _m_texture_lock(m_texture.get());
    TextureMapperCacheLock _m_surface_lock(m_surface.get());
    TextureMapperCacheLock _m_replicaSurface_lock(m_replicaSurface.get());

    gTextureMapperCache.purge();

    if (isSurface) {
        ASSERT(m_surface);
        if (!m_surface->isValid())
            wasDirty = true;
        if (m_state.tiled) {
            m_surface->reset(options.visibleRect.size());
            m_surface->setOffset(options.visibleRect.location());
        } else if (wasDirty)
            m_surface->reset(m_layerType == TransparencyLayer ? options.surface->size() : m_size);
        gTextureMapperCache.mark(m_surface.get());
        options.textureMapper->bindSurface(m_surface.get());
        optionsForDescendants.surface = m_surface.get();
    } else {
        if (m_surface)
            m_surface->destroy();
        contentOpacity = options.opacity;
    }

    RefPtr<BitmapTexture> maskTexture;
    RefPtr<BitmapTexture> replicaMaskTexture;
    if (TextureMapperNode* mask = m_state.maskLayer) {
        mask->uploadTextureFromContent(options.textureMapper);
        maskTexture = mask->m_texture;
    }

    if (replica && replica->m_state.maskLayer) {
        replica->m_state.maskLayer->uploadTextureFromContent(options.textureMapper);
        replicaMaskTexture = replica->m_state.maskLayer->m_texture;
    }

    const int childrenSize = m_children.size();
    if (wasDirty || !isSurface || m_state.tiled || !m_surface->isValid()) {
        bool didPaintSelf = false;
        if (!m_state.preserves3D || !childrenSize) {
            paintSelf(options.textureMapper, contentOpacity, maskTexture.get(), replicaMaskTexture.get(), isSurface);
            didPaintSelf = true;
        }
        if (childrenSize) {
            if (m_layerType == ScissorLayer)
                optionsForDescendants.scissorRect.intersect(m_transforms.target.mapRect(IntRect(0, 0, m_size.width(), m_size.height())));

            for (int i = 0; i < childrenSize; ++i) {
                TextureMapperNode* layer = m_children[i];
                if (!layer)
                    continue;

                if (!didPaintSelf && layer->m_transforms.centerZ >= 0) {
                    paintSelf(options.textureMapper, contentOpacity, maskTexture.get(), replicaMaskTexture.get(), isSurface);
                    didPaintSelf = true;
                }
                ++spaces;
                layer->paintRecursive(optionsForDescendants);
                --spaces;
                if (isSurface) {
                    ASSERT(m_surface);
                    gTextureMapperCache.mark(m_surface.get());
                    options.textureMapper->bindSurface(m_surface.get());
                }
            }

            if (!didPaintSelf) {
                paintSelf(options.textureMapper, contentOpacity, maskTexture.get(), replicaMaskTexture.get(), isSurface);
                didPaintSelf = true;
            }
        }
    }

#if 1
    if(LogRendering.state == WTFLogChannelOn) {
        for(int i = 0; i < spaces; ++i )
            fprintf(stderr, "  ");
        fprintf(stderr, "[TextureMapper] %d Rendered(%p/%d): %d [%dx%d] [%d %d %d %d] [%dms]\n", spaces, this, isSurface, m_layerType, m_size.width(), m_size.height(),
                optionsForDescendants.scissorRect.x(), optionsForDescendants.scissorRect.y(), optionsForDescendants.scissorRect.width(),
                optionsForDescendants.scissorRect.height(), int(stopWatch.elapsed() * 1000));
    }
#endif

    if (m_layerType == RootLayer || m_layerType == DefaultLayer || m_layerType == ScissorLayer || (!childrenSize && !isSurface))
        return;

    ASSERT(m_surface);
    BitmapTexture& texture = *m_surface.get();
    if (replica) {
        ASSERT(m_replicaSurface);
        m_replicaSurface->reset(options.surface->size());
        m_replicaSurface->setOffset(options.surface->offset());
        gTextureMapperCache.mark(m_replicaSurface.get());
        options.textureMapper->bindSurface(m_replicaSurface.get());
        options.textureMapper->drawTexture(texture, replicaRect(), m_transforms.replica, replica->m_state.opacity, replicaMaskTexture ? replicaMaskTexture.get() : maskTexture.get());
        options.textureMapper->drawTexture(texture, IntRect(IntPoint(0, 0), options.surface->size()), TransformationMatrix(), 1.0f, maskTexture.get());
        options.textureMapper->bindSurface(options.surface);
        gTextureMapperCache.mark(options.surface);
        options.textureMapper->drawTexture(*m_replicaSurface.get(), IntRect(IntPoint(0, 0), options.surface->size()), TransformationMatrix(), options.opacity, 0);
        return;
    }


    const IntRect rect = targetRect();
#if 0
    WTFLog(&LogRendering, "DrawTexture(%p): %d %d [%d %d %d %d] [%d %d %d %d]", this, m_size.width(), m_size.height(),
           rect.x(), rect.y(), rect.width(), rect.height(),
           m_transforms.targetBoundingRect.x(), m_transforms.targetBoundingRect.y(), m_transforms.targetBoundingRect.width(), m_transforms.targetBoundingRect.height());
#endif
    options.textureMapper->bindSurface(options.surface);
    options.textureMapper->drawTexture(texture,
                             m_layerType == TransparencyLayer ? IntRect(IntPoint(0, 0), options.surface->size()) : rect,
                             m_layerType == TransparencyLayer ? TransformationMatrix() : m_transforms.target,
                             options.opacity, maskTexture.get());
    gTextureMapperCache.mark(&texture);
}

void TextureMapperNode::cleanupTextureMapper()
{
    if (m_texture)
        m_texture->destroy();
    if (m_surface)
        m_surface->destroy();
    if (m_replicaSurface)
        m_replicaSurface->destroy();
    for (unsigned int i = 0; i < m_children.size(); ++i) {
        if (TextureMapperNode *child = m_children[i])
            child->cleanupTextureMapper();
    }
    if (m_lastTextureMapper)
        m_lastTextureMapper->cleanup();
}

TextureMapperNode::~TextureMapperNode()
{
    NOTIFY_CHANGE(this, ("DYING"));

    // needs active context
    setNeedsDisplay();
    {
        const int childrenSize = m_children.size();
        for (int i = childrenSize-1; i >= 0; --i) {
            ASSERT(m_children[i]->m_parent == this);
            m_children[i]->m_parent = 0;
        }
    }
    if (m_parent) {
        const size_t index = m_parent->m_children.find(this);
        if(index != notFound)
            m_parent->m_children.remove(index);
    }
}

#if ENABLE_TEXMAP_ANIMATION
void TextureMapperNode::setTransform(const TransformationMatrix& transform)
{
    setNeedsDisplay();
    m_anim.transform = transform;
    m_transforms.dirty = true;
    if (m_parent)
        m_parent->m_state.dirty = true;
    if (m_state.preserves3D)
        m_state.dirty = true;
    performPostSyncOperations();
    setNeedsDisplay();
    NOTIFY_CHANGE(this, ("%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
                         transform.m11(), transform.m12(), transform.m13(), transform.m14(),
                         transform.m21(), transform.m22(), transform.m23(), transform.m24(),
                         transform.m31(), transform.m32(), transform.m33(), transform.m34(),
                         transform.m41(), transform.m42(), transform.m43(), transform.m44(),
                         transform.a(), transform.b(), transform.c(), transform.d(), transform.e(), transform.f()));
}

void TextureMapperNode::setOpacityFromAnimationFrame(float opacity)
{
    setNeedsDisplay();
    if (m_layer)
        m_layer->setOpacity(opacity);
    m_anim.opacity = opacity;
    m_state.dirty = true;
    performPostSyncOperations();
    setNeedsDisplay();
    NOTIFY_CHANGE(this, ("%f", opacity));
}

void TextureMapperNode::notifyTransformAnimationRunning(bool running)
{
    m_layer->syncCompositingState(TextureMapperNode::SyncFlags_Force|TextureMapperNode::SyncFlags_Recurse);
    m_anim.transformAnimationRunning = running;
    if (running)
        m_anim.transform = m_state.transform;
    else
        notifyChange(TextureMapperNode::TransformChange);
}

void TextureMapperNode::notifyOpacityAnimationRunning(bool running)
{
    m_layer->syncCompositingState(TextureMapperNode::SyncFlags_Force|TextureMapperNode::SyncFlags_Recurse);
    m_anim.opacityAnimationRunning = running;
    if (running)
        m_anim.opacity = m_state.opacity;
    else
        notifyChange(TextureMapperNode::OpacityChange);
}

void TextureMapperNode::notifyAnimationStarted()
{
    if (m_layer)
        m_layer->client()->notifyAnimationStarted(m_layer, WTF::currentTime());
}

void TextureMapperNode::setTransformFromAnimationFrame(const TransformationMatrix& matrix)
{
    if (m_layer)
        m_layer->setTransform(matrix);
    setTransform(matrix);
}
#endif

void TextureMapperNode::notifyChange(ChangeMask changeMask)
{
    m_changeMask |= changeMask;
    if (!m_layer->client())
        return;
    m_layer->client()->notifySyncRequired(m_layer);
}

void TextureMapperNode::performPostSyncOperations()
{
#ifdef DEBUG_STATE_CHANGES
    WTF::StopWatch stopWatch(LogRendering.state == WTFLogChannelOn);
#endif

    m_state.hasSurfaceDescendants = false;
    m_state.descendantsWithContent = 0;
    const int children_size = m_children.size();
    for (int i = 0; i < children_size; ++i) {
        TextureMapperNode* layer = m_children[i];
        layer->computeTransformations();
        layer->performPostSyncOperations();
        if (layer->m_state.dirty)
            m_state.dirty = true;
        if(layer->m_state.hasSurfaceDescendants)
            m_state.hasSurfaceDescendants = true;
        m_state.descendantsWithContent += layer->m_state.descendantsWithContent;
    }
    if (isVisible() && hasContent())
        ++m_state.descendantsWithContent;

    const LayerType newLayerType = computeLayerType();
    if (newLayerType != m_layerType) {
        m_layerType = newLayerType;
        m_state.dirty = true;
    }
    if(m_layerType == ClipLayer || m_layerType == TransparencyLayer || m_state.replicaLayer)
        m_state.hasSurfaceDescendants = true;

    if (m_transforms.dirty && hasContent())
        setNeedsDisplay();
    if(m_transforms.dirty)
        computeTransformations();

    if (m_state.maskLayer)
        m_state.dirty = m_state.maskLayer->m_state.dirty;
    if (m_state.replicaLayer)
        m_state.dirty = m_state.replicaLayer->m_state.dirty;

    if (m_state.preserves3D)
        sortByZOrder(m_children, 0, children_size);
    if (m_state.dirty && hasContent())
        setNeedsDisplay();

#ifdef DEBUG_STATE_CHANGES
    if(LogRendering.state == WTFLogChannelOn) {
        WTFLog(&LogRendering, "PerformPostSyncOperations(%p): %dms", this, int(stopWatch.elapsed() * 1000));
        WTFLog(&LogRendering, "  LayerType(%p): %d", this, m_layerType);
        WTFLog(&LogRendering, "  MasksToBounds(%p): %d", this, m_state.masksToBounds);
        WTFLog(&LogRendering, "  Content(%p): %d/%d/%d", this, m_state.hasSurfaceDescendants, m_state.descendantsWithContent, hasContent());
        WTFLog(&LogRendering, "  Transforms(%p): %d/%d", this, m_transforms.dirty, m_transforms.localDirty);
    }
#endif
}

int TextureMapperNode::syncCompositingState(unsigned char flags)
{
    int ret = 0;
    if (!m_layer)
        return ret;
    bool needsToInvalidateTransform = false;
    const int changeMask = m_changeMask;
#ifdef DEBUG_STATE_CHANGES
    static int spaces = 0;
    WTF::StopWatch stopWatch(LogRendering.state == WTFLogChannelOn);
    if(LogRendering.state == WTFLogChannelOn) {
        for(int i = 0; i < spaces; ++i )
            fprintf(stderr, "  ");
        fprintf(stderr, "SyncCompositing(%p) %d: 0x%x\n", this, spaces, changeMask);
    }
#endif
    if (changeMask == NoChanges)
        goto afterCurrentLayerSync;
#if 0
    if(!(flags & SyncFlags_Force)) {
        if(TextureMapperLayerClient *client = getPlatformLayerClient()) {
            if(!client->canSyncNow()) {
                notifyChange(changeMask);
                return ret; //later
            }
        }
    }
#endif
    ++ret;
    m_changeMask = NoChanges;

    if(hasContent())
        setNeedsDisplay();
    if (m_parent) {
        for(TextureMapperNode *p = m_parent; p; p = p->m_parent)
            p->m_state.dirty = true;
    }
    if (changeMask & ParentChange) {
        TextureMapperNode* oldParent = m_parent;
        m_parent = toTextureMapperNode(m_layer->parent());
        if (oldParent != m_parent) {
            if (oldParent) {
                const size_t index = oldParent->m_children.find(this);
                if(index != notFound)
                    oldParent->m_children.remove(index);
            }
            if (m_parent) {
                m_parent->m_children.append(this);
                for(TextureMapperNode *p = m_parent; p; p = p->m_parent)
                    p->m_state.dirty = true;
            }
        }
    }

    if (changeMask & ChildrenChange) {
        {
            const int childrenSize = m_children.size();
            for (int i = childrenSize-1; i >= 0; --i) {
                ASSERT(m_children[i]->m_parent == this);
                m_children[i]->m_parent = 0;
            }
            m_children.clear();
        }
        for (size_t i = 0; i < m_layer->children().size(); ++i) {
            if (TextureMapperNode* child = toTextureMapperNode(m_layer->children()[i])) {
                if (!child)
                    continue;
                if (child->m_parent) {
                    const size_t index = child->m_parent->m_children.find(child);
                    if(index != notFound)
                        child->m_parent->m_children.remove(index);
                }
                m_children.append(child);
                child->m_parent = this;
            }
        }
        m_state.dirty = true;
    }

    if (changeMask & (SizeChange | ContentsRectChange)) {
        IntSize wantedSize = IntSize(m_layer->size().width(), m_layer->size().height());
        if (wantedSize.isEmpty() && m_pendingContent.contentType == HTMLContentType)
            wantedSize = IntSize(m_layer->contentsRect().width(), m_layer->contentsRect().height());

        if (wantedSize != m_size) {
            m_size = IntSize(wantedSize.width(), wantedSize.height());
            if (m_platformClient)
                m_platformClient->setSizeChanged(m_size);
            const bool needsTiling = m_size.width() > 2000 || m_size.height() > 2000;
            if (m_state.tiled != needsTiling)
                m_state.tiled = needsTiling;
            m_state.dirty = true;
        }
        m_currentContent.needsDisplay = true;
    }

    if (changeMask & MaskLayerChange)
       if (TextureMapperNode* layer = toTextureMapperNode(m_layer->maskLayer()))
           layer->m_effectTarget = this;

    if (changeMask & ReplicaLayerChange)
       if (TextureMapperNode* layer = toTextureMapperNode(m_layer->replicaLayer()))
           layer->m_effectTarget = this;

    if (changeMask & (TransformChange | SizeChange | AnchorPointChange | PositionChange))
        m_transforms.localDirty = true;

    if (changeMask & (ChildrenTransformChange | SizeChange))
        m_transforms.perspectiveDirty = true;

    if (changeMask & (ChildrenTransformChange | Preserves3DChange | TransformChange | AnchorPointChange |
                      SizeChange | ContentsRectChange | BackfaceVisibilityChange | PositionChange | MaskLayerChange |
                      DrawsContentChange | ContentChange | ReplicaLayerChange | ParentChange))    {
        // Due to the differences between the way WebCore handles transforms and the way Qt handles transforms,
        // all these elements affect the transforms of all the descendants.
        needsToInvalidateTransform = true;
    }

    if (changeMask & DisplayChange)
        m_state.dirty = true;

    m_state.maskLayer = toTextureMapperNode(m_layer->maskLayer());
    m_state.replicaLayer = toTextureMapperNode(m_layer->replicaLayer());
    m_state.pos = m_layer->position();
    m_state.anchorPoint = m_layer->anchorPoint();
    m_state.size = m_layer->size();
    m_state.transform = m_layer->transform();
    m_state.geoOrientation = m_layer->geometryOrientation();
    m_state.contentsOrientation =m_layer->contentsOrientation();
    m_state.contentsRect = m_layer->contentsRect();
    m_state.opacity = m_layer->opacity();
    m_state.preserves3D = m_layer->preserves3D();
    m_state.masksToBounds = m_layer->masksToBounds();
    m_state.drawsContent = m_layer->drawsContent();
    m_state.contentsOpaque = m_layer->contentsOpaque();
    m_state.backfaceVisibility = m_layer->backfaceVisibility();
    m_state.childrenTransform = m_layer->childrenTransform();
    m_currentContent.contentType = m_pendingContent.contentType;
    m_currentContent.image = m_pendingContent.image;
    m_currentContent.media = m_pendingContent.media;
    m_currentContent.backgroundColor = m_pendingContent.backgroundColor;
    m_currentContent.needsDisplay = m_currentContent.needsDisplay || m_pendingContent.needsDisplay;
    m_currentContent.needsDisplayRect.unite(m_pendingContent.needsDisplayRect);
    m_pendingContent.needsDisplay = false;
    m_pendingContent.needsDisplayRect = IntRect();
    setNeedsDisplay();
    afterCurrentLayerSync:
    if (needsToInvalidateTransform)
        invalidateTransform(true);

    if (m_state.maskLayer) {
        m_state.maskLayer->syncCompositingState(flags);
        if (m_state.maskLayer->m_size.isEmpty())
            m_state.maskLayer->m_size = m_size;
    }

    if (m_state.replicaLayer)
        m_state.replicaLayer->syncCompositingState(flags);

#if 0
    if (m_state.dirty && m_texture && m_texture->allowOfflineTextureUpload())
        uploadTextureFromContent(0);
#endif

    if ((flags & SyncFlags_Recurse)) {
#ifdef DEBUG_STATE_CHANGES
        ++spaces;
#endif
        const int childrenSize = m_children.size();
        for (int i = childrenSize-1; i >= 0; --i)
            ret += m_children[i]->syncCompositingState(flags);
#ifdef DEBUG_STATE_CHANGES
        --spaces;
#endif
    }

#ifdef DEBUG_STATE_CHANGES
    if(LogRendering.state == WTFLogChannelOn) {
        for(int i = 0; i < spaces; ++i )
            fprintf(stderr, "  ");
        fprintf(stderr, "~SyncCompositing(%p) %d: %dms\n", this, spaces, int(stopWatch.elapsed() * 1000));
    }
#endif
    return ret;
}

GraphicsLayerTextureMapper::GraphicsLayerTextureMapper(GraphicsLayerClient* client)
    : GraphicsLayer(client)
    , m_node(new TextureMapperNode(this))
{
}

void GraphicsLayerTextureMapper::setName(const String& name)
{
    m_node->m_name = name;
    NOTIFY_CHANGE(m_node.get(), ("%s", name.latin1().data()));
}

GraphicsLayerTextureMapper::~GraphicsLayerTextureMapper()
{
}

/* \reimp (GraphicsLayer.h): Qt is top-down
*/
GraphicsLayer::CompositingCoordinatesOrientation GraphicsLayer::compositingCoordinatesOrientation()
{
    return CompositingCoordinatesTopDown;
}

/* \reimp (GraphicsLayer.h): The current size might change, thus we need to update the whole display.
*/
void GraphicsLayerTextureMapper::setNeedsDisplay()
{
    m_node->m_pendingContent.needsDisplay = true;
    m_node->notifyChange(TextureMapperNode::DisplayChange);
    NOTIFY_CHANGE(m_node.get(), ("empty"));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setNeedsDisplayInRect(const FloatRect& rect)
{
    if (m_node->m_pendingContent.needsDisplay)
        return;
    m_node->m_pendingContent.needsDisplayRect.unite(IntRect(rect));
    m_node->notifyChange(TextureMapperNode::DisplayChange);
    NOTIFY_CHANGE(m_node.get(), ("%f %f %f %f", rect.x(), rect.y(), rect.width(), rect.height()));
}

/* Not virtual in GraphicsLayer.h
*/
void GraphicsLayerTextureMapper::setParent(GraphicsLayer* layer)
{
    m_node->notifyChange(TextureMapperNode::ParentChange);
    NOTIFY_CHANGE(m_node.get(), ("%p", layer));
}

/* \reimp (GraphicsLayer.h)
*/
bool GraphicsLayerTextureMapper::setChildren(const Vector<GraphicsLayer*>& children)
{
    m_node->notifyChange(TextureMapperNode::ChildrenChange);
    NOTIFY_CHANGE(m_node.get(), ("children"));
    for(Vector<GraphicsLayer*>::const_iterator it = children.begin(); it != children.end(); ++it)
        ((GraphicsLayerTextureMapper*)(*it))->setParent(this);
    return GraphicsLayer::setChildren(children);
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::addChild(GraphicsLayer* layer)
{
    GraphicsLayer::addChild(layer);
    ((GraphicsLayerTextureMapper*)layer)->setParent(this);
    m_node->notifyChange(TextureMapperNode::ChildrenChange);
    NOTIFY_CHANGE(m_node.get(), ("%p", TextureMapperNode::toTextureMapperNode(layer)));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::addChildAtIndex(GraphicsLayer* layer, int index)
{
    GraphicsLayer::addChildAtIndex(layer, index);
    ((GraphicsLayerTextureMapper*)layer)->setParent(this);
    m_node->notifyChange(TextureMapperNode::ChildrenChange);
    NOTIFY_CHANGE(m_node.get(), ("%p %d", TextureMapperNode::toTextureMapperNode(layer), index));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::addChildAbove(GraphicsLayer* layer, GraphicsLayer* sibling)
{
    GraphicsLayer::addChildAbove(layer, sibling);
    ((GraphicsLayerTextureMapper*)layer)->setParent(this);
    m_node->notifyChange(TextureMapperNode::ChildrenChange);
    NOTIFY_CHANGE(m_node.get(), ("%p %p", TextureMapperNode::toTextureMapperNode(layer), TextureMapperNode::toTextureMapperNode(sibling)));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::addChildBelow(GraphicsLayer* layer, GraphicsLayer* sibling)
{
    GraphicsLayer::addChildBelow(layer, sibling);
    ((GraphicsLayerTextureMapper*)layer)->setParent(this);
    m_node->notifyChange(TextureMapperNode::ChildrenChange);
    NOTIFY_CHANGE(m_node.get(), ("%p %p", TextureMapperNode::toTextureMapperNode(layer), TextureMapperNode::toTextureMapperNode(sibling)));
}

/* \reimp (GraphicsLayer.h)
*/
bool GraphicsLayerTextureMapper::replaceChild(GraphicsLayer* oldChild, GraphicsLayer* newChild)
{
    if (GraphicsLayer::replaceChild(oldChild, newChild)) {
        m_node->notifyChange(TextureMapperNode::ChildrenChange);
        ((GraphicsLayerTextureMapper*)oldChild)->setParent(0);
        ((GraphicsLayerTextureMapper*)newChild)->setParent(this);
        NOTIFY_CHANGE(m_node.get(), ("%p %p", TextureMapperNode::toTextureMapperNode(oldChild), TextureMapperNode::toTextureMapperNode(newChild)));
        return true;
    }
    return false;
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::removeFromParent()
{
    const GraphicsLayer *parent = this->parent();
    if (!parent)
        return;
    if(m_node->m_parent) {
        m_node->m_parent->notifyChange(TextureMapperNode::ChildrenChange);
        NOTIFY_CHANGE(m_node.get()->m_parent, ("CHILD %p", m_node.get()));
    }
    GraphicsLayer::removeFromParent();
    ((GraphicsLayerTextureMapper*)this)->setParent(0);
    NOTIFY_CHANGE(m_node.get(), ("%p", TextureMapperNode::toTextureMapperNode(parent)));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setMaskLayer(GraphicsLayer* value)
{
    if (value == maskLayer())
        return;
    GraphicsLayer::setMaskLayer(value);
    m_node->notifyChange(TextureMapperNode::MaskLayerChange);
    NOTIFY_CHANGE(m_node.get(), ("%p", TextureMapperNode::toTextureMapperNode(value)));
}


/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setReplicatedByLayer(GraphicsLayer* value)
{
    if (value == replicaLayer())
        return;
    GraphicsLayer::setReplicatedByLayer(value);
    m_node->notifyChange(TextureMapperNode::ReplicaLayerChange);
    NOTIFY_CHANGE(m_node.get(), ("%p", TextureMapperNode::toTextureMapperNode(value)));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setPosition(const FloatPoint& value)
{
    if (value == position())
        return;
    GraphicsLayer::setPosition(value);
    m_node->notifyChange(TextureMapperNode::PositionChange);
    NOTIFY_CHANGE(m_node.get(), ("%f %f", value.x(), value.y()));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setAnchorPoint(const FloatPoint3D& value)
{
    if (value == anchorPoint())
        return;
    GraphicsLayer::setAnchorPoint(value);
    m_node->notifyChange(TextureMapperNode::AnchorPointChange);
    NOTIFY_CHANGE(m_node.get(), ("%f %f", value.x(), value.y()));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setSize(const FloatSize& value)
{
    if (value == size())
        return;
    GraphicsLayer::setSize(value);
    m_node->notifyChange(TextureMapperNode::SizeChange);
    NOTIFY_CHANGE(m_node.get(), ("%f %f", value.width(), value.height()));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setTransform(const TransformationMatrix& value)
{
    if (value == transform())
        return;
    GraphicsLayer::setTransform(value);
    m_node->notifyChange(TextureMapperNode::TransformChange);
    NOTIFY_CHANGE(m_node.get(), ("%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
                   value.m11(), value.m12(), value.m13(), value.m14(),
                   value.m21(), value.m22(), value.m23(), value.m24(),
                   value.m31(), value.m32(), value.m33(), value.m34(),
                   value.m41(), value.m42(), value.m43(), value.m44(),
                   value.a(), value.b(), value.c(), value.d(), value.e(), value.f()));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setChildrenTransform(const TransformationMatrix& value)
{
    if (value == childrenTransform())
        return;
    GraphicsLayer::setChildrenTransform(value);
    m_node->notifyChange(TextureMapperNode::ChildrenTransformChange);
    NOTIFY_CHANGE(m_node.get(), ("%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
                   value.m11(), value.m12(), value.m13(), value.m14(),
                   value.m21(), value.m22(), value.m23(), value.m24(),
                   value.m31(), value.m32(), value.m33(), value.m34(),
                   value.m41(), value.m42(), value.m43(), value.m44(),
                   value.a(), value.b(), value.c(), value.d(), value.e(), value.f()));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setPreserves3D(bool value)
{
    if (value == preserves3D())
        return;
    GraphicsLayer::setPreserves3D(value);
    m_node->notifyChange(TextureMapperNode::Preserves3DChange);
    NOTIFY_CHANGE(m_node.get(), ("%d", value));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setMasksToBounds(bool value)
{
    if (value == masksToBounds())
        return;
    GraphicsLayer::setMasksToBounds(value);
    m_node->notifyChange(TextureMapperNode::MasksToBoundsChange);
    NOTIFY_CHANGE(m_node.get(), ("%d", value));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setDrawsContent(bool value)
{
    if (value == drawsContent())
        return;
    m_node->notifyChange(TextureMapperNode::DrawsContentChange);
    GraphicsLayer::setDrawsContent(value);
    NOTIFY_CHANGE(m_node.get(), ("%d", value));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setBackgroundColor(const Color& value)
{
    if (value == m_node->m_pendingContent.backgroundColor)
        return;
    m_node->m_pendingContent.backgroundColor = value;
    GraphicsLayer::setBackgroundColor(value);
    m_node->notifyChange(TextureMapperNode::BackgroundColorChange);
    NOTIFY_CHANGE(m_node.get(), ("0x%x", value.rgb()));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::clearBackgroundColor()
{
    if (!m_node->m_pendingContent.backgroundColor.isValid())
        return;
    m_node->m_pendingContent.backgroundColor = Color();
    GraphicsLayer::clearBackgroundColor();
    m_node->notifyChange(TextureMapperNode::BackgroundColorChange);
    NOTIFY_CHANGE(m_node.get(), ("clear"));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setContentsOpaque(bool value)
{
    if (value == contentsOpaque())
        return;
    GraphicsLayer::setContentsOpaque(value);
    m_node->notifyChange(TextureMapperNode::ContentsOpaqueChange);
    NOTIFY_CHANGE(m_node.get(), ("%d", value));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setBackfaceVisibility(bool value)
{
    if (value == backfaceVisibility())
        return;
    GraphicsLayer::setBackfaceVisibility(value);
    m_node->notifyChange(TextureMapperNode::BackfaceVisibilityChange);
    NOTIFY_CHANGE(m_node.get(), ("%d", value));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setOpacity(float value)
{
    if (value == opacity())
        return;
    GraphicsLayer::setOpacity(value);
    m_node->notifyChange(TextureMapperNode::OpacityChange);
    NOTIFY_CHANGE(m_node.get(), ("%f", value));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setContentsRect(const IntRect& value)
{
    if (value == contentsRect())
        return;
    GraphicsLayer::setContentsRect(value);
    m_node->notifyChange(TextureMapperNode::ContentsRectChange);
    NOTIFY_CHANGE(m_node.get(), ("%d %d %d %d", value.x(), value.y(), value.width(), value.height()));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setContentsToImage(Image* image)
{
    m_node->m_pendingContent.contentType = image ? TextureMapperNode::DirectImageContentType : TextureMapperNode::HTMLContentType;
    m_node->m_pendingContent.image = image;
    GraphicsLayer::setContentsToImage(image);
    m_node->notifyChange(TextureMapperNode::ContentChange);
    NOTIFY_CHANGE(m_node.get(), ("%p", image));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setContentsBackgroundColor(const Color& color)
{
    m_node->m_pendingContent.contentType = TextureMapperNode::ColorContentType;
    m_node->m_pendingContent.backgroundColor = color;
    GraphicsLayer::setContentsBackgroundColor(color);
    m_node->notifyChange(TextureMapperNode::ContentChange);
    NOTIFY_CHANGE(m_node.get(), ("0x%x", color.rgb()));
}

void GraphicsLayerTextureMapper::setContentsToMedia(PlatformLayer* media)
{
    GraphicsLayer::setContentsToMedia(media);
    m_node->m_pendingContent.contentType = media ? TextureMapperNode::MediaContentType : TextureMapperNode::HTMLContentType;
    if (media)
        m_node->m_pendingContent.media = static_cast<TextureMapperMediaLayer*>(media);
    else
        m_node->m_pendingContent.media = 0;
    m_node->notifyChange(TextureMapperNode::ContentChange);
    NOTIFY_CHANGE(m_node.get(), ("%p", media));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setGeometryOrientation(CompositingCoordinatesOrientation orientation)
{
    if (geometryOrientation() == orientation)
        return;
    GraphicsLayer::setGeometryOrientation(orientation);
    m_node->notifyChange(TextureMapperNode::GeometryOrientationChange);
    NOTIFY_CHANGE(m_node.get(), ("%d", orientation));
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::setContentsOrientation(CompositingCoordinatesOrientation orientation)
{
    if (contentsOrientation() == orientation)
        return;
    GraphicsLayer::setContentsOrientation(orientation);
    m_node->notifyChange(TextureMapperNode::ContentsOrientationChange);
    NOTIFY_CHANGE(m_node.get(), ("%d", orientation));
}

void GraphicsLayerTextureMapper::syncCompositingState(unsigned char flags)
{
    if(m_node->syncCompositingState(flags))
        m_node->performPostSyncOperations();
}

/* \reimp (GraphicsLayer.h)
*/
void GraphicsLayerTextureMapper::syncCompositingState()
{
    GraphicsLayer::syncCompositingState();
    syncCompositingState(TextureMapperNode::SyncFlags_Recurse);
}

/* \reimp (GraphicsLayer.h)
 */
NativeLayer GraphicsLayerTextureMapper::nativeLayer() const
{
    return m_node.get();
}

/* \reimp (GraphicsLayer.h)
*/
PlatformLayer* GraphicsLayerTextureMapper::platformLayer() const
{
    return m_node.get();
}

PassOwnPtr<GraphicsLayer> GraphicsLayer::create(GraphicsLayerClient* client)
{
    return new GraphicsLayerTextureMapper(client);
}

#if ENABLE_TEXMAP_ANIMATION
template <typename T>
struct KeyframeValuetextureMapper {
    TimingFunction timingFunction;
    float keyTime;
    T value;
};

/* Copied from AnimationBase.cpp
*/
static inline double solveEpsilon(double duration)
{
    return 1.0 / (200.0 * duration);
}

static inline double solveCubicBezierFunction(float p1x, float p1y, float p2x, float p2y, double t, double duration)
{
    UnitBezier bezier(p1x, p1y, p2x, p2y);
    return bezier.solve(t, solveEpsilon(duration));
}

static inline float applyTimingFunction(const TimingFunction& timingFunction, float progress, double duration)
{
    // We want the timing function to be as close as possible to what the web-developer intended, so
    // we're using the same function used by WebCore when compositing is disabled. Using easing-curves
    // would probably work for some of the cases, but wouldn't really buy us anything as we'd have to
    // convert the bezier function back to an easing curve.

    if (timingFunction.type() == LinearTimingFunction)
        return progress;
    if (timingFunction.type() == CubicBezierTimingFunction) {
        return solveCubicBezierFunction(timingFunction.x1(),
                                        timingFunction.y1(),
                                        timingFunction.x2(),
                                        timingFunction.y2(),
                                        double(progress), double(duration) / 1000);
    }
    return progress;
}

// Helper functions to safely get a value out of WebCore's AnimationValue*.

static void webkitAnimationTotextureMapperAnimationValue(const AnimationValue* animationValue, TransformOperations& transformOperations)
{
    transformOperations = TransformOperations();
    if (!animationValue)
        return;

    if (const TransformOperations* ops = static_cast<const TransformAnimationValue*>(animationValue)->value())
        transformOperations = *ops;
}

static void webkitAnimationTotextureMapperAnimationValue(const AnimationValue* animationValue, float& realValue)
{
    realValue = animationValue ? static_cast<const FloatAnimationValue*>(animationValue)->value() : 0;
}

class TexmapAnimationBase : public PlatformAnimationClient {
public:
    TexmapAnimationBase(TextureMapperNode* node, const KeyframeValueList& values, const IntSize& boxSize,
                     const Animation* anim, const String & name)
        : m_node(node)
        , m_animation(PlatformAnimation::create(this))
        , m_boxSize(boxSize)
        , m_isAlternate(anim->direction() == Animation::AnimationDirectionAlternate)
        , m_webkitPropertyID(values.property())
        , m_webkitAnimation(anim)
        , m_keyframesName(name)
        , m_fillsForwards(false)
    {
        m_animation->setAnimationDuration(anim->duration());
    }

    virtual ~TexmapAnimationBase()
    {
    }

    virtual void setAnimationStarted()
    {
        // WTFLog(&LogRendering, "++AnimationStarted: %p", m_node);
        if (m_node)
            m_node->notifyAnimationStartedAsync();
    }

    virtual void setAnimationStopped()
    {
        // WTFLog(&LogRendering, "++AnimationStopped: %p", m_node);
    }

    TextureMapperNode* m_node;
    OwnPtr<PlatformAnimation> m_animation;
    IntSize m_boxSize;
    bool m_isAlternate;
    AnimatedPropertyID m_webkitPropertyID;

    // We might need this in case the same animation is added again (i.e. resumed by WebCore).
    const Animation* m_webkitAnimation;
    String m_keyframesName;
    bool m_fillsForwards;
};

template <typename T>
class TexmapAnimation : public TexmapAnimationBase {

public:
    TexmapAnimation(TextureMapperNode* layer, const KeyframeValueList& values, const IntSize& boxSize,
                 const Animation* anim, const String & name)
        : TexmapAnimationBase(layer, values, boxSize, anim, name)
    {
        // Copying those WebCore structures is not trivial, we have to do it like this.
        for (size_t i = 0; i < values.size(); ++i) {
            const AnimationValue* animationValue = values.at(i);
            KeyframeValuetextureMapper<T> keyframeValue;
            if (animationValue->timingFunction())
                keyframeValue.timingFunction = *animationValue->timingFunction();
            else
                keyframeValue.timingFunction = anim->timingFunction();
            keyframeValue.keyTime = animationValue->keyTime();
            webkitAnimationTotextureMapperAnimationValue(animationValue, keyframeValue.value);
            m_keyframeValues.append(keyframeValue);
        }
    }

protected:

    // This is the part that differs between animated properties.
    virtual void applyFrame(const T& fromValue, const T& toValue, float progress) = 0;

    virtual void processFrame(float)
    {
        float progress = float(m_animation->animationCurrentLoopTime()) / m_webkitAnimation->duration();

        if (m_isAlternate && m_animation->animationCurrentLoop() % 2)
            progress = 1-progress;

        if (m_keyframeValues.isEmpty())
            return;

        const int size = m_keyframeValues.size();
        KeyframeValuetextureMapper<T>* lowerBound = 0;
        KeyframeValuetextureMapper<T>* upperBound = 0;
        if (progress == 1) {
            upperBound = &m_keyframeValues[size-1];
            lowerBound = &m_keyframeValues[size-2];
        } else
        for (int i = 0; i < size; ++i) {
            lowerBound = &m_keyframeValues[i];
            if (lowerBound->keyTime == progress) {
                upperBound = &m_keyframeValues[(i + 1) % size];
            } else if (lowerBound->keyTime > progress) {
                upperBound = lowerBound;
                lowerBound = &m_keyframeValues[(i - 1) % size];
            }
        }

        if (!upperBound) {
            lowerBound = &m_keyframeValues[0];
            upperBound = &m_keyframeValues[1 % size];
        }

        const KeyframeValuetextureMapper<T>& fromKeyframe = *lowerBound;
        const KeyframeValuetextureMapper<T>& toKeyframe = *upperBound;

        const TimingFunction& timingFunc = fromKeyframe.timingFunction;
        const T& fromValue = fromKeyframe.value;
        const T& toValue = toKeyframe.value;

        // Now we have a source keyframe, origin keyframe and a timing function.
        // We can now process the progress and apply the frame.
        if (progress && progress != 1 && lowerBound->keyTime != upperBound->keyTime)
            progress = applyTimingFunction(timingFunc, (progress - lowerBound->keyTime) / (upperBound->keyTime - lowerBound->keyTime),
                                           m_webkitAnimation->duration());
        applyFrame(fromValue, toValue, progress);
    }

    Vector<KeyframeValuetextureMapper<T> > m_keyframeValues;
};

class TransformTexmapAnimation : public TexmapAnimation<TransformOperations> {
public:
    TransformTexmapAnimation(TextureMapperNode* layer, const KeyframeValueList& values, const IntSize& boxSize,
                          const Animation* anim, const String & name)
        : TexmapAnimation<TransformOperations>(layer, values, boxSize, anim, name)
    {
      if(values.size() > 1) {
            const TransformOperations *t = static_cast<const TransformAnimationValue*>(values.at(0))->value();
            t->apply(boxSize,m_sourceMatrix);
        } else {
            m_sourceMatrix = m_node->m_anim.transformAnimationRunning ? m_node->m_anim.transform : m_node->m_state.transform;
      }
    }

    virtual ~TransformTexmapAnimation()
    {
        if (m_fillsForwards)
            processFrame(1);
    }

    // The idea is that we let WebCore manage the transform operations and Qt just manage the
    // animation heartbeat and the bottom-line QTransform. We gain performance, not by using
    // Transform instead of TransformationMatrix, but by proper caching of items that are
    // expensive for WebCore to render. We want the rest to be as close to WebCore's idea as possible.
    virtual void applyFrame(const TransformOperations& sourceOperations, const TransformOperations& targetOperations, float progress)
    {
        TransformationMatrix transformMatrix;

        bool validTransformLists = true;
        const unsigned int sourceOperationCount = sourceOperations.size();
        if (sourceOperationCount) {
            if (targetOperations.size() != sourceOperationCount)
                validTransformLists = false;
            else {
                for (size_t j = 0; j < sourceOperationCount && validTransformLists; ++j) {
                    if (!sourceOperations.operations()[j]->isSameType(*targetOperations.operations()[j]))
                        validTransformLists = false;
                }
            }
        }

        if (validTransformLists) {
            for (size_t i = 0; i < targetOperations.size(); ++i)
                targetOperations.operations()[i]->blend(sourceOperations.at(i), progress)->apply(transformMatrix, m_boxSize);
        } else {
            targetOperations.apply(m_boxSize, transformMatrix);
            transformMatrix.blend(m_sourceMatrix, progress);
        }
        m_node->setTransformFromAnimationFrame(transformMatrix);
    }

    virtual void setAnimationStarted()
    {
        m_node->notifyTransformAnimationRunning(true);
        TexmapAnimationBase::setAnimationStarted();
    }

    virtual void setAnimationStopped()
    {
        m_node->notifyTransformAnimationRunning(false);
        TexmapAnimationBase::setAnimationStopped();
    }

    TransformationMatrix m_sourceMatrix;
};

class OpacityTexmapAnimation : public TexmapAnimation<float> {
public:
    OpacityTexmapAnimation(TextureMapperNode* layer, const KeyframeValueList& values, const IntSize& boxSize,
                        const Animation* anim, const String& name)
         : TexmapAnimation<float>(layer, values, boxSize, anim, name)
    {
    }

    virtual ~OpacityTexmapAnimation()
    {
        if (m_fillsForwards)
            processFrame(1);
    }

    virtual void applyFrame(const float& fromValue, const float& toValue, float progress)
    {
        if (!m_node)
            return;
        const float opacity = qBound(float(0), fromValue + (toValue - fromValue) * progress, float(1));
        m_node->setOpacityFromAnimationFrame(opacity);
    }

    virtual void setAnimationStarted()
    {
        m_node->notifyOpacityAnimationRunning(true);
        TexmapAnimationBase::setAnimationStarted();
    }
    virtual void setAnimationStopped()
    {
        m_node->notifyOpacityAnimationRunning(false);
        TexmapAnimationBase::setAnimationStopped();
    }
};

void GraphicsLayerTextureMapper::removeAnimationsForKeyframes(const String& name)
{
    const int size = m_node->m_anim.active.size();
    for (int i = size - 1; i >= 0; --i) {
        TexmapAnimationBase* anim = m_node->m_anim.active[i].get();
        if (!anim || anim->m_keyframesName != String(name))
            continue;
        // We need to stop the animation right away, or it might flicker before it's deleted.
        anim->m_animation->stopAnimation();
        m_node->m_anim.active.remove(i);
    }
}

void GraphicsLayerTextureMapper::pauseAnimation(const String& name, double timeOffset)
{
    const int size = m_node->m_anim.active.size();
    for (int i = 0; i < size; ++i) {
        TexmapAnimationBase* anim = m_node->m_anim.active[i].get();
        if (!anim || anim->m_keyframesName != String(name))
            continue;
        anim->m_animation->pauseAnimation();
        anim->m_animation->setAnimationCurrentTime(timeOffset);
    }
}

void GraphicsLayerTextureMapper::suspendAnimations(double time)
{
    const int size = m_node->m_anim.active.size();
    for (int i = 0; i < size; ++i) {
        TexmapAnimationBase* anim = m_node->m_anim.active[i].get();
        if (!anim)
            continue;
        anim->m_animation->pauseAnimation();
        anim->m_animation->setAnimationCurrentTime(time);
    }
}

void GraphicsLayerTextureMapper::resumeAnimations()
{
    const int size = m_node->m_anim.active.size();
    for (int i = 0; i < size; ++i) {
        TexmapAnimationBase* anim = m_node->m_anim.active[i].get();
        anim->m_animation->resumeAnimation();
    }
}
#endif
bool GraphicsLayerTextureMapper::addAnimation(const KeyframeValueList& values, const IntSize& boxSize, const Animation* anim,
                                    const String& keyframesName, double timeOffset)
{
#if DEBUG_TEXMAP_FPS
    m_node->beginAnimation(values.property());
#endif

#if ENABLE_TEXMAP_ANIMATION
    if (!anim->duration() || !anim->iterationCount() || !PlatformAnimation::isSupported())
        return false;

    TexmapAnimationBase* newAnim = 0;

    // Fixed: we might already have the Qt animation object associated with this WebCore::Animation object.
    const int size = m_node->m_anim.active.size();
    for (int i = 0; i < size; ++i) {
        TexmapAnimationBase* otherAnim = m_node->m_anim.active[i].get();
        if (otherAnim && otherAnim->m_webkitAnimation == anim && otherAnim->m_webkitPropertyID == values.property()) {
            newAnim = otherAnim;
            break;
        }
    }

    if (!newAnim) {
        switch (values.property()) {
        case AnimatedPropertyOpacity:
            newAnim = new OpacityTexmapAnimation(m_node.get(), values, boxSize, anim, keyframesName);
            break;
        case AnimatedPropertyWebkitTransform:
            newAnim = new TransformTexmapAnimation(m_node.get(), values, boxSize, anim, keyframesName);
            break;
        default:
            return false;
        }

        // We make sure WebCore::Animation and QAnimation are on the same terms.
        newAnim->m_animation->setAnimationIterationCount(anim->iterationCount());
        newAnim->m_fillsForwards = anim->fillsForwards();
        m_node->m_anim.active.append(newAnim);
    }

    // when fill-mode is backwards/both, we set the value to 0 before the delay takes place.
    if (anim->fillsBackwards())
        newAnim->m_animation->setAnimationCurrentTime(0);
    newAnim->m_animation->startAnimation();
    newAnim->m_animation->setAnimationCurrentTime(timeOffset);
    return true;
#else
    return false;
#endif
}

void GraphicsLayerTextureMapper::removeAnimationsForProperty(AnimatedPropertyID id)
{
#if DEBUG_TEXMAP_FPS
    m_node->endAnimation(id);
#endif

#if ENABLE_TEXMAP_ANIMATION
    const int size = m_node->m_anim.active.size();
    for (int i = size-1; i >= 0; --i) {
        TexmapAnimationBase* anim = m_node->m_anim.active[i].get();
        if (!anim || anim->m_webkitPropertyID != id || !anim->m_animation)
            continue;

        // We need to stop the animation right away, or it might flicker before it's deleted.
        anim->m_animation->stopAnimation();
        m_node->m_anim.active.remove(i);
    }
#endif
}

}
