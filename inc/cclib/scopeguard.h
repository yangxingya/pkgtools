//!
/// brief: scopeguard.h define scope guard for execute special func when 
///        obj leave scope.
///        study loki lib, then improve it.

#if !defined(cclib_scope_guard_h_)
#define cclib_scope_guard_h_

#include "types.h"

CCLIB_NAMESPACE_BEGIN

struct ScopeGuardBase
{
    ScopeGuardBase() : dismiss_(false) {}
    void Dismiss() const { dismiss_ = true; }
protected:
    ScopeGuardBase(ScopeGuardBase const& rhs) : dismiss_(rhs.dismiss_) { rhs.Dismiss(); }
    ~ScopeGuardBase() {}

    template<typename T>
    static void SafeExecute(T& t)
    {
        if (!t.dismiss_) {
            try {
                t.Execute();
            } catch (...) {
            
            }
        }
    }
    mutable bool dismiss_;
private:
    /// disable assign .ctor.
    ScopeGuardBase& operator=(ScopeGuardBase const&);
};

typedef ScopeGuardBase const& ScopeGuard;

template<typename F>
struct ScopeGuard0 : public ScopeGuardBase
{
    static ScopeGuard0<F> makeGuard(F fun) { return ScopeGuard0<F>(fun); }
    ~ScopeGuard0() { SafeExecute(*this); }
    void Execute() { fun_(); }
protected:
    ScopeGuard0(F fun) : fun_(fun) {}
    F fun_;
};

template<typename F>
inline ScopeGuard0<F> makeGuard(F f)
{
    return ScopeGuard0<F>::makeGuard(f);
}

template<typename F, typename P1>
struct ScopeGuard1 : public ScopeGuardBase
{
    static ScopeGuard1<F, P1> makeGuard(F fun, P1 p1) { return ScopeGuard1<F, P1>(fun, p1); }
    ~ScopeGuard1() { SafeExecute(*this); }
    void Execute() { fun_(p1_); }
protected:
    ScopeGuard1(F fun, P1 p1) : fun_(fun), p1_(p1) {}
    F fun_;
    P1 p1_;
};

template<typename F, typename P1>
inline ScopeGuard1<F, P1> makeGuard(F f, P1 p1)
{
    return ScopeGuard1<F, P1>::makeGuard(f, p1);
}

template <class Obj, typename MemFun>
struct ObjScopeGuard0 : public ScopeGuardBase
{
    static ObjScopeGuard0<Obj, MemFun> makeObjGuard(Obj& obj, MemFun memFun) { return ObjScopeGuard0<Obj, MemFun>(obj, memFun); }
    ~ObjScopeGuard0() throw() { SafeExecute(*this); }
    void Execute() { (obj_.*memFun_)(); }

protected:
    ObjScopeGuard0(Obj& obj, MemFun memFun) : obj_(obj), memFun_(memFun) {}

    Obj& obj_;
    MemFun memFun_;
};

template <class Obj, typename MemFun>
inline ObjScopeGuard0<Obj, MemFun> makeObjGuard(Obj& obj, MemFun memFun)
{
    return ObjScopeGuard0<Obj, MemFun>::makeObjGuard(obj, memFun);
}

//#define MemFree(h) 

CCLIB_NAMESPACE_END

#define CONN(x, y) x ## y
#define XCONN(x, y) CONN(x, y)
#define VARNAME_WITH_LINE(var) XCONN(var, __LINE__)

#define SCOPEEXIT ScopeGuard VARNAME_WITH_LINE(guard) = cclib::makeGuard

#endif // cclib_scope_guard_h_