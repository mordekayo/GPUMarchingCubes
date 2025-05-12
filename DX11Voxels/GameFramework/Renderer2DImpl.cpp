// ReSharper disable CppClangTidyClangDiagnosticMissingFieldInitializers
#include "Renderer2DImpl.h"
#include "Game.h"
#include <wincodec.h>
#include "InputDevice.h"



void Renderer2DImpl::Initialize()
{
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED, 
		__uuidof(IDWriteFactory), 
		reinterpret_cast<IUnknown**>(&writeFactory)
	);
	
	D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&factory
	);

	D2D1_ROUNDED_RECT roundRect = {
		D2D1_RECT_F {
			0,
			0,
			400,
			200
		},
		32,
		32
	};
	//
	factory->CreateRoundedRectangleGeometry(roundRect, &recGeometry);	
	//
	writeFactory->CreateTextFormat(
		L"Gabriola",
		nullptr,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 
		32, 
		L"en-us", 
		&textFormat
	);

	writeFactory->CreateTextLayout(
		L"テスト", 
		3, 
		textFormat, 
		512, 
		100,
		&textLayout);

	//////////////////////
	std::wstring introText = L"Hello from DirectWrite, this is a long text to show some more advanced features like paragraph alignment, custom drawing...";
	writeFactory->CreateTextLayout(
		introText.c_str(),
		introText.size(),
		textFormat,
		512,
		300,
		&testLayout);

	writeFactory->CreateTypography(&typography);

	typography->AddFontFeature(DWRITE_FONT_FEATURE { DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_7, 1 });

	testLayout->SetUnderline(true, DWRITE_TEXT_RANGE{ 0, 5 });
	testLayout->SetUnderline(true, DWRITE_TEXT_RANGE{ 11, 7 });
	testLayout->SetFontSize(24, DWRITE_TEXT_RANGE{ 6, 4 });
	testLayout->SetFontFamilyName(L"Comic Sans MS", DWRITE_TEXT_RANGE{ 11, 7 });
	testLayout->SetFontWeight(DWRITE_FONT_WEIGHT_BOLD, { 38, 14 });
	testLayout->SetTypography(typography, { 55, 20 });

	//////////////////
	float dashes[] = { 1.0f, 5.0f, 2.0f, 6.0f };
	
	factory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_FLAT,
			D2D1_CAP_STYLE_FLAT,
			D2D1_CAP_STYLE_TRIANGLE,
			D2D1_LINE_JOIN_MITER,
			10.0f,
			D2D1_DASH_STYLE_CUSTOM,
			0.0f),
		dashes,
		ARRAYSIZE(dashes),
		&stroke);

	factory->CreatePathGeometry(&path);

	ID2D1GeometrySink* sink;
	path->Open(&sink);

	sink->BeginFigure({ 100, 300 }, D2D1_FIGURE_BEGIN_FILLED);
	sink->AddLine({ 100, 100 });
	sink->AddLine({ 300, 100 });
	sink->AddLine({ 300, 300 });
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	
	sink->BeginFigure({ 150, 250 }, D2D1_FIGURE_BEGIN_FILLED);
	sink->AddLine({ 150, 150 });
	sink->AddLine({ 250, 150 });
	sink->AddLine({ 250, 250 });

	sink->AddArc(
		D2D1_ARC_SEGMENT{
			D2D1_POINT_2F{150, 250},
			D2D1_SIZE_F{100, 120},
			0,
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_LARGE
		});
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	
	recGeometry->Outline(nullptr, sink);
	
	sink->Close();
	sink->Release();
}


void Renderer2DImpl::UnloadResources()
{
	SafeRelease(renderTarget2D);
	SafeRelease(sceneColorBrush);
	SafeRelease(greenBrush);
	SafeRelease(redBrush);
	SafeRelease(bit);
}


