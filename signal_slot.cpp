#include <functional>
#include <vector>
using namespace std;

template <typename>
class signal;

// 如果您在 signal 类模板中使用 template <typename... Args>，
// 并且传入的函数类型是带引用的，那么 Args 会被推导为带引用的类型。
template <typename... Args>
class signal<void(Args...)>
{
private:
    vector<function<void(Args...)>> slots;

public:
    template <typename F>
    void bind(F&& func)
    {
        slots.emplace_back(std::forward<F>(func));
    }

    template <typename L>
    void bind(L* listener, void (L::*mem_func)(Args...))
    {
        slots.emplace_back([listener, mem_func](Args... args)
                           { (listener->*mem_func)(std::forward<Args>(args)...); });
    }

    void unbind_all() { slots = {}; }

    void operator()(Args... args)
    {
        for (auto& slot : slots) { slot(std::forward<Args>(args)...); }
    }
};

#include <iostream>
#include <memory>

void func1(unique_ptr<string>&& u)
{
    cout << *u << '\n';
}

void func2(shared_ptr<string>& s)
{
    cout << s.use_count() << '\n';
}

struct Foo
{
    int i {1024};

    void func(unique_ptr<string>&& u, shared_ptr<string>& s)
    {
        cout << *u << '\n';
        cout << s.use_count() << '\n';
        cout << i << '\n';
    }
};

int main()
{
    auto u = make_unique<string>("hello");
    auto s = make_shared<string>("hello");

    signal<void(unique_ptr<string>&&)> s1;
    signal<void(shared_ptr<string>&)> s2;
    signal<void(unique_ptr<string>&&, shared_ptr<string>&)> s3;

    s1.bind(func1);
    s1(std::move(u));

    s2.bind(func2);
    s2(s);

    Foo foo;
    s3.bind(&foo, &Foo::func);
    s3(std::move(u), s);
}
