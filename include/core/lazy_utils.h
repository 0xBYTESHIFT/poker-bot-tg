#pragma once
#include <algorithm>
#include <functional>
#include <chrono>

namespace bot{
namespace utils{

template<class T>
using predicate_unary = std::function<bool(const T&)>;
template<class T>
using predicate_binary = std::function<bool(const T&, const T&)>;

template<class T, class Val = typename T::value_type>
auto find(const T &cont, const Val &val){
    auto it = std::find(cont.begin(), cont.end(), val);
    return it;
}

template<class T, class Pred>
auto find_if(const T &cont, Pred pred){
    auto it = std::find_if(cont.begin(), cont.end(), pred);
    return it;
}

template<class T, class Val = typename T::value_type>
auto erase(T &cont, const Val &val){
    auto it = find(cont, val);
    if(it == cont.end()){
        return false;
    }
    cont.erase(it);
    return true;
}

template<class T, class Val = typename T::value_type>
auto erase(T &cont, const predicate_unary<Val> &pred){
    auto it = find(cont, pred);
    if(it == cont.end()){
        return false;
    }
    cont.erase(it);
    return true;
}

template<class T, class Val = typename T::value_type>
auto contains(const T &cont, const Val &val){
    auto it = find(cont, val);
    return it != cont.end();
}

template<class T, class Func, class ...Args>
auto measure(Func f, Args && ...args)->T{
    auto time0 = std::chrono::steady_clock::now();
    std::invoke(f, std::forward<Args>(args)...);
    auto time1 = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<T>(time1-time0);
}

}
}