#pragma once

#include <mbgl/gl/program.hpp>
#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/programs/attributes.hpp>

#include <cassert>

namespace mbgl {

template <class S, class P, class L, class U, class Ps = style::PaintProperties<>>
class Program;

template <class Shaders,
          class Primitive,
          class LayoutAttrs,
          class Uniforms,
          class... Ps>
class Program<Shaders, Primitive, LayoutAttrs, Uniforms, style::PaintProperties<Ps...>> {
public:
    using LayoutAttributes = LayoutAttrs;
    using PaintAttributes = gl::Attributes<typename Ps::Attribute...>;
    using Attributes = gl::ConcatenateAttributes<LayoutAttributes, PaintAttributes>;

    using LayoutVertex = typename LayoutAttributes::Vertex;
    using PaintValues = typename PaintAttributes::Values;

    using ProgramType = gl::Program<Primitive, Attributes, Uniforms>;
    using UniformValues = typename ProgramType::UniformValues;

    ProgramType program;

    Program(gl::Context& context, const ProgramParameters& programParameters)
        : program(context, vertexSource(programParameters), fragmentSource(programParameters))
        {}
    
    static std::string pixelRatioDefine(const ProgramParameters& parameters) {
        return std::string("#define DEVICE_PIXEL_RATIO ") + std::to_string(parameters.pixelRatio) + "\n";
    }

    static std::string fragmentSource(const ProgramParameters& parameters) {
        std::string source = pixelRatioDefine(parameters) + Shaders::fragmentSource;
        if (parameters.overdraw) {
            assert(source.find("#ifdef OVERDRAW_INSPECTOR") != std::string::npos);
            source.replace(source.find_first_of('\n'), 1, "\n#define OVERDRAW_INSPECTOR\n");
        }
        return source;
    }

    static std::string vertexSource(const ProgramParameters& parameters) {
        return pixelRatioDefine(parameters) + Shaders::vertexSource;
    }

    template <class AllProperties>
    static PaintValues paintPropertyValues(const AllProperties& properties) {
        return {
            Ps::Attribute::constantValue(properties.template get<Ps>())...
        };
    }

    template <class DrawMode, class AllPaintProperties>
    void draw(gl::Context& context,
              DrawMode drawMode,
              gl::DepthMode depthMode,
              gl::StencilMode stencilMode,
              gl::ColorMode colorMode,
              UniformValues&& uniformValues,
              const gl::VertexBuffer<LayoutVertex>& layoutVertexBuffer,
              const gl::IndexBuffer<DrawMode>& indexBuffer,
              const gl::SegmentVector<Attributes>& segments,
              const AllPaintProperties& properties) {
        program.draw(
            context,
            std::move(drawMode),
            std::move(depthMode),
            std::move(stencilMode),
            std::move(colorMode),
            std::move(uniformValues),
            LayoutAttributes::allVariableValues(layoutVertexBuffer)
                .concat(paintPropertyValues(properties)),
            indexBuffer,
            segments
        );
    }
};

} // namespace mbgl
