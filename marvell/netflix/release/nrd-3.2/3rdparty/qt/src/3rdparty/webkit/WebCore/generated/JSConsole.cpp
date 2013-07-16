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
#include "JSConsole.h"

#include "Console.h"
#include "ScriptCallStack.h"
#include <runtime/Error.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSConsole);

/* Hash table for prototype */

static const HashTableValue JSConsolePrototypeTableValues[16] =
{
    { "debug", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsConsolePrototypeFunctionDebug), (intptr_t)0 },
    { "error", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsConsolePrototypeFunctionError), (intptr_t)0 },
    { "info", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsConsolePrototypeFunctionInfo), (intptr_t)0 },
    { "log", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsConsolePrototypeFunctionLog), (intptr_t)0 },
    { "warn", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsConsolePrototypeFunctionWarn), (intptr_t)0 },
    { "dir", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsConsolePrototypeFunctionDir), (intptr_t)0 },
    { "dirxml", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsConsolePrototypeFunctionDirxml), (intptr_t)0 },
    { "trace", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsConsolePrototypeFunctionTrace), (intptr_t)0 },
    { "assert", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsConsolePrototypeFunctionAssert), (intptr_t)1 },
    { "count", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsConsolePrototypeFunctionCount), (intptr_t)0 },
    { "markTimeline", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsConsolePrototypeFunctionMarkTimeline), (intptr_t)0 },
    { "time", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsConsolePrototypeFunctionTime), (intptr_t)1 },
    { "timeEnd", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsConsolePrototypeFunctionTimeEnd), (intptr_t)1 },
    { "group", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsConsolePrototypeFunctionGroup), (intptr_t)0 },
    { "groupEnd", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsConsolePrototypeFunctionGroupEnd), (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSConsolePrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 511, JSConsolePrototypeTableValues, 0 };
#else
    { 35, 31, JSConsolePrototypeTableValues, 0 };
#endif

const ClassInfo JSConsolePrototype::s_info = { "ConsolePrototype", 0, &JSConsolePrototypeTable, 0 };

JSObject* JSConsolePrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSConsole>(exec, globalObject);
}

bool JSConsolePrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticFunctionSlot<JSObject>(exec, &JSConsolePrototypeTable, this, propertyName, slot);
}

bool JSConsolePrototype::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticFunctionDescriptor<JSObject>(exec, &JSConsolePrototypeTable, this, propertyName, descriptor);
}

const ClassInfo JSConsole::s_info = { "Console", 0, 0, 0 };

JSConsole::JSConsole(NonNullPassRefPtr<Structure> structure, JSDOMGlobalObject* globalObject, PassRefPtr<Console> impl)
    : DOMObjectWithGlobalPointer(structure, globalObject)
    , m_impl(impl)
{
}

JSConsole::~JSConsole()
{
    forgetDOMObject(this, impl());
}

JSObject* JSConsole::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSConsolePrototype(JSConsolePrototype::createStructure(globalObject->objectPrototype()));
}

