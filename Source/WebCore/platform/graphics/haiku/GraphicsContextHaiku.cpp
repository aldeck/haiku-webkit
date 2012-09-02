/*
 * Copyright (C) 2007 Ryan Leavengood <leavengood@gmail.com>
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "GraphicsContext.h"

#include "AffineTransform.h"
#include "Color.h"
#include "Font.h"
#include "FontData.h"
#include "NotImplemented.h"
#include "Path.h"
#include <wtf/text/CString.h>
#include <Bitmap.h>
#include <GraphicsDefs.h>
#include <Region.h>
#include <Shape.h>
#include <View.h>
#include <Window.h>
#include <stdio.h>


namespace WebCore {

class GraphicsContextPlatformPrivate {
public:
    GraphicsContextPlatformPrivate(BView* view);
    ~GraphicsContextPlatformPrivate();

    struct Layer {
    public:
        Layer(BView* _view)
            : view(_view)
            , bitmap(0)
            , clipping()
            , cippingSet(false)
            , globalAlpha(255)
            , currentShape(0)
            , clipShape(0)
            , locationInParent(B_ORIGIN)
            , accumulatedOrigin(B_ORIGIN)
            , previous(0)
        {
        }
        Layer(Layer* previous)
            : view(0)
            , bitmap(0)
            , clipping()
            , cippingSet(false)
            , globalAlpha(255)
            , currentShape(0)
            , clipShape(previous->clipShape ? new BShape(*previous->clipShape) : 0)
            , locationInParent(B_ORIGIN)
            , accumulatedOrigin(B_ORIGIN)
            , previous(previous)
        {
            BRegion parentClipping;
            previous->view->GetClippingRegion(&parentClipping);
            BRect frameInParent = parentClipping.Frame();
            if (!frameInParent.IsValid())
                frameInParent = previous->view->Bounds();
            BRect bounds = frameInParent.OffsetToCopy(B_ORIGIN);
            locationInParent += frameInParent.LeftTop();
            view = new BView(bounds, "WebCore transparency layer", 0, 0);
            bitmap = new BBitmap(bounds, B_RGBA32, true);
            bitmap->Lock();
            bitmap->AddChild(view);
            view->SetHighColor(0, 0, 0, 0);
            view->FillRect(view->Bounds());
            view->SetHighColor(previous->view->HighColor());
            view->SetDrawingMode(previous->view->DrawingMode());
            view->SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_COMPOSITE);
            // TODO: locationInParent and accumulatedOrigin can
            // probably somehow be merged. But for now it works.
            accumulatedOrigin.x = -frameInParent.left;
            accumulatedOrigin.y = -frameInParent.top;
            view->SetOrigin(previous->accumulatedOrigin + accumulatedOrigin);
            view->SetScale(previous->view->Scale());
            view->SetPenSize(previous->view->PenSize());
        }
        ~Layer()
        {
            if (bitmap) {
                bitmap->Unlock();
                // Deleting the bitmap will also take care of the view,
                // if there is no bitmap, the view does not belong to us (initial layer).
                delete bitmap;
            }
            delete currentShape;
            delete clipShape;
        }

        BView* view;
        BBitmap* bitmap;
        BRegion clipping;
        bool cippingSet;
        uint8 globalAlpha;
        BShape* currentShape;
        BShape* clipShape;
        BPoint locationInParent;
        BPoint accumulatedOrigin;

        Layer* previous;
    };

    struct CustomGraphicsState {
        CustomGraphicsState()
            : previous(0)
            , imageInterpolationQuality(InterpolationDefault)
        {
        }

        CustomGraphicsState(CustomGraphicsState* previous)
            : previous(previous)
            , imageInterpolationQuality(previous->imageInterpolationQuality)
        {
        }

        CustomGraphicsState* previous;
        InterpolationQuality imageInterpolationQuality;
    };

    void pushState()
    {
    	m_graphicsState = new CustomGraphicsState(m_graphicsState);
        view()->PushState();
        resetClipping();
    }

    void popState()
    {
    	ASSERT(m_graphicsState->previous);
    	if (!m_graphicsState->previous)
    	    return;

    	CustomGraphicsState* oldTop = m_graphicsState;
    	m_graphicsState = oldTop->previous;
    	delete oldTop;

        m_currentLayer->accumulatedOrigin -= view()->Origin();
        view()->PopState();
        resetClipping();
    }

    BView* view() const
    {
        return m_currentLayer->view;
    }

	uint8 globalAlpha() const
	{
		return m_currentLayer->globalAlpha;
	}

    void setClipping(const BRegion& region)
    {
    	// If we are supposed to set an empty region, but never
    	// have set any region on this layer before, comparing
    	// the two empty regions alone will prevent us from setting
    	// the clipping, that's why there is the additional "cippingSet" flag.
    	if (m_currentLayer->cippingSet && region == m_currentLayer->clipping)
    	    return;

        m_currentLayer->cippingSet = true;
    	m_currentLayer->clipping = region;
  	    m_currentLayer->view->ConstrainClippingRegion(&m_currentLayer->clipping);
    }

    void resetClipping()
    {
        m_currentLayer->cippingSet = false;
    	m_currentLayer->clipping = BRegion();
    }

    void setClipShape(BShape* shape)
    {
    	// NOTE: For proper clipping, the paths would have to
    	// be combined with the previous layers. But for now,
    	// this is just supposed to support a small hack to get
    	// box elements with round corners to work properly. BView shall
    	// get proper clipping path support in the future.
        delete m_currentLayer->clipShape;
        m_currentLayer->clipShape = shape;
    }

    BShape* clipShape() const
    {
        if (!m_currentLayer->clipShape)
            m_currentLayer->clipShape = new BShape();
        return m_currentLayer->clipShape;
    }

    void pushLayer(float opacity)
    {
        m_currentLayer = new Layer(m_currentLayer);
        m_currentLayer->globalAlpha = (uint8)(opacity * 255.0);
    }

    void popLayer()
    {
        if (!m_currentLayer->previous)
            return;
        Layer* layer = m_currentLayer;
        m_currentLayer = layer->previous;
        if (layer->globalAlpha > 0) {
            // Post process the bitmap in order to apply global alpha...
            layer->view->Sync();
            if (layer->globalAlpha < 255) {
                uint8* bits = reinterpret_cast<uint8*>(layer->bitmap->Bits());
                uint32 width = layer->bitmap->Bounds().IntegerWidth() + 1;
                uint32 height = layer->bitmap->Bounds().IntegerHeight() + 1;
                uint32 bpr = layer->bitmap->BytesPerRow();
                uint8 alpha = layer->globalAlpha;
                for (uint32 y = 0; y < height; y++) {
                    uint8* p = bits + 3;
                    for (uint32 x = 0; x < width; x++) {
                        *p = (uint8)((uint16)*p * alpha / 255);
                        p += 4;
                    }
                    bits += bpr;
                }
            }
            BPoint bitmapLocation(layer->locationInParent);
            bitmapLocation -= m_currentLayer->accumulatedOrigin;
            drawing_mode drawingMode = m_currentLayer->view->DrawingMode();
            m_currentLayer->view->SetDrawingMode(B_OP_ALPHA);
            m_currentLayer->view->DrawBitmap(layer->bitmap, bitmapLocation);
            m_currentLayer->view->SetDrawingMode(drawingMode);
        }
        delete layer;
    }

    bool inTransparencyLayer() const
    {
        return m_currentLayer->previous;
    }

    Layer* m_currentLayer;
    CustomGraphicsState* m_graphicsState;
};

GraphicsContextPlatformPrivate::GraphicsContextPlatformPrivate(BView* view)
    : m_currentLayer(new Layer(view))
    , m_graphicsState(new CustomGraphicsState)
{
}

GraphicsContextPlatformPrivate::~GraphicsContextPlatformPrivate()
{
    while (Layer* previous = m_currentLayer->previous) {
        delete m_currentLayer;
        m_currentLayer = previous;
    }
    delete m_currentLayer;

    while (CustomGraphicsState* previous = m_graphicsState->previous) {
        delete m_graphicsState;
        m_graphicsState = previous;
    }
    delete m_graphicsState;
}

void GraphicsContext::platformInit(PlatformGraphicsContext* context)
{
    m_data = new GraphicsContextPlatformPrivate(context);
    setPaintingDisabled(!context);
}

void GraphicsContext::platformDestroy()
{
    delete m_data;
}

PlatformGraphicsContext* GraphicsContext::platformContext() const
{
    return m_data->view();
}

void GraphicsContext::savePlatformState()
{
	m_data->pushState();
}

void GraphicsContext::restorePlatformState()
{
	m_data->popState();
}

// Draws a filled rectangle with a stroked border.
void GraphicsContext::drawRect(const IntRect& rect)
{
    if (paintingDisabled())
        return;

    if (m_state.fillPattern || m_state.fillGradient || fillColor().alpha()) {
//        TODO: What's this shadow business?
        if (m_state.fillPattern)
            notImplemented();
        else if (m_state.fillGradient) {
            BGradient* gradient = m_state.fillGradient->platformGradient();
//            gradient->SetTransform(m_state.fillGradient->gradientSpaceTransform());
            m_data->view()->FillRect(rect, *gradient);
        } else
            m_data->view()->FillRect(rect);
    }

    // TODO: Support gradients
    if (strokeStyle() != NoStroke && strokeThickness() > 0.0f && strokeColor().alpha())
        m_data->view()->StrokeRect(rect, getHaikuStrokeStyle());
}

// This is only used to draw borders.
void GraphicsContext::drawLine(const IntPoint& point1, const IntPoint& point2)
{
    if (paintingDisabled() || strokeStyle() == NoStroke || strokeThickness() <= 0.0f || !strokeColor().alpha())
        return;

    m_data->view()->StrokeLine(point1, point2, getHaikuStrokeStyle());
}

// This method is only used to draw the little circles used in lists.
void GraphicsContext::drawEllipse(const IntRect& rect)
{
    if (paintingDisabled())
        return;

    if (m_state.fillPattern || m_state.fillGradient || fillColor().alpha()) {
//        TODO: What's this shadow business?
        if (m_state.fillPattern)
            notImplemented();
        else if (m_state.fillGradient) {
            BGradient* gradient = m_state.fillGradient->platformGradient();
//            gradient->SetTransform(m_state.fillGradient->gradientSpaceTransform());
            m_data->view()->FillEllipse(rect, *gradient);
        } else
            m_data->view()->FillEllipse(rect);
    }

    // TODO: Support gradients
    if (strokeStyle() != NoStroke && strokeThickness() > 0.0f && strokeColor().alpha())
        m_data->view()->StrokeEllipse(rect, getHaikuStrokeStyle());
}

void GraphicsContext::strokeRect(const FloatRect& rect, float width)
{
    if (paintingDisabled() || strokeStyle() == NoStroke || strokeThickness() <= 0.0f || !strokeColor().alpha())
        return;

    float oldSize = m_data->view()->PenSize();
    m_data->view()->SetPenSize(width);
    m_data->view()->StrokeRect(rect, getHaikuStrokeStyle());
    m_data->view()->SetPenSize(oldSize);
}

void GraphicsContext::strokeArc(const IntRect& rect, int startAngle, int angleSpan)
{
    if (paintingDisabled() || strokeStyle() == NoStroke || strokeThickness() <= 0.0f || !strokeColor().alpha())
        return;

    // TODO: The code below will only make round-corner boxen look nice. For an utterly shocking
    // implementation of round corner drawing, see RenderBoxModelObject::paintBorder(). It tries
    // to use one (or two) alpha mask(s) per box corner to cut off a thicker stroke and doubles
    // the stroke with. All this to align the arc with the box sides...

    m_data->view()->PushState();
    float penSize = strokeThickness() / 2.0f;
    m_data->view()->SetPenSize(penSize);
    BRect bRect(rect.x(), rect.y(), rect.maxX(), rect.maxY());
    if (startAngle >= 0 && startAngle < 90) {
		bRect.left += penSize / 2 - 0.5;
        bRect.top += penSize / 2 - 0.5;
        bRect.right -= penSize / 2 + 0.5;
        bRect.bottom -= penSize / 2 - 0.5;
    } else if (startAngle >= 90 && startAngle < 180) {
        bRect.left += penSize / 2 - 0.5;
        bRect.top += penSize / 2 - 0.5;
        bRect.right -= penSize / 2 - 0.5;
        bRect.bottom -= penSize / 2 - 0.5;
    } else if (startAngle >= 180 && startAngle < 270) {
        bRect.left += penSize / 2 - 0.5;
        bRect.top += penSize / 2 - 0.5;
        bRect.right -= penSize / 2 - 0.5;
        bRect.bottom -= penSize / 2 + 0.5;
    } else if (startAngle >= 270 && startAngle < 360) {
        bRect.left += penSize / 2 - 0.5;
        bRect.top += penSize / 2 - 0.5;
        bRect.right -= penSize / 2 + 0.5;
        bRect.bottom -= penSize / 2 + 0.5;
    }
    uint32 flags = m_data->view()->Flags();
    m_data->view()->SetFlags(flags | B_SUBPIXEL_PRECISE);
    m_data->view()->SetDrawingMode(B_OP_ALPHA);
    m_data->view()->StrokeArc(bRect, startAngle, angleSpan, getHaikuStrokeStyle());
    m_data->view()->SetFlags(flags);

    m_data->view()->PopState();
}

void GraphicsContext::strokePath(const Path& path)
{
    if (paintingDisabled())
        return;

    m_data->view()->MovePenTo(B_ORIGIN);

    if (m_state.strokePattern || m_state.strokeGradient || strokeColor().alpha()) {
        if (m_state.strokePattern)
            notImplemented();
        else if (m_state.strokeGradient) {
            notImplemented();
//            BGradient* gradient = m_state.strokeGradient->platformGradient();
//            gradient->SetTransform(m_state.fillGradient->gradientSpaceTransform());
//            m_data->view()->StrokeShape(m_data->shape(), *gradient);
        } else {
            drawing_mode mode = m_data->view()->DrawingMode();
            if (m_data->view()->HighColor().alpha < 255)
                m_data->view()->SetDrawingMode(B_OP_ALPHA);

            m_data->view()->StrokeShape(path.platformPath());
            m_data->view()->SetDrawingMode(mode);
        }
    }
}

void GraphicsContext::drawConvexPolygon(size_t pointsLength, const FloatPoint* points, bool shouldAntialias)
{
    if (paintingDisabled())
        return;

    BPoint bPoints[pointsLength];
    for (size_t i = 0; i < pointsLength; i++)
        bPoints[i] = points[i];

    m_data->view()->FillPolygon(bPoints, pointsLength);
    if (strokeStyle() != NoStroke)
        // Stroke with low color
        m_data->view()->StrokePolygon(bPoints, pointsLength, true, getHaikuStrokeStyle());
}

void GraphicsContext::clipConvexPolygon(size_t numPoints, const FloatPoint* points, bool antialiased)
{
    // FIXME: implement
    if (paintingDisabled())
        return;

    notImplemented();
}

void GraphicsContext::fillRect(const FloatRect& rect, const Color& color, ColorSpace colorSpace)
{
    if (paintingDisabled())
        return;

    rgb_color previousColor = m_data->view()->HighColor();
    drawing_mode previousMode = m_data->view()->DrawingMode();

    m_data->view()->SetHighColor(color);
    // NOTE: having a clipShape means we're filling a rounded rect
    // thus needing alpha blending for antialiasing
    if (color.hasAlpha() || m_data->clipShape())
        m_data->view()->SetDrawingMode(B_OP_ALPHA);
    fillRect(rect);

    m_data->view()->SetHighColor(previousColor);
    m_data->view()->SetDrawingMode(previousMode);
}

void GraphicsContext::fillRect(const FloatRect& rect)
{
    if (paintingDisabled())
        return;

    // NOTE: Trick to implement filling rects with clipping path
    // (needed for box elements with round corners):
    // When the rect extends outside the current clipping path on
    // all sides, then we can fill the path instead of the rect.
    // (Clipping paths are simply not supported yet for anything else.)
    if (m_data->clipShape()) {
    	BRect bRect(rect);
    	BRect clipPathBounds(m_data->clipShape()->Bounds());
    	// NOTE: BShapes do not suffer the weird coordinate mixup
    	// of other drawing primitives, since the conversion would be
    	// too expensive in the app_server. Thus the right/bottom edge
    	// can be considered one pixel smaller. On screen, it will be
    	// the same again.
    	clipPathBounds.right--;
    	clipPathBounds.bottom--;
    	if (clipPathBounds.IsValid() && bRect.Contains(clipPathBounds)) {
    		m_data->view()->MovePenTo(B_ORIGIN);
    		m_data->view()->FillShape(m_data->clipShape());
    		return;
    	}
    }
    m_data->view()->FillRect(rect);
}

void GraphicsContext::fillRoundedRect(const IntRect& rect, const IntSize& topLeft, const IntSize& topRight, const IntSize& bottomLeft, const IntSize& bottomRight, const Color& color, ColorSpace colorSpace)
{
    if (paintingDisabled() || !color.alpha())
        return;

    BPoint points[3];
    const float kRadiusBezierScale = 1.0f - 0.5522847498f; //  1 - (sqrt(2) - 1) * 4 / 3

    BShape shape;
    shape.MoveTo(BPoint(rect.x() + topLeft.width(), rect.y()));
    shape.LineTo(BPoint(rect.maxX() - topRight.width(), rect.y()));
    points[0].x = rect.maxX() - kRadiusBezierScale * topRight.width();
    points[0].y = rect.y();
    points[1].x = rect.maxX();
    points[1].y = rect.y() + kRadiusBezierScale * topRight.height();
    points[2].x = rect.maxX();
    points[2].y = rect.y() + topRight.height();
    shape.BezierTo(points);
    shape.LineTo(BPoint(rect.maxX(), rect.maxY() - bottomRight.height()));
    points[0].x = rect.maxX();
    points[0].y = rect.maxY() - kRadiusBezierScale * bottomRight.height();
    points[1].x = rect.maxX() - kRadiusBezierScale * bottomRight.width();
    points[1].y = rect.maxY();
    points[2].x = rect.maxX() - bottomRight.width();
    points[2].y = rect.maxY();
    shape.BezierTo(points);
    shape.LineTo(BPoint(rect.x() + bottomLeft.width(), rect.maxY()));
    points[0].x = rect.x() + kRadiusBezierScale * bottomLeft.width();
    points[0].y = rect.maxY();
    points[1].x = rect.x();
    points[1].y = rect.maxY() - kRadiusBezierScale * bottomRight.height();
    points[2].x = rect.x();
    points[2].y = rect.maxY() - bottomRight.height();
    shape.BezierTo(points);
    shape.LineTo(BPoint(rect.x(), rect.y() + topLeft.height()));
    points[0].x = rect.x();
    points[0].y = rect.y() + kRadiusBezierScale * topLeft.height();
    points[1].x = rect.x() + kRadiusBezierScale * topLeft.width();
    points[1].y = rect.y();
    points[2].x = rect.x() + topLeft.width();
    points[2].y = rect.y();
    shape.BezierTo(points);
    shape.Close();

    rgb_color oldColor = m_data->view()->HighColor();
    m_data->view()->SetHighColor(color);
    m_data->view()->MovePenTo(B_ORIGIN);
    m_data->view()->FillShape(&shape);
    m_data->view()->SetHighColor(oldColor);
}

void GraphicsContext::fillPath(const Path& path)
{
    if (paintingDisabled())
        return;

//    m_data->view()->SetFillRule(toHaikuFillRule(fillRule()));
    m_data->view()->MovePenTo(B_ORIGIN);

    if (m_state.fillPattern || m_state.fillGradient || fillColor().alpha()) {
//        drawFilledShadowPath(this, p, path); TODO: What's this shadow business?
        if (m_state.fillPattern)
            notImplemented();
        else if (m_state.fillGradient) {
            BGradient* gradient = m_state.fillGradient->platformGradient();
//            gradient->SetTransform(m_state.fillGradient->gradientSpaceTransform());
            m_data->view()->FillShape(path.platformPath(), *gradient);
        } else {
            drawing_mode mode = m_data->view()->DrawingMode();
            if (m_data->view()->HighColor().alpha < 255)
                m_data->view()->SetDrawingMode(B_OP_ALPHA);

            m_data->view()->FillShape(path.platformPath());
            m_data->view()->SetDrawingMode(mode);
        }
    }
}

void GraphicsContext::clipPath(const Path&, WindRule clipRule)
{
    if (paintingDisabled())
        return;

    notImplemented();
}

void GraphicsContext::clip(const FloatRect& rect)
{
    if (paintingDisabled())
        return;

    m_data->setClipping(BRegion(rect));
}

void GraphicsContext::clip(const Path& path)
{
    if (paintingDisabled())
        return;

    if (path.platformPath()->Bounds().IsValid())
        m_data->setClipShape(new BShape(*path.platformPath()));
    else
        m_data->setClipShape(0);

    // FIXME: Support actual clipping paths in the BView API...
    FloatRect rect(path.platformPath()->Bounds());
    clip(rect);
}

void GraphicsContext::canvasClip(const Path& path)
{
    clip(path);
}

void GraphicsContext::clipOut(const Path& path)
{
    if (paintingDisabled())
        return;

    notImplemented();
}

void GraphicsContext::clipOut(const IntRect& rect)
{
    if (paintingDisabled())
        return;

    BRegion region(m_data->view()->Bounds());
    region.Include(rect);
    m_data->setClipping(region);
}

void GraphicsContext::addInnerRoundedRectClip(const IntRect& rect, int thickness)
{
    if (paintingDisabled())
        return;

    // NOTE: Used by RenderBoxModelObject to clip out the inner part of an arc when rending box corners...

    notImplemented();
}

void GraphicsContext::drawFocusRing(const Path& path, int width, int offset, const Color& color)
{
    if (paintingDisabled())
        return;

    notImplemented();
}

void GraphicsContext::drawFocusRing(const Vector<IntRect>& rects, int /* width */, int /* offset */, const Color& color)
{
    if (paintingDisabled())
        return;

    unsigned rectCount = rects.size();

    // FIXME: maybe we should implement this with BShape?

    if (rects.size() > 1) {
        BRegion    region;
        for (unsigned i = 0; i < rectCount; ++i)
            region.Include(BRect(rects[i]));

        m_data->view()->SetHighColor(color);
        m_data->view()->StrokeRect(region.Frame(), B_MIXED_COLORS);
    }
}

