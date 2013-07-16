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

#if ENABLE(INSPECTOR)

#include "JSInspectorBackend.h"

#include "InspectorBackend.h"
#include <runtime/Error.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSInspectorBackend);

/* Hash table */

static const HashTableValue JSInspectorBackendTableValues[2] =
{
    { "constructor", DontEnum|ReadOnly, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsInspectorBackendConstructor), (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSInspectorBackendTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSInspectorBackendTableValues, 0 };
#else
    { 2, 1, JSInspectorBackendTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSInspectorBackendConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSInspectorBackendConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSInspectorBackendConstructorTableValues, 0 };
#else
    { 1, 0, JSInspectorBackendConstructorTableValues, 0 };
#endif

class JSInspectorBackendConstructor : public DOMConstructorObject {
public:
    JSInspectorBackendConstructor(ExecState* exec, JSDOMGlobalObject* globalObject)
        : DOMConstructorObject(JSInspectorBackendConstructor::createStructure(globalObject->objectPrototype()), globalObject)
    {
        putDirect(exec->propertyNames().prototype, JSInspectorBackendPrototype::self(exec, globalObject), None);
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

const ClassInfo JSInspectorBackendConstructor::s_info = { "InspectorBackendConstructor", 0, &JSInspectorBackendConstructorTable, 0 };

bool JSInspectorBackendConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSInspectorBackendConstructor, DOMObject>(exec, &JSInspectorBackendConstructorTable, this, propertyName, slot);
}

bool JSInspectorBackendConstructor::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSInspectorBackendConstructor, DOMObject>(exec, &JSInspectorBackendConstructorTable, this, propertyName, descriptor);
}

/* Hash table for prototype */

static const HashTableValue JSInspectorBackendPrototypeTableValues[42] =
{
    { "storeLastActivePanel", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionStoreLastActivePanel), (intptr_t)1 },
    { "saveFrontendSettings", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionSaveFrontendSettings), (intptr_t)1 },
    { "enableSearchingForNode", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionEnableSearchingForNode), (intptr_t)0 },
    { "disableSearchingForNode", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionDisableSearchingForNode), (intptr_t)0 },
    { "enableResourceTracking", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionEnableResourceTracking), (intptr_t)1 },
    { "disableResourceTracking", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionDisableResourceTracking), (intptr_t)1 },
    { "getResourceContent", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionGetResourceContent), (intptr_t)2 },
    { "reloadPage", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionReloadPage), (intptr_t)0 },
    { "startTimelineProfiler", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionStartTimelineProfiler), (intptr_t)0 },
    { "stopTimelineProfiler", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionStopTimelineProfiler), (intptr_t)0 },
    { "setInjectedScriptSource", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionSetInjectedScriptSource), (intptr_t)1 },
    { "dispatchOnInjectedScript", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionDispatchOnInjectedScript), (intptr_t)5 },
    { "addScriptToEvaluateOnLoad", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionAddScriptToEvaluateOnLoad), (intptr_t)1 },
    { "removeAllScriptsToEvaluateOnLoad", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionRemoveAllScriptsToEvaluateOnLoad), (intptr_t)0 },
    { "getChildNodes", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionGetChildNodes), (intptr_t)2 },
    { "setAttribute", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionSetAttribute), (intptr_t)4 },
    { "removeAttribute", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionRemoveAttribute), (intptr_t)3 },
    { "setTextNodeValue", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionSetTextNodeValue), (intptr_t)3 },
    { "getEventListenersForNode", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionGetEventListenersForNode), (intptr_t)2 },
    { "copyNode", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionCopyNode), (intptr_t)1 },
    { "removeNode", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionRemoveNode), (intptr_t)2 },
    { "highlightDOMNode", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionHighlightDOMNode), (intptr_t)1 },
    { "hideDOMNodeHighlight", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionHideDOMNodeHighlight), (intptr_t)0 },
    { "getStyles", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionGetStyles), (intptr_t)3 },
    { "getAllStyles", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionGetAllStyles), (intptr_t)1 },
    { "getInlineStyle", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionGetInlineStyle), (intptr_t)2 },
    { "getComputedStyle", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionGetComputedStyle), (intptr_t)2 },
    { "applyStyleText", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionApplyStyleText), (intptr_t)4 },
    { "setStyleText", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionSetStyleText), (intptr_t)3 },
    { "setStyleProperty", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionSetStyleProperty), (intptr_t)4 },
    { "toggleStyleEnabled", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionToggleStyleEnabled), (intptr_t)4 },
    { "setRuleSelector", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionSetRuleSelector), (intptr_t)4 },
    { "addRule", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionAddRule), (intptr_t)3 },
    { "getCookies", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionGetCookies), (intptr_t)1 },
    { "deleteCookie", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionDeleteCookie), (intptr_t)2 },
    { "releaseWrapperObjectGroup", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionReleaseWrapperObjectGroup), (intptr_t)2 },
    { "didEvaluateForTestInFrontend", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionDidEvaluateForTestInFrontend), (intptr_t)2 },
    { "getDatabaseTableNames", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionGetDatabaseTableNames), (intptr_t)2 },
    { "getDOMStorageEntries", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionGetDOMStorageEntries), (intptr_t)2 },
    { "setDOMStorageItem", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionSetDOMStorageItem), (intptr_t)4 },
    { "removeDOMStorageItem", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsInspectorBackendPrototypeFunctionRemoveDOMStorageItem), (intptr_t)3 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSInspectorBackendPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 511, JSInspectorBackendPrototypeTableValues, 0 };
