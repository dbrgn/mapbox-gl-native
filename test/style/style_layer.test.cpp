#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_layer_observer.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/util/color.hpp>

using namespace mbgl;
using namespace mbgl::style;

namespace {

template <class T, class... Params> void testClone(Params... params) {
    auto layer = std::make_unique<T>(std::forward<Params>(params)...);
    auto clone = layer->baseImpl->clone();
    EXPECT_NE(layer.get(), clone.get());
    EXPECT_TRUE(reinterpret_cast<typename T::Impl*>(clone->baseImpl.get()));
    layer->impl->id = "test";
    EXPECT_EQ("test", layer->baseImpl->clone()->getID());
}

const auto color = Color { 1, 0, 0, 1 };
const auto opacity = 1.0f;
const auto radius = 1.0f;
const auto blur = 1.0f;
const auto pattern = std::string { "foo" };
const auto antialias = false;
const auto translate = std::array<float, 2> {{ 0, 0 }};
const auto translateAnchor = TranslateAnchorType::Map;
const auto lineCap = LineCapType::Round;
const auto lineJoin = LineJoinType::Miter;
const auto miterLimit = 1.0f;
const auto roundLimit = 1.0f;
const auto width = 1.0f;
const auto gapWidth = 1.0f;
const auto offset = 1.0f;
const auto dashArray = std::vector<float> {};
const auto hueRotate = 1.0f;
const auto brightness = 1.0f;
const auto saturation = 1.0f;
const auto contrast = 1.0f;
const auto duration = 1.0f;

} // namespace

TEST(Layer, Clone) {
    testClone<BackgroundLayer>("background");
    testClone<CircleLayer>("circle", "source");
    testClone<CustomLayer>("custom", [](void*){}, [](void*, const CustomLayerRenderParameters&){}, [](void*){}, nullptr),
    testClone<FillLayer>("fill", "source");
    testClone<LineLayer>("line", "source");
    testClone<RasterLayer>("raster", "source");
    testClone<SymbolLayer>("symbol", "source");
}

TEST(Layer, BackgroundProperties) {
    auto layer = std::make_unique<BackgroundLayer>("background");
    EXPECT_TRUE(layer->is<BackgroundLayer>());

    // Paint properties

    layer->setBackgroundColor(color);
    EXPECT_EQ(layer->getBackgroundColor().asConstant(), color);

    layer->setBackgroundOpacity(opacity);
    EXPECT_EQ(layer->getBackgroundOpacity().asConstant(), opacity);

    layer->setBackgroundPattern(pattern);
    EXPECT_EQ(layer->getBackgroundPattern().asConstant(), pattern);
}

TEST(Layer, CircleProperties) {
    auto layer = std::make_unique<CircleLayer>("circle", "source");
    EXPECT_TRUE(layer->is<CircleLayer>());

    // Paint properties

    layer->setCircleColor(color);
    EXPECT_EQ(layer->getCircleColor().asConstant(), color);

    layer->setCircleOpacity(opacity);
    EXPECT_EQ(layer->getCircleOpacity().asConstant(), opacity);

    layer->setCircleRadius(radius);
    EXPECT_EQ(layer->getCircleRadius().asConstant(), radius);

    layer->setCircleBlur(blur);
    EXPECT_EQ(layer->getCircleBlur().asConstant(), blur);

    layer->setCircleTranslate(translate);
    EXPECT_EQ(layer->getCircleTranslate().asConstant(), translate);

    layer->setCircleTranslateAnchor(translateAnchor);
    EXPECT_EQ(layer->getCircleTranslateAnchor().asConstant(), translateAnchor);
}

TEST(Layer, FillProperties) {
    auto layer = std::make_unique<FillLayer>("fill", "source");
    EXPECT_TRUE(layer->is<FillLayer>());

    // Paint properties

    layer->setFillColor(color);
    EXPECT_EQ(layer->getFillColor().asConstant(), color);

    layer->setFillOutlineColor(color);
    EXPECT_EQ(layer->getFillOutlineColor().asConstant(), color);

    layer->setFillOpacity(opacity);
    EXPECT_EQ(layer->getFillOpacity().asConstant(), opacity);

    layer->setFillPattern(pattern);
    EXPECT_EQ(layer->getFillPattern().asConstant(), pattern);

    layer->setFillAntialias(antialias);
    EXPECT_EQ(layer->getFillAntialias().asConstant(), antialias);

    layer->setFillTranslate(translate);
    EXPECT_EQ(layer->getFillTranslate().asConstant(), translate);

    layer->setFillTranslateAnchor(translateAnchor);
    EXPECT_EQ(layer->getFillTranslateAnchor().asConstant(), translateAnchor);
}