void GraphicsContext::drawLineForText(const FloatPoint& origin, float width, bool printing)
{
    if (paintingDisabled())
        return;

    FloatPoint endPoint = origin + FloatSize(width, 0);
    drawLine(IntPoint(origin), IntPoint(endPoint));
}

void GraphicsContext::drawLineForDocumentMarker(const FloatPoint&, float /* width */, DocumentMarkerLineStyle /* style */)
{
    if (paintingDisabled())
        return;

    notImplemented();
}

FloatRect GraphicsContext::roundToDevicePixels(const FloatRect& rect, RoundingMode /* mode */)
{
    FloatRect rounded(rect);
    rounded.setX(roundf(rect.x()));
    rounded.setY(roundf(rect.y()));
    rounded.setWidth(roundf(rect.width()));
    rounded.setHeight(roundf(rect.height()));
    return rounded;
}

void GraphicsContext::beginPlatformTransparencyLayer(float opacity)
{
    if (paintingDisabled())
        return;

    m_data->pushLayer(opacity);
}

void GraphicsContext::endPlatformTransparencyLayer()
{
    if (paintingDisabled())
        return;

    m_data->popLayer();
}

bool GraphicsContext::supportsTransparencyLayers()
{
    return true;
}

void GraphicsContext::clearRect(const FloatRect& rect)
{
    if (paintingDisabled())
        return;

    m_data->view()->PushState();
    m_data->view()->SetHighColor(0, 0, 0, 0);
    m_data->view()->SetDrawingMode(B_OP_COPY);
    m_data->view()->FillRect(rect);
    m_data->view()->PopState();
}

