#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/property_evaluation_parameters.hpp>
#include <mbgl/util/interpolate.hpp>

namespace mbgl {

class GeometryTileFeature;

namespace style {

class PropertyEvaluationParameters;

template <typename T>
class PropertyEvaluator {
public:
    using ResultType = T;

    PropertyEvaluator(const PropertyEvaluationParameters& parameters_, T defaultValue_)
        : parameters(parameters_),
          defaultValue(std::move(defaultValue_)) {}

    T operator()(const Undefined&) const { return defaultValue; }
    T operator()(const T& constant) const { return constant; }
    T operator()(const ZoomFunction<T>& f) const { return f.evaluate(parameters.z); }

private:
    const PropertyEvaluationParameters& parameters;
    T defaultValue;
};

template <class T>
class PossiblyEvaluatedProperty {
public:
    PossiblyEvaluatedProperty() = default;

    PossiblyEvaluatedProperty(T t)
        : value(std::move(t))
        {}

    PossiblyEvaluatedProperty(PropertyFunction<T> t)
        : value(std::move(t))
        {}

    bool isConstant() const { return value.template is<T>(); };
    bool isVariable() const { return !value.template is<T>(); };

    optional<T> constant() const {
        return isConstant() ? value.template get<T>() : optional<T>();
    }

    T constantOr(const T& t) const {
        return constant().value_or(t);
    }

    T evaluate(float, const GeometryTileFeature& feature) const {
        return value.match(
            [&] (const T& t) { return t; },
            [&] (const PropertyFunction<T>& t) { return t.evaluate(feature); });
    }

private:
    variant<T, PropertyFunction<T>> value;
};

template <typename T>
class DataDrivenPropertyEvaluator {
public:
    using ResultType = PossiblyEvaluatedProperty<T>;

    DataDrivenPropertyEvaluator(const PropertyEvaluationParameters& parameters_, T defaultValue_)
        : parameters(parameters_),
          defaultValue(std::move(defaultValue_)) {}

    ResultType operator()(const Undefined&) const { return defaultValue; }
    ResultType operator()(const T& constant) const { return constant; }
    ResultType operator()(const ZoomFunction<T>& f) const { return f.evaluate(parameters.z); }
    ResultType operator()(const PropertyFunction<T>& f) const { return f; }

private:
    const PropertyEvaluationParameters& parameters;
    T defaultValue;
};

template <typename T>
class Faded {
public:
    T from;
    T to;
    float fromScale;
    float toScale;
    float t;
};

template <typename T>
class CrossFadedPropertyEvaluator {
public:
    using ResultType = Faded<T>;

    CrossFadedPropertyEvaluator(const PropertyEvaluationParameters& parameters_, T defaultValue_)
        : parameters(parameters_),
          defaultValue(std::move(defaultValue_)) {}

    Faded<T> operator()(const Undefined&) const;
    Faded<T> operator()(const T& constant) const;
    Faded<T> operator()(const ZoomFunction<T>&) const;

private:
    Faded<T> calculate(const T& min, const T& mid, const T& max) const;

    const PropertyEvaluationParameters& parameters;
    T defaultValue;
};

} // namespace style

namespace util {

template <typename T>
struct Interpolator<style::PossiblyEvaluatedProperty<T>> {
    style::PossiblyEvaluatedProperty<T> operator()(const style::PossiblyEvaluatedProperty<T>& a,
                                                   const style::PossiblyEvaluatedProperty<T>& b,
                                                   const double t) const {
        if (a.isConstant() && b.isConstant()) {
            return interpolate(*a.constant(), *b.constant(), t);
        } else {
            return a;
        }
    }
};

template <typename T>
struct Interpolator<style::Faded<T>>
    : Uninterpolated {};

} // namespace util

} // namespace mbgl
