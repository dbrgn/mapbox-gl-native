#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/gl/segment.hpp>
#include <mbgl/util/ignore.hpp>
#include <mbgl/util/indexed_tuple.hpp>
#include <mbgl/util/variant.hpp>

#include <cstddef>
#include <functional>

namespace mbgl {
namespace gl {

template <class T, std::size_t N>
class VariableAttributeValue {
public:
    BufferID vertexBuffer;
    std::size_t vertexSize;
    std::size_t attributeOffset;

    void bind(Context&, AttributeLocation, std::size_t vertexOffset) const;
};

template <class T, std::size_t N>
bool operator==(const VariableAttributeValue<T, N>& lhs, const VariableAttributeValue<T, N>& rhs) {
    return lhs.vertexBuffer == rhs.vertexBuffer
        && lhs.vertexSize == rhs.vertexSize
        && lhs.attributeOffset == rhs.attributeOffset;
}

template <class T, std::size_t N>
class ConstantAttributeValue {
public:
    T value[N];

    void bind(Context&, AttributeLocation, std::size_t) const;
};

template <class T, std::size_t N>
bool operator==(const ConstantAttributeValue<T, N>& lhs, const ConstantAttributeValue<T, N>& rhs) {
    return lhs.value == rhs.value;
}

template <class Tag, class T, std::size_t N>
class Attribute {
public:
    using Type = T[N];

    using VariableValue = VariableAttributeValue<T, N>;
    using ConstantValue = ConstantAttributeValue<T, N>;

    using Location = AttributeLocation;

    using Value = variant<
        VariableValue,
        ConstantValue>;

    static void bind(Context& context,
                     const Location& location,
                     const Value& value_,
                     std::size_t vertexOffset) {
        Value::visit(value_, [&] (const auto& value) {
            value.bind(context, location, vertexOffset);
        });
    }
};

#define MBGL_DEFINE_ATTRIBUTE(type_, n_, name_) \
    struct name_ : ::mbgl::gl::Attribute<name_, type_, n_> { static constexpr auto name = #name_; }

namespace detail {

// Attribute binding requires member offsets. The only standard way to
// obtain an offset is the offsetof macro. The offsetof macro has defined
// behavior only for standard layout types. That rules out std::tuple and
// any other solution that relies on chained inheritance. Manually implemented
// variadic specialization looks like the only solution. Fortunately, we
// only use a maximum of five attributes.

template <class... As>
class Vertex;

template <>
class Vertex<> {
public:
    using VertexType = Vertex<>;
};

template <class A1>
class Vertex<A1> {
public:
    typename A1::Type a1;

    using VertexType = Vertex<A1>;
    static const std::size_t attributeOffsets[1];
};

template <class A1, class A2>
class Vertex<A1, A2> {
public:
    typename A1::Type a1;
    typename A2::Type a2;

    using VertexType = Vertex<A1, A2>;
    static const std::size_t attributeOffsets[2];
};

template <class A1, class A2, class A3>
class Vertex<A1, A2, A3> {
public:
    typename A1::Type a1;
    typename A2::Type a2;
    typename A3::Type a3;

    using VertexType = Vertex<A1, A2, A3>;
    static const std::size_t attributeOffsets[3];
};

template <class A1, class A2, class A3, class A4>
class Vertex<A1, A2, A3, A4> {
public:
    typename A1::Type a1;
    typename A2::Type a2;
    typename A3::Type a3;
    typename A4::Type a4;

    using VertexType = Vertex<A1, A2, A3, A4>;
    static const std::size_t attributeOffsets[4];
};

template <class A1, class A2, class A3, class A4, class A5>
class Vertex<A1, A2, A3, A4, A5> {
public:
    typename A1::Type a1;
    typename A2::Type a2;
    typename A3::Type a3;
    typename A4::Type a4;
    typename A5::Type a5;

    using VertexType = Vertex<A1, A2, A3, A4, A5>;
    static const std::size_t attributeOffsets[5];
};

template <class A1>
const std::size_t Vertex<A1>::attributeOffsets[1] = {
    offsetof(VertexType, a1)
};

template <class A1, class A2>
const std::size_t Vertex<A1, A2>::attributeOffsets[2] = {
    offsetof(VertexType, a1),
    offsetof(VertexType, a2)
};

template <class A1, class A2, class A3>
const std::size_t Vertex<A1, A2, A3>::attributeOffsets[3] = {
    offsetof(VertexType, a1),
    offsetof(VertexType, a2),
    offsetof(VertexType, a3)
};

template <class A1, class A2, class A3, class A4>
const std::size_t Vertex<A1, A2, A3, A4>::attributeOffsets[4] = {
    offsetof(VertexType, a1),
    offsetof(VertexType, a2),
    offsetof(VertexType, a3),
    offsetof(VertexType, a4)
};

template <class A1, class A2, class A3, class A4, class A5>
const std::size_t Vertex<A1, A2, A3, A4, A5>::attributeOffsets[5] = {
    offsetof(VertexType, a1),
    offsetof(VertexType, a2),
    offsetof(VertexType, a3),
    offsetof(VertexType, a4),
    offsetof(VertexType, a5)
};

} // namespace detail

AttributeLocation bindAttributeLocation(ProgramID, AttributeLocation, const char * name);

template <class... As>
class Attributes {
public:
    using Types = TypeList<As...>;
    using Locations = IndexedTuple<TypeList<As...>, TypeList<typename As::Location...>>;
    using Values = IndexedTuple<TypeList<As...>, TypeList<typename As::Value...>>;

    using Vertex = detail::Vertex<As...>;

    template <class A>
    static constexpr std::size_t Index = TypeIndex<A, As...>::value;

    static Locations locations(const ProgramID& id) {
        return Locations { bindAttributeLocation(id, Index<As>, As::name)... };
    }

    template <class DrawMode>
    static Values allVariableValues(const VertexBuffer<Vertex, DrawMode>& buffer) {
        static_assert(std::is_standard_layout<Vertex>::value, "vertex type must use standard layout");

        return Values {
            typename As::VariableValue {
                buffer.buffer,
                sizeof(Vertex),
                Vertex::attributeOffsets[Index<As>]
            }...
        };
    }

    static void bind(Context& context, const Locations& locations, const Values& values, std::size_t vertexOffset) {
        util::ignore({ (As::bind(context,
                                 locations.template get<As>(),
                                 values.template get<As>(),
                                 vertexOffset), 0)... });
    }
};

namespace detail {

template <class...>
struct ConcatenateAttributes;

template <class... As, class... Bs>
struct ConcatenateAttributes<TypeList<As...>, TypeList<Bs...>> {
    using Type = Attributes<As..., Bs...>;
};

} // namespace detail

template <class A, class B>
using ConcatenateAttributes = typename detail::ConcatenateAttributes<
    typename A::Types,
    typename B::Types>::Type;

} // namespace gl
} // namespace mbgl