void Renderer2DImpl::SetTarget(ID3D11RenderTargetView* rtv)
{
	const auto dpi = GetDpiForWindow(Game::Instance->Display->hWnd);
	factory->ReloadSystemMetrics();

	
	ID3D11Resource* res;
	rtv->GetResource(&res);

	IDXGISurface* surface;
	res->QueryInterface(__uuidof(IDXGISurface), reinterpret_cast<void**>(&surface));

	factory->CreateDxgiSurfaceRenderTarget(
		surface, 
		D2D1_RENDER_TARGET_PROPERTIES{
			D2D1_RENDER_TARGET_TYPE_HARDWARE,
			D2D1_PIXEL_FORMAT{DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED},
			static_cast<FLOAT>(dpi),
			static_cast<FLOAT>(dpi)
		}, 
		&renderTarget2D);

	res->Release();
	surface->Release();

	renderTarget2D->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	renderTarget2D->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

	renderTarget2D->CreateSolidColorBrush(D2D1_COLOR_F{ 1.0f, 1.0f, 1.0f, 1.0f }, &sceneColorBrush);
	renderTarget2D->CreateSolidColorBrush(
		D2D1_COLOR_F{ 0.0f, 1.0f, 0.0f, 1.0f }, 
		D2D1_BRUSH_PROPERTIES{1.0f, D2D1_MATRIX_3X2_F{1,0, 0,1, 0,0}},
		&greenBrush);
	renderTarget2D->CreateSolidColorBrush(D2D1_COLOR_F{ 1.0f, 0.0f, 0.0f, 1.0f }, &redBrush);

	testLayout->SetDrawingEffect(greenBrush, DWRITE_TEXT_RANGE{ 20, 10 });
	

	ID2D1GradientStopCollection* gsc;
	D2D1_GRADIENT_STOP stops[] = {
		D2D1_GRADIENT_STOP{0, {1.0f, 0.0f, 0.0f, 1.0f}},
		D2D1_GRADIENT_STOP{0.5f, {0.0f, 0.0f, 1.0f, 1.0f}},
		D2D1_GRADIENT_STOP{1, {0.0f, 1.0f, 0.0f, 1.0f}} };
	renderTarget2D->CreateGradientStopCollection(
		stops,
		ARRAYSIZE(stops), 
		D2D1_GAMMA_2_2, 
		D2D1_EXTEND_MODE_MIRROR, 
		&gsc
	);

	renderTarget2D->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
			D2D1::Point2F(0, 0),
			D2D1::Point2F(150, 150)),
		D2D1_BRUSH_PROPERTIES{ 1.0f, {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f} },
		gsc,
		&linBrush
	);

	renderTarget2D->CreateRadialGradientBrush(
		D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES{},
		D2D1_BRUSH_PROPERTIES{ 1.0f, {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f} },
		gsc,
		&radBrush
	);

	gsc->Release();

	customTextRenderer = new CustomColorRenderer();
	customTextRenderer->AssignResources(renderTarget2D, redBrush);
	

	//bit = LoadImageWic(L"D:/img.jpg");
	//
	//renderTarget2D->CreateBitmapBrush(bit, 
	//	D2D1_BITMAP_BRUSH_PROPERTIES{
	//		D2D1_EXTEND_MODE_CLAMP,
	//		D2D1_EXTEND_MODE_MIRROR,
	//		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR }, 
	//	D2D1_BRUSH_PROPERTIES{0.8f, D2D1::Matrix3x2F::Scale(0.1f, 0.1f)}, &bitBrush);
	
}


void Renderer2DImpl::DrawSomething()
{
	if (!renderTarget2D) return;

	renderTarget2D->BeginDraw();
	//renderTarget2D->PushLayer(
	//	D2D1::LayerParameters(D2D1::InfiniteRect(), recGeometry),
	//	nullptr
	//);
	
	D2D1_ROUNDED_RECT rect;
	recGeometry->GetRoundedRect(&rect);
	linBrush->SetStartPoint({ rect.rect.left, rect.rect.top });
	linBrush->SetEndPoint({ rect.rect.right, rect.rect.bottom});
	
	renderTarget2D->SetTransform(D2D1::Matrix3x2F::Translation(100, 300));
	renderTarget2D->FillGeometry(recGeometry, linBrush);

	renderTarget2D->SetTransform(D2D1::Matrix3x2F::Translation(100, 520));
	renderTarget2D->DrawGeometry(recGeometry, linBrush, 10, stroke);
	renderTarget2D->SetTransform(D2D1::Matrix3x2F::Identity());

	////////
	renderTarget2D->DrawTextLayout(D2D1::Point2F(0, 0), textLayout, sceneColorBrush);
	testLayout->Draw(nullptr, customTextRenderer, 0, 30);

	////////
	auto size = renderTarget2D->GetSize();
	auto center = D2D1::Point2F(size.width / 2.0f, size.height / 2.0f);
	radBrush->SetCenter(center);

	const auto mOffsets = Game::Instance->InputDevice->MousePosition - DirectX::SimpleMath::Vector2(center.x, center.y);
	radBrush->SetGradientOriginOffset( D2D1::Point2F(mOffsets.x, mOffsets.y) );
	radBrush->SetRadiusX(200);
	radBrush->SetRadiusY(200);
	//renderTarget2D->FillRectangle(D2D1::RectF(0, 0, 
	//	renderTarget2D->GetSize().width, 
	//	renderTarget2D->GetSize().height), radBrush);

	renderTarget2D->SetTransform(D2D1::Matrix3x2F::Translation(600, 100));
	
	renderTarget2D->FillGeometry(path, bitBrush, nullptr);
	renderTarget2D->DrawGeometry(path, greenBrush, 3, stroke);
	
	renderTarget2D->SetTransform(D2D1::Matrix3x2F::Identity());
	
	renderTarget2D->DrawBitmap(bit, D2D1::RectF(size.width - 300, size.height - 200, size.width, size.height), 1.0f);
	
	//renderTarget2D->PopLayer();
	renderTarget2D->EndDraw();
}