#else
    { 134, 127, JSInspectorBackendPrototypeTableValues, 0 };
#endif

const ClassInfo JSInspectorBackendPrototype::s_info = { "InspectorBackendPrototype", 0, &JSInspectorBackendPrototypeTable, 0 };

JSObject* JSInspectorBackendPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSInspectorBackend>(exec, globalObject);
}

bool JSInspectorBackendPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticFunctionSlot<JSObject>(exec, &JSInspectorBackendPrototypeTable, this, propertyName, slot);
}

bool JSInspectorBackendPrototype::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticFunctionDescriptor<JSObject>(exec, &JSInspectorBackendPrototypeTable, this, propertyName, descriptor);
}

const ClassInfo JSInspectorBackend::s_info = { "InspectorBackend", 0, &JSInspectorBackendTable, 0 };

JSInspectorBackend::JSInspectorBackend(NonNullPassRefPtr<Structure> structure, JSDOMGlobalObject* globalObject, PassRefPtr<InspectorBackend> impl)
    : DOMObjectWithGlobalPointer(structure, globalObject)
    , m_impl(impl)
{
}

JSInspectorBackend::~JSInspectorBackend()
{
    forgetDOMObject(this, impl());
}

JSObject* JSInspectorBackend::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSInspectorBackendPrototype(JSInspectorBackendPrototype::createStructure(globalObject->objectPrototype()));
}

bool JSInspectorBackend::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSInspectorBackend, Base>(exec, &JSInspectorBackendTable, this, propertyName, slot);
}

bool JSInspectorBackend::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSInspectorBackend, Base>(exec, &JSInspectorBackendTable, this, propertyName, descriptor);
}

