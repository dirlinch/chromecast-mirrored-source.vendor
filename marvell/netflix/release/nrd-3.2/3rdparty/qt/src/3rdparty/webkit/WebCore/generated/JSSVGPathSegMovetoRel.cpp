/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"

#if ENABLE(SVG)

#include "JSSVGPathSegMovetoRel.h"

#include "SVGPathSegMoveto.h"
#include <runtime/JSNumberCell.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSSVGPathSegMovetoRel);

/* Hash table */

static const HashTableValue JSSVGPathSegMovetoRelTableValues[4] =
{
    { "x", DontDelete, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPathSegMovetoRelX), (intptr_t)setJSSVGPathSegMovetoRelX },
    { "y", DontDelete, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPathSegMovetoRelY), (intptr_t)setJSSVGPathSegMovetoRelY },
    { "constructor", DontEnum|ReadOnly, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPathSegMovetoRelConstructor), (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGPathSegMovetoRelTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 15, JSSVGPathSegMovetoRelTableValues, 0 };
#else
    { 9, 7, JSSVGPathSegMovetoRelTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSSVGPathSegMovetoRelConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGPathSegMovetoRelConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSSVGPathSegMovetoRelConstructorTableValues, 0 };
#else
    { 1, 0, JSSVGPathSegMovetoRelConstructorTableValues, 0 };
#endif

class JSSVGPathSegMovetoRelConstructor : public DOMConstructorObject {
public:
    JSSVGPathSegMovetoRelConstructor(ExecState* exec, JSDOMGlobalObject* globalObject)
        : DOMConstructorObject(JSSVGPathSegMovetoRelConstructor::createStructure(globalObject->objectPrototype()), globalObject)
    {
        putDirect(exec->propertyNames().prototype, JSSVGPathSegMovetoRelPrototype::self(exec, globalObject), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual bool getOwnPropertyDescriptor(ExecState*, const Identifier&, PropertyDescriptor&);
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    static PassRefPtr<Structure> createStructure(JSValue proto) 
    { 
        return Structure::create(proto, TypeInfo(ObjectType, StructureFlags), AnonymousSlotCount); 
    }
    
protected:
    static const unsigned StructureFlags = OverridesGetOwnPropertySlot | ImplementsHasInstance | DOMConstructorObject::StructureFlags;
};

const ClassInfo JSSVGPathSegMovetoRelConstructor::s_info = { "SVGPathSegMovetoRelConstructor", 0, &JSSVGPathSegMovetoRelConstructorTable, 0 };

bool JSSVGPathSegMovetoRelConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGPathSegMovetoRelConstructor, DOMObject>(exec, &JSSVGPathSegMovetoRelConstructorTable, this, propertyName, slot);
}

bool JSSVGPathSegMovetoRelConstructor::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSSVGPathSegMovetoRelConstructor, DOMObject>(exec, &JSSVGPathSegMovetoRelConstructorTable, this, propertyName, descriptor);
}

/* Hash table for prototype */

static const HashTableValue JSSVGPathSegMovetoRelPrototypeTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGPathSegMovetoRelPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSSVGPathSegMovetoRelPrototypeTableValues, 0 };
#else
    { 1, 0, JSSVGPathSegMovetoRelPrototypeTableValues, 0 };
#endif

const ClassInfo JSSVGPathSegMovetoRelPrototype::s_info = { "SVGPathSegMovetoRelPrototype", 0, &JSSVGPathSegMovetoRelPrototypeTable, 0 };

JSObject* JSSVGPathSegMovetoRelPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGPathSegMovetoRel>(exec, globalObject);
}

const ClassInfo JSSVGPathSegMovetoRel::s_info = { "SVGPathSegMovetoRel", &JSSVGPathSeg::s_info, &JSSVGPathSegMovetoRelTable, 0 };

JSSVGPathSegMovetoRel::JSSVGPathSegMovetoRel(NonNullPassRefPtr<Structure> structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGPathSegMovetoRel> impl)
    : JSSVGPathSeg(structure, globalObject, impl)
{
}

JSObject* JSSVGPathSegMovetoRel::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSSVGPathSegMovetoRelPrototype(JSSVGPathSegMovetoRelPrototype::createStructure(JSSVGPathSegPrototype::self(exec, globalObject)));
}

bool JSSVGPathSegMovetoRel::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGPathSegMovetoRel, Base>(exec, &JSSVGPathSegMovetoRelTable, this, propertyName, slot);
}

bool JSSVGPathSegMovetoRel::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSSVGPathSegMovetoRel, Base>(exec, &JSSVGPathSegMovetoRelTable, this, propertyName, descriptor);
}

JSValue jsSVGPathSegMovetoRelX(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSSVGPathSegMovetoRel* castedThis = static_cast<JSSVGPathSegMovetoRel*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGPathSegMovetoRel* imp = static_cast<SVGPathSegMovetoRel*>(castedThis->impl());
    JSValue result = jsNumber(exec, imp->x());
    return result;
}

JSValue jsSVGPathSegMovetoRelY(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSSVGPathSegMovetoRel* castedThis = static_cast<JSSVGPathSegMovetoRel*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGPathSegMovetoRel* imp = static_cast<SVGPathSegMovetoRel*>(castedThis->impl());
    JSValue result = jsNumber(exec, imp->y());
    return result;
}

JSValue jsSVGPathSegMovetoRelConstructor(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSSVGPathSegMovetoRel* domObject = static_cast<JSSVGPathSegMovetoRel*>(asObject(slotBase));
    return JSSVGPathSegMovetoRel::getConstructor(exec, domObject->globalObject());
}
void JSSVGPathSegMovetoRel::put(ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot)
{
    lookupPut<JSSVGPathSegMovetoRel, Base>(exec, propertyName, value, &JSSVGPathSegMovetoRelTable, this, slot);
}

void setJSSVGPathSegMovetoRelX(ExecState* exec, JSObject* thisObject, JSValue value)
{
    JSSVGPathSegMovetoRel* castedThisObj = static_cast<JSSVGPathSegMovetoRel*>(thisObject);
    SVGPathSegMovetoRel* imp = static_cast<SVGPathSegMovetoRel*>(castedThisObj->impl());
    imp->setX(value.toFloat(exec));
    JSSVGContextCache::propagateSVGDOMChange(castedThisObj, imp->associatedAttributeName());
}

void setJSSVGPathSegMovetoRelY(ExecState* exec, JSObject* thisObject, JSValue value)
{
    JSSVGPathSegMovetoRel* castedThisObj = static_cast<JSSVGPathSegMovetoRel*>(thisObject);
    SVGPathSegMovetoRel* imp = static_cast<SVGPathSegMovetoRel*>(castedThisObj->impl());
    imp->setY(value.toFloat(exec));
    JSSVGContextCache::propagateSVGDOMChange(castedThisObj, imp->associatedAttributeName());
}

JSValue JSSVGPathSegMovetoRel::getConstructor(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSSVGPathSegMovetoRelConstructor>(exec, static_cast<JSDOMGlobalObject*>(globalObject));
}


}

#endif // ENABLE(SVG)
