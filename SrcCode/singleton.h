#ifndef __ZY_SINGLETION_H__
#define __ZY_SINGLETION_H__
#include <memory>

// 返回裸指针
template<class T, class X = void, int N = 0>
class Singletion
{
public:
    static T* getSingletion()
    {
        // 只能定义在函数内部，如果定义在类中，那么就必须在类外定义，而模板不支持分离编译
        // 第一次声明之后，就不会再次声明，只要是使用这个特例类，得到的都是这一个指针
        // 通过定义不同的模板特例类，可以定义出多个不同的对象，加强了单例的功能
        static T v;            

        return &v;
    }
private:
};



// 返回智能指针
template<class T, class X = void, int N = 0>
class SingletionPtr
{
public:
    static std::shared_ptr<T> getSingletion()
    {
        // 只能定义在函数内部，如果定义在类中，那么就必须在类外定义，而模板不支持分离编译
        // 第一次声明之后，就不会再次声明，只要是使用这个特例类，得到的都是这一个指针
        // 通过定义不同的模板特例类，可以定义出多个不同的对象，加强了单例的功能
        static std::shared_ptr<T> v(new T);       

        return v;
    }
private:
};



#endif