void GraphicsContext::setLineCap(LineCap lineCap)
{
    if (paintingDisabled())
        return;

    cap_mode mode = B_BUTT_CAP;
    switch (lineCap) {
    case RoundCap:
        mode = B_ROUND_CAP;
        break;
    case SquareCap:
        mode = B_SQUARE_CAP;
        break;
    case ButtCap:
    default:
        break;
    }

    m_data->view()->SetLineMode(mode, m_data->view()->LineJoinMode(), m_data->view()->LineMiterLimit());
}

void GraphicsContext::setLineDash(const DashArray& dashes, float dashOffset)
{
    notImplemented();
}

void GraphicsContext::setLineJoin(LineJoin lineJoin)
{
    if (paintingDisabled())
        return;

    join_mode mode = B_MITER_JOIN;
    switch (lineJoin) {
    case RoundJoin:
        mode = B_ROUND_JOIN;
        break;
    case BevelJoin:
        mode = B_BEVEL_JOIN;
        break;
    case MiterJoin:
    default:
        break;
    }

    m_data->view()->SetLineMode(m_data->view()->LineCapMode(), mode, m_data->view()->LineMiterLimit());
}

void GraphicsContext::setMiterLimit(float limit)
{
    if (paintingDisabled())
        return;

    m_data->view()->SetLineMode(m_data->view()->LineCapMode(), m_data->view()->LineJoinMode(), limit);
}