JSValue JSC_HOST_CALL jsConsolePrototypeFunctionDebug(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSConsole::s_info))
        return throwError(exec, TypeError);
    JSConsole* castedThisObj = static_cast<JSConsole*>(asObject(thisValue));
    Console* imp = static_cast<Console*>(castedThisObj->impl());
    ScriptCallStack callStack(exec, args, 0);

    imp->debug(&callStack);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsConsolePrototypeFunctionError(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSConsole::s_info))
        return throwError(exec, TypeError);
    JSConsole* castedThisObj = static_cast<JSConsole*>(asObject(thisValue));
    Console* imp = static_cast<Console*>(castedThisObj->impl());
    ScriptCallStack callStack(exec, args, 0);

    imp->error(&callStack);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsConsolePrototypeFunctionInfo(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSConsole::s_info))
        return throwError(exec, TypeError);
    JSConsole* castedThisObj = static_cast<JSConsole*>(asObject(thisValue));
    Console* imp = static_cast<Console*>(castedThisObj->impl());
    ScriptCallStack callStack(exec, args, 0);

    imp->info(&callStack);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsConsolePrototypeFunctionLog(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSConsole::s_info))
        return throwError(exec, TypeError);
    JSConsole* castedThisObj = static_cast<JSConsole*>(asObject(thisValue));
    Console* imp = static_cast<Console*>(castedThisObj->impl());
    ScriptCallStack callStack(exec, args, 0);

    imp->log(&callStack);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsConsolePrototypeFunctionWarn(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSConsole::s_info))
        return throwError(exec, TypeError);
    JSConsole* castedThisObj = static_cast<JSConsole*>(asObject(thisValue));
    Console* imp = static_cast<Console*>(castedThisObj->impl());
    ScriptCallStack callStack(exec, args, 0);

    imp->warn(&callStack);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsConsolePrototypeFunctionDir(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSConsole::s_info))
        return throwError(exec, TypeError);
    JSConsole* castedThisObj = static_cast<JSConsole*>(asObject(thisValue));
    Console* imp = static_cast<Console*>(castedThisObj->impl());
    ScriptCallStack callStack(exec, args, 0);

    imp->dir(&callStack);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsConsolePrototypeFunctionDirxml(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSConsole::s_info))
        return throwError(exec, TypeError);
    JSConsole* castedThisObj = static_cast<JSConsole*>(asObject(thisValue));
    Console* imp = static_cast<Console*>(castedThisObj->impl());
    ScriptCallStack callStack(exec, args, 0);

    imp->dirxml(&callStack);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsConsolePrototypeFunctionTrace(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSConsole::s_info))
        return throwError(exec, TypeError);
    JSConsole* castedThisObj = static_cast<JSConsole*>(asObject(thisValue));
    Console* imp = static_cast<Console*>(castedThisObj->impl());
    ScriptCallStack callStack(exec, args, 0);

    imp->trace(&callStack);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsConsolePrototypeFunctionAssert(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSConsole::s_info))
        return throwError(exec, TypeError);
    JSConsole* castedThisObj = static_cast<JSConsole*>(asObject(thisValue));
    Console* imp = static_cast<Console*>(castedThisObj->impl());
    ScriptCallStack callStack(exec, args, 1);
    bool condition = args.at(0).toBoolean(exec);

    imp->assertCondition(condition, &callStack);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsConsolePrototypeFunctionCount(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSConsole::s_info))
        return throwError(exec, TypeError);
    JSConsole* castedThisObj = static_cast<JSConsole*>(asObject(thisValue));
    Console* imp = static_cast<Console*>(castedThisObj->impl());
    ScriptCallStack callStack(exec, args, 0);

    imp->count(&callStack);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsConsolePrototypeFunctionMarkTimeline(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSConsole::s_info))
        return throwError(exec, TypeError);
    JSConsole* castedThisObj = static_cast<JSConsole*>(asObject(thisValue));
    Console* imp = static_cast<Console*>(castedThisObj->impl());
    ScriptCallStack callStack(exec, args, 0);

    imp->markTimeline(&callStack);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsConsolePrototypeFunctionTime(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSConsole::s_info))
        return throwError(exec, TypeError);
    JSConsole* castedThisObj = static_cast<JSConsole*>(asObject(thisValue));
    Console* imp = static_cast<Console*>(castedThisObj->impl());
    const UString& title = valueToStringWithUndefinedOrNullCheck(exec, args.at(0));

    imp->time(title);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsConsolePrototypeFunctionTimeEnd(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSConsole::s_info))
        return throwError(exec, TypeError);
    JSConsole* castedThisObj = static_cast<JSConsole*>(asObject(thisValue));
    Console* imp = static_cast<Console*>(castedThisObj->impl());
    ScriptCallStack callStack(exec, args, 1);
    const UString& title = valueToStringWithUndefinedOrNullCheck(exec, args.at(0));

    imp->timeEnd(title, &callStack);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsConsolePrototypeFunctionGroup(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSConsole::s_info))
        return throwError(exec, TypeError);
    JSConsole* castedThisObj = static_cast<JSConsole*>(asObject(thisValue));
    Console* imp = static_cast<Console*>(castedThisObj->impl());
    ScriptCallStack callStack(exec, args, 0);

    imp->group(&callStack);
    return jsUndefined();
}

JSValue JSC_HOST_CALL jsConsolePrototypeFunctionGroupEnd(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSConsole::s_info))
        return throwError(exec, TypeError);
    JSConsole* castedThisObj = static_cast<JSConsole*>(asObject(thisValue));
    Console* imp = static_cast<Console*>(castedThisObj->impl());

    imp->groupEnd();
    return jsUndefined();
}

JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, Console* object)
{
    return getDOMObjectWrapper<JSConsole>(exec, globalObject, object);
}
Console* toConsole(JSC::JSValue value)
{
    return value.inherits(&JSConsole::s_info) ? static_cast<JSConsole*>(asObject(value))->impl() : 0;
}

}