void Renderer2DImpl::DrawOnScreenMessage(std::wstring& message)
{
	IDWriteTextLayout* mess;
	writeFactory->CreateTextLayout(message.c_str(), message.size(), textFormat, 1000, 1000, &mess);
	onScreenMessages.push_back(mess);
}


void Renderer2DImpl::Draw()
{
	if (!renderTarget2D) return;
	
	renderTarget2D->BeginDraw();
	
	int i = 0;
	for (auto& mess : onScreenMessages)
	{
		renderTarget2D->DrawTextLayout({ 5.0f, 25.0f * i++ }, mess, sceneColorBrush);
	}

	renderTarget2D->EndDraw();
}


void Renderer2DImpl::ClearMessages()
{
	for (auto& mess : onScreenMessages)
	{
		mess->Release();
	}
	onScreenMessages.clear();
}


ID2D1Bitmap* Renderer2DImpl::LoadImageWic(LPCWSTR fileName)
{
	ID2D1Bitmap* bitmap = nullptr;

	//CoInitialize(nullptr);
	
	IWICImagingFactory* wicFactory;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));

	IWICBitmapDecoder* decoder;

	wicFactory->CreateDecoderFromFilename(fileName, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);

	IWICBitmapFrameDecode* frame;
	hr = decoder->GetFrame(0, &frame);

	IWICFormatConverter* converter;
	wicFactory->CreateFormatConverter(&converter);

	converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom);

	D2D1_BITMAP_PROPERTIES props = {};
	props.dpiX = 96;
	props.dpiY = 96;
	props.pixelFormat = { DXGI_FORMAT_R8G8B8A8_UINT, D2D1_ALPHA_MODE_PREMULTIPLIED };
	
	renderTarget2D->CreateBitmapFromWicBitmap(converter, nullptr, &bitmap);

	wicFactory->Release();
	decoder->Release();
	frame->Release();
	converter->Release();
	
	
	return bitmap;
}











void CustomColorRenderer::AssignResources(ID2D1RenderTarget* renderTarget, ID2D1SolidColorBrush* defaultBrush)
{
	this->renderTarget = renderTarget;
	this->defaultBrush = defaultBrush;
}


HRESULT CustomColorRenderer::DrawGlyphRun(void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode, DWRITE_GLYPH_RUN const* glyphRun,
	DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription, IUnknown* clientDrawingEffect)
{
	ID2D1SolidColorBrush* sb = defaultBrush;
	ID2D1SolidColorBrush* clientCast = nullptr;
	if(clientDrawingEffect) clientDrawingEffect->QueryInterface(IID_ID2D1SolidColorBrush, reinterpret_cast<void**>(&clientCast));

	if (clientDrawingEffect != nullptr && clientCast != nullptr)
	{
 		sb = clientCast;
	}

	try
	{
		renderTarget->DrawGlyphRun({ baselineOriginX, baselineOriginY }, glyphRun, sb, measuringMode);
		return S_OK;
	}
	catch (...)
	{
		return E_FAIL;
	}
	
}


HRESULT CustomColorRenderer::DrawUnderline(void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY,
	DWRITE_UNDERLINE const* underline, IUnknown* clientDrawingEffect)
{
	renderTarget->DrawLine({ baselineOriginX, baselineOriginY + underline->offset }, { baselineOriginX + underline->width, baselineOriginY + underline->offset }, defaultBrush, underline->thickness / 2.0f);
	return S_OK;
}


