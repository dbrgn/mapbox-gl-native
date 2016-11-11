#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/property_evaluation_parameters.hpp>
#include <mbgl/util/interpolate.hpp>

namespace mbgl {
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
class PossiblyEvaluatedProperty : public variant<T, PropertyFunction<T>> {
public:
    using variant<T, PropertyFunction<T>>::variant;

    T evaluatedValueOr(const T& other) const {
        return this->template is<T>() ? this->template get<T>() : other;
    }
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
        if (a.template is<T>() && b.template is<T>()) {
            return interpolate(a.template get<T>(), b.template get<T>(), t);
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
