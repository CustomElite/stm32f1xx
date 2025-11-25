#pragma once

template <typename tParam>
class ifunction {
public:
    using parameter = tParam;
    
    virtual ~ifunction() {}
    virtual void operator()(tParam) const = 0;
};

template <>
class ifunction<void> {
public:
    using parameter = void;
    
    virtual ~ifunction() {}
    virtual void operator()() const = 0;
};

template <typename tObj, typename tParam>
class function : public ifunction<tParam> {
public:
    using object = tObj;
    using parameter = tParam;

    function(tObj& obj, void(tObj::*func)(tParam))
        : mObject{ &obj }
        , mFunction{ func }
    {}

    virtual void operator()(tParam param) const override { (mObject->*mFunction)(param); }

private:
    tObj* mObject;
    void(tObj::*mFunction)(tParam);
};

template <typename tObj>
class function<tObj, void> : public ifunction<void> {
public:
    function(tObj& object, void(tObj::*func)(void))
        : mObject{ &object }
        , mFunction{ func }
    {}

    virtual void operator()() const override { (mObject->*mFunction)(); }

private:
    tObj* mObject;
    void(tObj::*mFunction)();
};

template <typename tParam>
class function<void, tParam> : public ifunction<tParam> {
public:
    using parameter = tParam;

    explicit function(void(*func)(tParam))
        : mFunction{ func }
    {}

    virtual void operator()(tParam param) const override { (*mFunction)(param); }

private:
    void(*mFunction)(tParam);
};

template <>
class function<void, void> : public ifunction<void> {
public:
    explicit function(void(*func)(void))
        : mFunction{ func }
    {}

    virtual void operator()() const override { (*mFunction)(); }

private:
    void(*mFunction)();
};

//////////////////////////////////////////////
//////////////////////////////////////////////

template <typename tObj, typename tParam, void(tObj::*tFunc)(tParam)>
class function_mp : public ifunction<tParam> {
public:
    using object = tObj;
    using parameter = tParam;

    explicit function_mp(tObj& obj) : mObject{ &obj } {}

    virtual void operator()(tParam param) const override { (mObject->*tFunc)(param); }

private:
    tObj* mObject;
};

template <typename tObj, void(tObj::*tFunc)(void)>
class function_mv : public ifunction<void> {
public:
    using object = tObj;

    explicit function_mv(tObj& obj) : mObject{ &obj } {}

    virtual void operator()() const override { (mObject->*tFunc)(); }

private:
    tObj* mObject;
};

template <typename tObj, typename tParam, tObj& tInstance, void(tObj::*tFunc)(tParam)>
class function_imp : public ifunction<tParam> {
public:
    using object = tObj;
    using parameter = tParam;

    virtual void operator()(tParam param) const override { (tInstance.*tFunc)(param); }
};

template <typename tObj, tObj& tInstance, void(tObj::*tFunc)(void)>
class function_imv : public ifunction<void> {
public:
    using object = tObj;

    virtual void operator()() const override { (tInstance.*tFunc)(); }
};

template <typename tParam, void(*tFunc)(tParam)>
class function_fp : public ifunction<tParam> {
public:
    using parameter = tParam;

    function_fp() {}

    virtual void operator()(tParam param) const override { (*tFunc)(param); }
};

template <void(*tFunc)(void)>
class function_fv : public ifunction<void> {
public:
    function_fv() {}

    virtual void operator()() const override { (*tFunc)(); }
};