void GraphicsContext::setAlpha(float opacity)
{
    if (paintingDisabled())
        return;

    m_data->m_currentLayer->globalAlpha = (uint8)(opacity * 255.0f);
}

void GraphicsContext::setPlatformCompositeOperation(CompositeOperator op)
{
    if (paintingDisabled())
        return;

    drawing_mode mode = B_OP_COPY;
    switch (op) {
    case CompositeClear:
    case CompositeCopy:
        // Use the default above
        break;
    case CompositeSourceOver:
        mode = B_OP_OVER;
        break;
    case CompositePlusLighter:
        mode = B_OP_ADD;
        break;
    case CompositePlusDarker:
        mode = B_OP_SUBTRACT;
        break;
    default:
        printf("GraphicsContext::setCompositeOperation: Unsupported composite operation %s\n",
                compositeOperatorName(op).utf8().data());
    }
    m_data->view()->SetDrawingMode(mode);
}

AffineTransform GraphicsContext::getCTM() const
{
    // TODO: Maybe this needs to use the accumulated transform?
    AffineTransform matrix;
    BPoint origin = m_data->view()->Origin();
    matrix.translate(origin.x, origin.y);
    matrix.scale(m_data->view()->Scale());
    return matrix;
}

void GraphicsContext::translate(float x, float y)
{
    if (paintingDisabled())
        return;

    m_data->m_currentLayer->accumulatedOrigin.x += x;
    m_data->m_currentLayer->accumulatedOrigin.y += y;
    BPoint origin(m_data->view()->Origin());
    m_data->view()->SetOrigin(origin.x + x, origin.y + y);

    // TODO: currentPath needs to be translated along, according to Qt implementation
}

