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

#include "JSSVGStyleElement.h"

#include "KURL.h"
#include "SVGStyleElement.h"
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSSVGStyleElement);

/* Hash table */

static const HashTableValue JSSVGStyleElementTableValues[7] =
{
    { "type", DontDelete, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGStyleElementType), (intptr_t)setJSSVGStyleElementType },
    { "media", DontDelete, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGStyleElementMedia), (intptr_t)setJSSVGStyleElementMedia },
    { "title", DontDelete, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGStyleElementTitle), (intptr_t)setJSSVGStyleElementTitle },
    { "xmllang", DontDelete, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGStyleElementXmllang), (intptr_t)setJSSVGStyleElementXmllang },
    { "xmlspace", DontDelete, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGStyleElementXmlspace), (intptr_t)setJSSVGStyleElementXmlspace },
    { "constructor", DontEnum|ReadOnly, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGStyleElementConstructor), (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGStyleElementTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 31, JSSVGStyleElementTableValues, 0 };
#else
    { 17, 15, JSSVGStyleElementTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSSVGStyleElementConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGStyleElementConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSSVGStyleElementConstructorTableValues, 0 };
#else
    { 1, 0, JSSVGStyleElementConstructorTableValues, 0 };
#endif

class JSSVGStyleElementConstructor : public DOMConstructorObject {
public:
    JSSVGStyleElementConstructor(ExecState* exec, JSDOMGlobalObject* globalObject)
        : DOMConstructorObject(JSSVGStyleElementConstructor::createStructure(globalObject->objectPrototype()), globalObject)
    {
        putDirect(exec->propertyNames().prototype, JSSVGStyleElementPrototype::self(exec, globalObject), None);
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

const ClassInfo JSSVGStyleElementConstructor::s_info = { "SVGStyleElementConstructor", 0, &JSSVGStyleElementConstructorTable, 0 };

bool JSSVGStyleElementConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGStyleElementConstructor, DOMObject>(exec, &JSSVGStyleElementConstructorTable, this, propertyName, slot);
}

bool JSSVGStyleElementConstructor::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSSVGStyleElementConstructor, DOMObject>(exec, &JSSVGStyleElementConstructorTable, this, propertyName, descriptor);
}

/* Hash table for prototype */

static const HashTableValue JSSVGStyleElementPrototypeTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGStyleElementPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSSVGStyleElementPrototypeTableValues, 0 };
#else
    { 1, 0, JSSVGStyleElementPrototypeTableValues, 0 };
#endif

const ClassInfo JSSVGStyleElementPrototype::s_info = { "SVGStyleElementPrototype", 0, &JSSVGStyleElementPrototypeTable, 0 };

JSObject* JSSVGStyleElementPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGStyleElement>(exec, globalObject);
}

const ClassInfo JSSVGStyleElement::s_info = { "SVGStyleElement", &JSSVGElement::s_info, &JSSVGStyleElementTable, 0 };

JSSVGStyleElement::JSSVGStyleElement(NonNullPassRefPtr<Structure> structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGStyleElement> impl)
    : JSSVGElement(structure, globalObject, impl)
{
}

JSObject* JSSVGStyleElement::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSSVGStyleElementPrototype(JSSVGStyleElementPrototype::createStructure(JSSVGElementPrototype::self(exec, globalObject)));
}

bool JSSVGStyleElement::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGStyleElement, Base>(exec, &JSSVGStyleElementTable, this, propertyName, slot);
}

bool JSSVGStyleElement::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSSVGStyleElement, Base>(exec, &JSSVGStyleElementTable, this, propertyName, descriptor);
}

JSValue jsSVGStyleElementType(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSSVGStyleElement* castedThis = static_cast<JSSVGStyleElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGStyleElement* imp = static_cast<SVGStyleElement*>(castedThis->impl());
    JSValue result = jsString(exec, imp->type());
    return result;
}

