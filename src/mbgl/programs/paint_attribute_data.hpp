#pragma once

#include <mbgl/style/paint_property.hpp>
#include <mbgl/util/type_list.hpp>

namespace mbgl {

template <class AllProperties, class DataDrivenProperties>
class PaintAttributeData;

template <class... Ps, class... DDPs>
class PaintAttributeData<TypeList<Ps...>, TypeList<DDPs...>> {
public:
    using EvaluatedProperties = typename style::PaintProperties<Ps...>::Evaluated;
    using Attributes = gl::Attributes<typename DDPs::Attribute...>;

    PaintAttributeData(EvaluatedProperties properties_, float z)
        : properties(std::move(properties_)),
          zoom(z),
          vertexVectors(DDPs::vertexVector(properties.template get<DDPs>())...)
    {
        util::ignore({
            (DDPs::setAttributeValueIfConstant(attributeValues_.template get<DDPs>(),
                                               properties.template get<DDPs>()), 0)...
        });
    }

    void populateVertexVectors(const GeometryTileFeature& feature,
                               std::size_t length) {
        util::ignore({
            (DDPs::populateVertexVector(vertexVectors.template get<DDPs>(),
                                        properties.template get<DDPs>(),
                                        feature, zoom, length), 0)...
        });
    }

    void upload(gl::Context& context) {
        vertexBuffers = VertexBuffers {
            DDPs::vertexBuffer(context, std::move(vertexVectors.template get<DDPs>()))...
        };

        util::ignore({
            (DDPs::setAttributeValueIfVariable(attributeValues_.template get<DDPs>(),
                                               vertexBuffers.template get<DDPs>()), 0)...
        });
    }

    typename Attributes::Values attributeValues() {
        return typename Attributes::Values {
            attributeValues_.template get<DDPs>()...
        };
    }

private:
    template <class... Ts>
    using Tuple = IndexedTuple<TypeList<DDPs...>, TypeList<Ts...>>;

    using VertexVectors = Tuple<optional<typename DDPs::VertexVector>...>;
    using VertexBuffers = Tuple<optional<typename DDPs::VertexBuffer>...>;
    using AttributeValues = Tuple<typename DDPs::AttributeValue...>;

    EvaluatedProperties properties;
    float zoom;

    VertexVectors vertexVectors;
    VertexBuffers vertexBuffers;
    AttributeValues attributeValues_;
};

}
