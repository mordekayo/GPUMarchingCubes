#pragma once
#include "IRenderer2D.h"
#include <d2d1_1.h>
#include <dwrite.h>
#include <vector>

class CustomColorRenderer;

class Renderer2DImpl : public IRenderer2D
{
	ID2D1Factory1*		factory			= nullptr;
	IDWriteFactory*		writeFactory	= nullptr;
	ID2D1RenderTarget*	renderTarget2D	= nullptr;

	ID2D1SolidColorBrush*	sceneColorBrush = nullptr;
	ID2D1SolidColorBrush*	greenBrush		= nullptr;
	ID2D1SolidColorBrush*	redBrush		= nullptr;

	ID2D1LinearGradientBrush*		linBrush	= nullptr;
	ID2D1RadialGradientBrush*		radBrush	= nullptr;
	ID2D1RoundedRectangleGeometry*	recGeometry = nullptr;

	ID2D1BitmapBrush*	bitBrush	= nullptr;

	ID2D1PathGeometry*	path	= nullptr;
	ID2D1StrokeStyle*	stroke	= nullptr;
	
	IDWriteTextFormat* textFormat = nullptr;
	IDWriteTextLayout* textLayout = nullptr;
	IDWriteTextLayout* testLayout = nullptr;

	IDWriteTypography* typography = nullptr;

	CustomColorRenderer* customTextRenderer = nullptr;
	
	ID2D1Bitmap* bit = nullptr;

	std::vector<IDWriteTextLayout*> onScreenMessages;
	
	
public:
	virtual void Initialize() override;
	virtual void UnloadResources() override;
	virtual void SetTarget(ID3D11RenderTargetView* rtv) override;
	virtual void DrawSomething() override;
	virtual void DrawOnScreenMessage(std::wstring& message) override;
	virtual void Draw() override;
	virtual void ClearMessages() override;

	ID2D1Bitmap* LoadImageWic(LPCWSTR fileName);
};


class CustomColorRenderer : public IDWriteTextRenderer
{
	ID2D1RenderTarget* renderTarget;
	ID2D1SolidColorBrush* defaultBrush;

public:

	void AssignResources(ID2D1RenderTarget* renderTarget, ID2D1SolidColorBrush* defaultBrush);


	virtual HRESULT DrawGlyphRun(void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY,
		DWRITE_MEASURING_MODE measuringMode, DWRITE_GLYPH_RUN const* glyphRun,
		DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription, IUnknown* clientDrawingEffect) override;



	virtual HRESULT DrawUnderline(void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY,
		DWRITE_UNDERLINE const* underline, IUnknown* clientDrawingEffect) override;

	virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override { return 0; };
	virtual ULONG AddRef() override { return 0; };
	virtual ULONG Release() override { return 0; };
	virtual HRESULT IsPixelSnappingDisabled(void* clientDrawingContext, BOOL* isDisabled) override { return 0; };
	virtual HRESULT GetCurrentTransform(void* clientDrawingContext, DWRITE_MATRIX* transform) override { return 0; };
	virtual HRESULT GetPixelsPerDip(void* clientDrawingContext, FLOAT* pixelsPerDip) override { return 0; };
	virtual HRESULT DrawStrikethrough(void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY,
		DWRITE_STRIKETHROUGH const* strikethrough, IUnknown* clientDrawingEffect) override {
		return 0;
	};
	virtual HRESULT DrawInlineObject(void* clientDrawingContext, FLOAT originX, FLOAT originY,
		IDWriteInlineObject* inlineObject, BOOL isSideways, BOOL isRightToLeft, IUnknown* clientDrawingEffect) override {
		return 0;
	};
};