void GraphicsContext::rotate(float radians)
{
    if (paintingDisabled())
        return;

    notImplemented();
}

void GraphicsContext::scale(const FloatSize& size)
{
    if (paintingDisabled())
        return;

    // NOTE: Non-uniform scaling not supported on Haiku, yet.
    m_data->view()->SetScale((size.width() + size.height()) / 2);
}

void GraphicsContext::concatCTM(const AffineTransform& transform)
{
    if (paintingDisabled())
        return;

    notImplemented();
}

void GraphicsContext::setCTM(const AffineTransform& transform)
{
    if (paintingDisabled())
        return;

    notImplemented();
}

void GraphicsContext::setPlatformShouldAntialias(bool enable)
{
    if (paintingDisabled())
        return;

    notImplemented();
}

void GraphicsContext::setImageInterpolationQuality(InterpolationQuality quality)
{
    m_data->m_graphicsState->imageInterpolationQuality = quality;
}

InterpolationQuality GraphicsContext::imageInterpolationQuality() const
{
    return m_data->m_graphicsState->imageInterpolationQuality;
}

void GraphicsContext::setURLForRect(const KURL& link, const IntRect& destRect)
{
    notImplemented();
}

void GraphicsContext::setPlatformFont(const Font& font)
{
    m_data->view()->SetFont(font.primaryFont()->platformData().font());
}

