#pragma once

#include <mbgl/gl/context.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/util/optional.hpp>

#include <cstddef>
#include <vector>

namespace mbgl {
namespace gl {

template <class Attributes>
class Segment {
public:
    Segment(std::size_t vertexOffset_,
            std::size_t indexOffset_,
            std::size_t vertexLength_ = 0,
            std::size_t indexLength_ = 0)
        : vertexOffset(vertexOffset_),
          indexOffset(indexOffset_),
          vertexLength(vertexLength_),
          indexLength(indexLength_) {}

    const std::size_t vertexOffset;
    const std::size_t indexOffset;

    std::size_t vertexLength;
    std::size_t indexLength;

    void bind(Context& context,
              BufferID indexBuffer_,
              const typename Attributes::Locations& attributeLocations,
              const typename Attributes::Values& attributeValues_) const {
        if (!vao) {
            vao = context.createVertexArray();
        }

        context.vertexArrayObject = *vao;

        if (indexBuffer != indexBuffer_) {
            indexBuffer = indexBuffer_;
            context.elementBuffer.setDirty();
            context.elementBuffer = indexBuffer_;
        }

        if (attributeValues != attributeValues_) {
            attributeValues = attributeValues_;
            Attributes::bind(context,
                             attributeLocations,
                             attributeValues_,
                             vertexOffset);
        }
    }

private:
    mutable optional<UniqueVertexArray> vao;
    mutable optional<BufferID> indexBuffer;
    mutable optional<typename Attributes::Values> attributeValues;
};

template <class Attributes>
class SegmentVector : public std::vector<Segment<Attributes>> {
public:
    SegmentVector() = default;
};

} // namespace gl
} // namespace mbgl
