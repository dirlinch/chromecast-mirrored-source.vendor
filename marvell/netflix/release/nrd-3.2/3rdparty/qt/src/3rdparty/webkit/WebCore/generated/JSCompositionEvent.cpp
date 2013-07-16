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
#include "JSCompositionEvent.h"

#include "CompositionEvent.h"
#include "JSDOMWindow.h"
#include "KURL.h"
#include <runtime/Error.h>
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSCompositionEvent);

/* Hash table */

static const HashTableValue JSCompositionEventTableValues[3] =
{
    { "data", DontDelete|ReadOnly, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsCompositionEventData), (intptr_t)0 },
    { "constructor", DontEnum|ReadOnly, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsCompositionEventConstructor), (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSCompositionEventTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 3, JSCompositionEventTableValues, 0 };
#else
    { 4, 3, JSCompositionEventTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSCompositionEventConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSCompositionEventConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSCompositionEventConstructorTableValues, 0 };
#else
    { 1, 0, JSCompositionEventConstructorTableValues, 0 };
#endif

class JSCompositionEventConstructor : public DOMConstructorObject {
public:
    JSCompositionEventConstructor(ExecState* exec, JSDOMGlobalObject* globalObject)
        : DOMConstructorObject(JSCompositionEventConstructor::createStructure(globalObject->objectPrototype()), globalObject)
    {
        putDirect(exec->propertyNames().prototype, JSCompositionEventPrototype::self(exec, globalObject), None);
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

const ClassInfo JSCompositionEventConstructor::s_info = { "CompositionEventConstructor", 0, &JSCompositionEventConstructorTable, 0 };

bool JSCompositionEventConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSCompositionEventConstructor, DOMObject>(exec, &JSCompositionEventConstructorTable, this, propertyName, slot);
}

bool JSCompositionEventConstructor::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSCompositionEventConstructor, DOMObject>(exec, &JSCompositionEventConstructorTable, this, propertyName, descriptor);
}

/* Hash table for prototype */

static const HashTableValue JSCompositionEventPrototypeTableValues[2] =
{
    { "initCompositionEvent", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsCompositionEventPrototypeFunctionInitCompositionEvent), (intptr_t)5 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSCompositionEventPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSCompositionEventPrototypeTableValues, 0 };
#else
    { 2, 1, JSCompositionEventPrototypeTableValues, 0 };
#endif

const ClassInfo JSCompositionEventPrototype::s_info = { "CompositionEventPrototype", 0, &JSCompositionEventPrototypeTable, 0 };

JSObject* JSCompositionEventPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSCompositionEvent>(exec, globalObject);
}

bool JSCompositionEventPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticFunctionSlot<JSObject>(exec, &JSCompositionEventPrototypeTable, this, propertyName, slot);
}

bool JSCompositionEventPrototype::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticFunctionDescriptor<JSObject>(exec, &JSCompositionEventPrototypeTable, this, propertyName, descriptor);
}

const ClassInfo JSCompositionEvent::s_info = { "CompositionEvent", &JSUIEvent::s_info, &JSCompositionEventTable, 0 };

JSCompositionEvent::JSCompositionEvent(NonNullPassRefPtr<Structure> structure, JSDOMGlobalObject* globalObject, PassRefPtr<CompositionEvent> impl)
    : JSUIEvent(structure, globalObject, impl)
{
}

JSObject* JSCompositionEvent::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSCompositionEventPrototype(JSCompositionEventPrototype::createStructure(JSUIEventPrototype::self(exec, globalObject)));
}

bool JSCompositionEvent::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSCompositionEvent, Base>(exec, &JSCompositionEventTable, this, propertyName, slot);
}

bool JSCompositionEvent::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSCompositionEvent, Base>(exec, &JSCompositionEventTable, this, propertyName, descriptor);
}

JSValue jsCompositionEventData(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSCompositionEvent* castedThis = static_cast<JSCompositionEvent*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    CompositionEvent* imp = static_cast<CompositionEvent*>(castedThis->impl());
    JSValue result = jsString(exec, imp->data());
    return result;
}

JSValue jsCompositionEventConstructor(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSCompositionEvent* domObject = static_cast<JSCompositionEvent*>(asObject(slotBase));
    return JSCompositionEvent::getConstructor(exec, domObject->globalObject());
}
JSValue JSCompositionEvent::getConstructor(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSCompositionEventConstructor>(exec, static_cast<JSDOMGlobalObject*>(globalObject));
}

JSValue JSC_HOST_CALL jsCompositionEventPrototypeFunctionInitCompositionEvent(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSCompositionEvent::s_info))
        return throwError(exec, TypeError);
    JSCompositionEvent* castedThisObj = static_cast<JSCompositionEvent*>(asObject(thisValue));
    CompositionEvent* imp = static_cast<CompositionEvent*>(castedThisObj->impl());
    const UString& typeArg = args.at(0).toString(exec);
    bool canBubbleArg = args.at(1).toBoolean(exec);
    bool cancelableArg = args.at(2).toBoolean(exec);
    DOMWindow* viewArg = toDOMWindow(args.at(3));
    const UString& dataArg = args.at(4).toString(exec);

    imp->initCompositionEvent(typeArg, canBubbleArg, cancelableArg, viewArg, dataArg);
    return jsUndefined();
}


}
