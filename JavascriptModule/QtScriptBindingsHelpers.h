#pragma once

#include <QScriptValue>
#include <QScriptContext>
#include <QScriptEngine>
#include <QVariant>

#include <QScriptClass>
Q_DECLARE_METATYPE(QScriptClass*)

///\todo Remove these from here and move them to the programmatically generated files.
#include "Math/AABB.h"
#include "Math/Circle.h"
#include "Math/Cone.h"
#include "Math/Cylinder.h"
#include "Math/Ellipsoid.h"
#include "Math/float2.h"
#include "Math/float3.h"
#include "Math/float3x3.h"
#include "Math/float3x4.h"
#include "Math/float4.h"
#include "Math/float4x4.h"
#include "Math/Frustum.h"
#include "Math/HitInfo.h"
#include "Math/Line.h"
#include "Math/LineSegment.h"
#include "Math/OBB.h"
#include "Math/Plane.h"
#include "Math/Quat.h"
#include "Math/Ray.h"
#include "Math/Sphere.h"
#include "Math/TransformOps.h"
#include "Math/Triangle.h"
#include "Transform.h"

template<typename T> struct TypeHasAScriptClass { typedef QScriptValue QScriptValueWithoutScriptClass; };
template<> struct TypeHasAScriptClass<float2> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<float2*> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<float3> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<float3*> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<float4> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<float4*> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<float3x3> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<float3x3*> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<float3x4> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<float3x4*> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<float4x4> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<float4x4*> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<Quat> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<Quat*> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<Ray> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<Ray*> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<Transform> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<Transform*> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<TranslateOp> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<TranslateOp*> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<ScaleOp> { typedef QScriptValue QScriptValueWithScriptClass; };
template<> struct TypeHasAScriptClass<ScaleOp*> { typedef QScriptValue QScriptValueWithScriptClass; };

template<typename T>
bool QSVIsOfType(const typename TypeHasAScriptClass<T>::QScriptValueWithoutScriptClass &value)
{
    return value.toVariant().canConvert<T>();
}

template<typename T>
bool QSVIsOfType(const typename TypeHasAScriptClass<T>::QScriptValueWithScriptClass &value)
{
    return value.data().toVariant().canConvert<T>();
}

template<typename T>
T TypeFromQScriptValue(const typename TypeHasAScriptClass<T>::QScriptValueWithoutScriptClass &scriptValue)
{
    return qscriptvalue_cast<T>(scriptValue);
}

template<typename T>
class TypeT
{
public:
    static T foo(QScriptValue &scriptValue)
    {
        return scriptValue.data().toVariant().value<T>();
    }
};

template<typename T>
class TypeT<T*>
{
public:
    static T* foo(QScriptValue &scriptValue)
    {
        T *data = qscriptvalue_cast<T*>(scriptValue);
        if (data)
            return data;
        return qscriptvalue_cast<T*>(scriptValue.data());
    }
};

/// Converts a QScriptValue which has its data inside a QScriptClass back to T. 
template<typename T>
T TypeFromQScriptValue(const typename TypeHasAScriptClass<T>::QScriptValueWithScriptClass &scriptValue)
{
    QScriptValue sv = scriptValue;
    return TypeT<T>::foo(sv);
}

/// Converts a T to QScriptValue. Specializations properly register the QScriptClass types. 
template<typename T>
QScriptValue TypeToQScriptValue(QScriptEngine *engine, const T &t)
{
    return qScriptValueFromValue(engine, t);
}

template<>inline QScriptValue TypeToQScriptValue<float2>(QScriptEngine *engine, const float2 &t)
{
    QScriptClass *sc = engine->property("float2_scriptclass").value<QScriptClass*>();
    return engine->newObject(sc, qScriptValueFromValue(engine, t));
}

template<>inline QScriptValue TypeToQScriptValue<float3>(QScriptEngine *engine, const float3 &t)
{
    QScriptClass *sc = engine->property("float3_scriptclass").value<QScriptClass*>();
    return engine->newObject(sc, qScriptValueFromValue(engine, t));
}

template<>inline QScriptValue TypeToQScriptValue<float4>(QScriptEngine *engine, const float4 &t)
{
    QScriptClass *sc = engine->property("float4_scriptclass").value<QScriptClass*>();
    return engine->newObject(sc, qScriptValueFromValue(engine, t));
}

template<>inline QScriptValue TypeToQScriptValue<float3x3>(QScriptEngine *engine, const float3x3 &t)
{
    QScriptClass *sc = engine->property("float3x3_scriptclass").value<QScriptClass*>();
    return engine->newObject(sc, qScriptValueFromValue(engine, t));
}

template<>inline QScriptValue TypeToQScriptValue<float3x4>(QScriptEngine *engine, const float3x4 &t)
{
    QScriptClass *sc = engine->property("float3x4_scriptclass").value<QScriptClass*>();
    return engine->newObject(sc, qScriptValueFromValue(engine, t));
}

template<>inline QScriptValue TypeToQScriptValue<float4x4>(QScriptEngine *engine, const float4x4 &t)
{
    QScriptClass *sc = engine->property("float4x4_scriptclass").value<QScriptClass*>();
    return engine->newObject(sc, qScriptValueFromValue(engine, t));
}

template<>inline QScriptValue TypeToQScriptValue<Quat>(QScriptEngine *engine, const Quat &t)
{
    QScriptClass *sc = engine->property("Quat_scriptclass").value<QScriptClass*>();
    return engine->newObject(sc, qScriptValueFromValue(engine, t));
}

template<>inline QScriptValue TypeToQScriptValue<Ray>(QScriptEngine *engine, const Ray &t)
{
    QScriptClass *sc = engine->property("Ray_scriptclass").value<QScriptClass*>();
    return engine->newObject(sc, qScriptValueFromValue(engine, t));
}

template<>inline QScriptValue TypeToQScriptValue<Transform>(QScriptEngine *engine, const Transform &t)
{
    QScriptClass *sc = engine->property("Transform_scriptclass").value<QScriptClass*>();
    return engine->newObject(sc, qScriptValueFromValue(engine, t));
}

template<>inline QScriptValue TypeToQScriptValue<TranslateOp>(QScriptEngine *engine, const TranslateOp &t)
{
    QScriptClass *sc = engine->property("TranslateOp_scriptclass").value<QScriptClass*>();
    return engine->newObject(sc, qScriptValueFromValue(engine, t));
}

template<>inline QScriptValue TypeToQScriptValue<ScaleOp>(QScriptEngine *engine, const ScaleOp &t)
{
    QScriptClass *sc = engine->property("ScaleOp_scriptclass").value<QScriptClass*>();
    return engine->newObject(sc, qScriptValueFromValue(engine, t));
}

// A function to help the automatically generated code produce cleaner error reporting.
inline std::string Capitalize(QString str)
{
    return (str.left(1).toUpper() + str.mid(1)).toStdString();
}
