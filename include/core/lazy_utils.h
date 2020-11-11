#pragma once
#include <algorithm>
#include <chrono>
#include <functional>

namespace bot {
namespace utils {

template<class T>
using predicate_unary = std::function<bool(const T&)>;
template<class T>
using predicate_binary = std::function<bool(const T&, const T&)>;

template<class T, class Arg>
auto stat_cast(Arg ptr){ return std::static_pointer_cast<T>(ptr); }

template<class T, class Arg>
auto dyn_cast(Arg ptr){ return std::dynamic_pointer_cast<T>(ptr); }

template<class T, class Arg>
auto dyn_cast(std::unique_ptr<Arg> &ptr){ return dynamic_cast<T*>(ptr.get()); }

template<class T, class Arg>
auto dyn_cast(const std::unique_ptr<Arg> &ptr){ return dynamic_cast<T*>(ptr.get()); }

template<class T, class Val = typename T::value_type>
auto find(const T& cont, const Val& val) {
    auto it = std::find(cont.begin(), cont.end(), val);
    return it;
}

template<class T, class Pred>
auto find_if(const T& cont, Pred pred) {
    auto it = std::find_if(cont.begin(), cont.end(), pred);
    return it;
}

template<class T, class Val = typename T::value_type>
auto erase(T& cont, const Val& val) {
    auto it = find(cont, val);
    if(it == cont.end()) {
        return false;
    }
    cont.erase(it);
    return true;
}

template<class T, class Pred>
auto erase_if(T& cont, Pred pred) {
    auto it = find_if(cont, pred);
    if(it == cont.end()) {
        return false;
    }
    cont.erase(it);
    return true;
}

template<class T, class Val = typename T::value_type>
auto contains(const T& cont, const Val& val) {
    auto it = find(cont, val);
    return it != cont.end();
}

template<class T, class Pred>
auto contains_if(const T& cont, Pred pred) {
    auto it = find_if(cont, pred);
    return it != cont.end();
}

template<class Cont, class Val = typename Cont::value_type>
size_t index(const Cont &cont, const Val &val){
    auto it = find(cont, val);
    if(it == cont.end()){
        return -1;
    }
    return std::distance(cont.begin(), it);
}

template<class T, class Func, class... Args>
auto measure(Func f, Args&&... args) -> T {
    auto time0 = std::chrono::steady_clock::now();
    std::invoke(f, std::forward<Args>(args)...);
    auto time1 = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<T>(time1 - time0);
}

} // namespace utils
} // namespace bot