void GraphicsContext::setPlatformStrokeColor(const Color& color, ColorSpace colorSpace)
{
    if (paintingDisabled())
        return;

    // NOTE: In theory, we should be able to use the low color and
    // return B_SOLID_LOW for the SolidStroke case in getHaikuStrokeStyle()
    // below. More stuff needs to be fixed, though, it will for example
    // prevent the text caret from rendering.
//    m_data->view()->SetLowColor(color);
    m_data->view()->SetHighColor(color);
}

bool GraphicsContext::inTransparencyLayer() const
{
	return m_data->inTransparencyLayer();
}

pattern GraphicsContext::getHaikuStrokeStyle()
{
    switch (strokeStyle()) {
    case SolidStroke:
        return B_SOLID_HIGH;
        break;
    case DottedStroke:
        return B_MIXED_COLORS;
        break;
    case DashedStroke:
        // FIXME: use a better dashed stroke!
        notImplemented();
        return B_MIXED_COLORS;
        break;
    default:
        return B_SOLID_LOW;
        break;
    }
}

void GraphicsContext::setPlatformStrokeStyle(StrokeStyle /* strokeStyle */)
{
    // FIXME: see getHaikuStrokeStyle.
    notImplemented();
}

void GraphicsContext::setPlatformStrokeThickness(float thickness)
{
    if (paintingDisabled())
        return;

    m_data->view()->SetPenSize(thickness);
}

void GraphicsContext::setPlatformFillColor(const Color& color, ColorSpace colorSpace)
{
    if (paintingDisabled())
        return;

    m_data->view()->SetHighColor(color);
}

void GraphicsContext::clearPlatformShadow()
{
    notImplemented();
}

void GraphicsContext::setPlatformShadow(FloatSize const&, float, Color const&, ColorSpace)
{
    notImplemented();
}

} // namespace WebCore