JSValue jsSVGStyleElementMedia(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSSVGStyleElement* castedThis = static_cast<JSSVGStyleElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGStyleElement* imp = static_cast<SVGStyleElement*>(castedThis->impl());
    JSValue result = jsString(exec, imp->media());
    return result;
}

JSValue jsSVGStyleElementTitle(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSSVGStyleElement* castedThis = static_cast<JSSVGStyleElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGStyleElement* imp = static_cast<SVGStyleElement*>(castedThis->impl());
    JSValue result = jsString(exec, imp->title());
    return result;
}

JSValue jsSVGStyleElementXmllang(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSSVGStyleElement* castedThis = static_cast<JSSVGStyleElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGStyleElement* imp = static_cast<SVGStyleElement*>(castedThis->impl());
    JSValue result = jsString(exec, imp->xmllang());
    return result;
}

JSValue jsSVGStyleElementXmlspace(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSSVGStyleElement* castedThis = static_cast<JSSVGStyleElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGStyleElement* imp = static_cast<SVGStyleElement*>(castedThis->impl());
    JSValue result = jsString(exec, imp->xmlspace());
    return result;
}

JSValue jsSVGStyleElementConstructor(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSSVGStyleElement* domObject = static_cast<JSSVGStyleElement*>(asObject(slotBase));
    return JSSVGStyleElement::getConstructor(exec, domObject->globalObject());
}
void JSSVGStyleElement::put(ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot)
{
    lookupPut<JSSVGStyleElement, Base>(exec, propertyName, value, &JSSVGStyleElementTable, this, slot);
}

void setJSSVGStyleElementType(ExecState* exec, JSObject* thisObject, JSValue value)
{
    JSSVGStyleElement* castedThisObj = static_cast<JSSVGStyleElement*>(thisObject);
    SVGStyleElement* imp = static_cast<SVGStyleElement*>(castedThisObj->impl());
    ExceptionCode ec = 0;
    imp->setType(value.toString(exec), ec);
    setDOMException(exec, ec);
}

void setJSSVGStyleElementMedia(ExecState* exec, JSObject* thisObject, JSValue value)
{
    JSSVGStyleElement* castedThisObj = static_cast<JSSVGStyleElement*>(thisObject);
    SVGStyleElement* imp = static_cast<SVGStyleElement*>(castedThisObj->impl());
    ExceptionCode ec = 0;
    imp->setMedia(value.toString(exec), ec);
    setDOMException(exec, ec);
}

void setJSSVGStyleElementTitle(ExecState* exec, JSObject* thisObject, JSValue value)
{
    JSSVGStyleElement* castedThisObj = static_cast<JSSVGStyleElement*>(thisObject);
    SVGStyleElement* imp = static_cast<SVGStyleElement*>(castedThisObj->impl());
    ExceptionCode ec = 0;
    imp->setTitle(value.toString(exec), ec);
    setDOMException(exec, ec);
}

void setJSSVGStyleElementXmllang(ExecState* exec, JSObject* thisObject, JSValue value)
{
    JSSVGStyleElement* castedThisObj = static_cast<JSSVGStyleElement*>(thisObject);
    SVGStyleElement* imp = static_cast<SVGStyleElement*>(castedThisObj->impl());
    imp->setXmllang(value.toString(exec));
}

void setJSSVGStyleElementXmlspace(ExecState* exec, JSObject* thisObject, JSValue value)
{
    JSSVGStyleElement* castedThisObj = static_cast<JSSVGStyleElement*>(thisObject);
    SVGStyleElement* imp = static_cast<SVGStyleElement*>(castedThisObj->impl());
    imp->setXmlspace(value.toString(exec));
}

JSValue JSSVGStyleElement::getConstructor(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSSVGStyleElementConstructor>(exec, static_cast<JSDOMGlobalObject*>(globalObject));
}


}

#endif // ENABLE(SVG)