TEST(Layer, LineProperties) {
    auto layer = std::make_unique<LineLayer>("line", "source");
    EXPECT_TRUE(layer->is<LineLayer>());

    // Layout properties

    layer->setLineCap(lineCap);
    EXPECT_EQ(layer->getLineCap().asConstant(), lineCap);

    layer->setLineJoin(lineJoin);
    EXPECT_EQ(layer->getLineJoin().asConstant(), lineJoin);

    layer->setLineMiterLimit(miterLimit);
    EXPECT_EQ(layer->getLineMiterLimit().asConstant(), miterLimit);

    layer->setLineRoundLimit(roundLimit);
    EXPECT_EQ(layer->getLineRoundLimit().asConstant(), roundLimit);

    // Paint properties

    layer->setLineColor(color);
    EXPECT_EQ(layer->getLineColor().asConstant(), color);

    layer->setLineOpacity(opacity);
    EXPECT_EQ(layer->getLineOpacity().asConstant(), opacity);

    layer->setLineTranslate(translate);
    EXPECT_EQ(layer->getLineTranslate().asConstant(), translate);

    layer->setLineTranslateAnchor(translateAnchor);
    EXPECT_EQ(layer->getLineTranslateAnchor().asConstant(), translateAnchor);

    layer->setLineWidth(width);
    EXPECT_EQ(layer->getLineWidth().asConstant(), width);

    layer->setLineGapWidth(gapWidth);
    EXPECT_EQ(layer->getLineGapWidth().asConstant(), gapWidth);

    layer->setLineOffset(offset);
    EXPECT_EQ(layer->getLineOffset().asConstant(), offset);

    layer->setLineBlur(blur);
    EXPECT_EQ(layer->getLineBlur().asConstant(), blur);

    layer->setLineDasharray(dashArray);
    EXPECT_EQ(layer->getLineDasharray().asConstant(), dashArray);

    layer->setLinePattern(pattern);
    EXPECT_EQ(layer->getLinePattern().asConstant(), pattern);
}

TEST(Layer, RasterProperties) {
    auto layer = std::make_unique<RasterLayer>("raster", "source");
    EXPECT_TRUE(layer->is<RasterLayer>());

    // Paint properties

    layer->setRasterOpacity(opacity);
    EXPECT_EQ(layer->getRasterOpacity().asConstant(), opacity);

    layer->setRasterHueRotate(hueRotate);
    EXPECT_EQ(layer->getRasterHueRotate().asConstant(), hueRotate);

    layer->setRasterBrightnessMin(brightness);
    EXPECT_EQ(layer->getRasterBrightnessMin().asConstant(), brightness);

    layer->setRasterBrightnessMax(brightness);
    EXPECT_EQ(layer->getRasterBrightnessMax().asConstant(), brightness);

    layer->setRasterSaturation(saturation);
    EXPECT_EQ(layer->getRasterSaturation().asConstant(), saturation);

    layer->setRasterContrast(contrast);
    EXPECT_EQ(layer->getRasterContrast().asConstant(), contrast);

    layer->setRasterFadeDuration(duration);
    EXPECT_EQ(layer->getRasterFadeDuration().asConstant(), duration);
}

TEST(Layer, Observer) {
    auto layer = std::make_unique<LineLayer>("line", "source");
    StubLayerObserver observer;
    layer->baseImpl->setObserver(&observer);

    // Notifies observer on filter change.
    bool filterChanged = false;
    observer.layerFilterChanged = [&] (Layer& layer_) {
        EXPECT_EQ(layer.get(), &layer_);
        filterChanged = true;
    };
    layer->setFilter(NullFilter());
    EXPECT_TRUE(filterChanged);

    // Notifies observer on visibility change.
    bool visibilityChanged = false;
    observer.layerVisibilityChanged = [&] (Layer& layer_) {
        EXPECT_EQ(layer.get(), &layer_);
        visibilityChanged = true;
    };
    layer->setVisibility(VisibilityType::None);
    EXPECT_TRUE(visibilityChanged);

    // Notifies observer on paint property change.
    bool paintPropertyChanged = false;
    observer.layerPaintPropertyChanged = [&] (Layer& layer_) {
        EXPECT_EQ(layer.get(), &layer_);
        paintPropertyChanged = true;
    };
    layer->setLineColor(color);
    EXPECT_TRUE(paintPropertyChanged);

    // Notifies observer on layout property change.
    bool layoutPropertyChanged = false;
    observer.layerLayoutPropertyChanged = [&] (Layer& layer_, const char *) {
        EXPECT_EQ(layer.get(), &layer_);
        layoutPropertyChanged = true;
    };
    layer->setLineCap(lineCap);
    EXPECT_TRUE(layoutPropertyChanged);

    // Does not notify observer on no-op visibility change.
    visibilityChanged = false;
    layer->setVisibility(VisibilityType::None);
    EXPECT_FALSE(visibilityChanged);

    // Does not notify observer on no-op paint property change.
    paintPropertyChanged = false;
    layer->setLineColor(color);
    EXPECT_FALSE(paintPropertyChanged);

    // Does not notify observer on no-op layout property change.
    layoutPropertyChanged = false;
    layer->setLineCap(lineCap);
    EXPECT_FALSE(layoutPropertyChanged);
}