JSValue jsInspectorBackendConstructor(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSInspectorBackend* domObject = static_cast<JSInspectorBackend*>(asObject(slotBase));
    return JSInspectorBackend::getConstructor(exec, domObject->globalObject());
}
JSValue JSInspectorBackend::getConstructor(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSInspectorBackendConstructor>(exec, static_cast<JSDOMGlobalObject*>(globalObject));
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionStoreLastActivePanel(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    const UString& panelName = args.at(0).toString(exec);

    imp->storeLastActivePanel(panelName);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionSaveFrontendSettings(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    const UString& settings = args.at(0).toString(exec);

    imp->saveFrontendSettings(settings);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionEnableSearchingForNode(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());

    imp->enableSearchingForNode();
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionDisableSearchingForNode(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());

    imp->disableSearchingForNode();
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionEnableResourceTracking(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    bool always = args.at(0).toBoolean(exec);

    imp->enableResourceTracking(always);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionDisableResourceTracking(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    bool always = args.at(0).toBoolean(exec);

    imp->disableResourceTracking(always);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionGetResourceContent(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    unsigned identifier = args.at(1).toInt32(exec);

    imp->getResourceContent(callId, identifier);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionReloadPage(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());

    imp->reloadPage();
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionStartTimelineProfiler(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());

    imp->startTimelineProfiler();
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionStopTimelineProfiler(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());

    imp->stopTimelineProfiler();
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionSetInjectedScriptSource(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    const UString& scriptSource = args.at(0).toString(exec);

    imp->setInjectedScriptSource(scriptSource);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionDispatchOnInjectedScript(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int injectedScriptId = args.at(1).toInt32(exec);
    const UString& methodName = args.at(2).toString(exec);
    const UString& arguments = args.at(3).toString(exec);
    bool async = args.at(4).toBoolean(exec);

    imp->dispatchOnInjectedScript(callId, injectedScriptId, methodName, arguments, async);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionAddScriptToEvaluateOnLoad(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    const UString& scriptSource = args.at(0).toString(exec);

    imp->addScriptToEvaluateOnLoad(scriptSource);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionRemoveAllScriptsToEvaluateOnLoad(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());

    imp->removeAllScriptsToEvaluateOnLoad();
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionGetChildNodes(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int nodeId = args.at(1).toInt32(exec);

    imp->getChildNodes(callId, nodeId);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionSetAttribute(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int elementId = args.at(1).toInt32(exec);
    const UString& name = args.at(2).toString(exec);
    const UString& value = args.at(3).toString(exec);

    imp->setAttribute(callId, elementId, name, value);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionRemoveAttribute(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int elementId = args.at(1).toInt32(exec);
    const UString& name = args.at(2).toString(exec);

    imp->removeAttribute(callId, elementId, name);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionSetTextNodeValue(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int nodeId = args.at(1).toInt32(exec);
    const UString& value = args.at(2).toString(exec);

    imp->setTextNodeValue(callId, nodeId, value);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionGetEventListenersForNode(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int nodeId = args.at(1).toInt32(exec);

    imp->getEventListenersForNode(callId, nodeId);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionCopyNode(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int nodeId = args.at(0).toInt32(exec);

    imp->copyNode(nodeId);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionRemoveNode(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int nodeId = args.at(1).toInt32(exec);

    imp->removeNode(callId, nodeId);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionHighlightDOMNode(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int nodeId = args.at(0).toInt32(exec);

    imp->highlightDOMNode(nodeId);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionHideDOMNodeHighlight(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());

    imp->hideDOMNodeHighlight();
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionGetStyles(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int nodeId = args.at(1).toInt32(exec);
    bool authOnly = args.at(2).toBoolean(exec);

    imp->getStyles(callId, nodeId, authOnly);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionGetAllStyles(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);

    imp->getAllStyles(callId);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionGetInlineStyle(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int nodeId = args.at(1).toInt32(exec);

    imp->getInlineStyle(callId, nodeId);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionGetComputedStyle(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int nodeId = args.at(1).toInt32(exec);

    imp->getComputedStyle(callId, nodeId);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionApplyStyleText(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int styleId = args.at(1).toInt32(exec);
    const UString& styleText = args.at(2).toString(exec);
    const UString& propertyName = args.at(3).toString(exec);

    imp->applyStyleText(callId, styleId, styleText, propertyName);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionSetStyleText(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int styleId = args.at(1).toInt32(exec);
    const UString& styleText = args.at(2).toString(exec);

    imp->setStyleText(callId, styleId, styleText);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionSetStyleProperty(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int styleId = args.at(1).toInt32(exec);
    const UString& name = args.at(2).toString(exec);
    const UString& value = args.at(3).toString(exec);

    imp->setStyleProperty(callId, styleId, name, value);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionToggleStyleEnabled(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int styleId = args.at(1).toInt32(exec);
    const UString& propertyName = args.at(2).toString(exec);
    bool disabled = args.at(3).toBoolean(exec);

    imp->toggleStyleEnabled(callId, styleId, propertyName, disabled);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionSetRuleSelector(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int ruleId = args.at(1).toInt32(exec);
    const UString& selector = args.at(2).toString(exec);
    int selectedNodeId = args.at(3).toInt32(exec);

    imp->setRuleSelector(callId, ruleId, selector, selectedNodeId);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionAddRule(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    const UString& selector = args.at(1).toString(exec);
    int selectedNodeId = args.at(2).toInt32(exec);

    imp->addRule(callId, selector, selectedNodeId);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionGetCookies(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);

    imp->getCookies(callId);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionDeleteCookie(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    const UString& cookieName = args.at(0).toString(exec);
    const UString& domain = args.at(1).toString(exec);

    imp->deleteCookie(cookieName, domain);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionReleaseWrapperObjectGroup(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int injectedScriptId = args.at(0).toInt32(exec);
    const UString& objectGroup = args.at(1).toString(exec);

    imp->releaseWrapperObjectGroup(injectedScriptId, objectGroup);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionDidEvaluateForTestInFrontend(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    const UString& jsonResult = args.at(1).toString(exec);

    imp->didEvaluateForTestInFrontend(callId, jsonResult);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionGetDatabaseTableNames(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int databaseId = args.at(1).toInt32(exec);

    imp->getDatabaseTableNames(callId, databaseId);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionGetDOMStorageEntries(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int storageId = args.at(1).toInt32(exec);

    imp->getDOMStorageEntries(callId, storageId);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionSetDOMStorageItem(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int storageId = args.at(1).toInt32(exec);
    const UString& key = args.at(2).toString(exec);
    const UString& value = args.at(3).toString(exec);

    imp->setDOMStorageItem(callId, storageId, key, value);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsInspectorBackendPrototypeFunctionRemoveDOMStorageItem(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSInspectorBackend::s_info))
        return throwError(exec, TypeError);
    JSInspectorBackend* castedThisObj = static_cast<JSInspectorBackend*>(asObject(thisValue));
    InspectorBackend* imp = static_cast<InspectorBackend*>(castedThisObj->impl());
    int callId = args.at(0).toInt32(exec);
    int storageId = args.at(1).toInt32(exec);
    const UString& key = args.at(2).toString(exec);

    imp->removeDOMStorageItem(callId, storageId, key);
    return jsUndefined();
}

JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, InspectorBackend* object)
{
    return getDOMObjectWrapper<JSInspectorBackend>(exec, globalObject, object);
}
InspectorBackend* toInspectorBackend(JSC::JSValue value)
{
    return value.inherits(&JSInspectorBackend::s_info) ? static_cast<JSInspectorBackend*>(asObject(value))->impl() : 0;
}

}

#endif // ENABLE(INSPECTOR)
