#ifndef __SINGLETON__H__
#define __SINGLETON__H__

namespace single {

template<class T>
class Singleton {
public:
    static T* GetInstance()
    {
        static T object;
        return &object;
    }
};

template<class T>
class SingletonPtr {
    std::shared_ptr<T> Singleton()
    {
        static std::shared_ptr<T> object_ptr(new T);
        return object_ptr;
    };
};

}

#endif