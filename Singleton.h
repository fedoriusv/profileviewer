#ifndef _PV_SINGLETON_H_
#define _PV_SINGLETON_H_

namespace pv
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    class Singleton
    {
    public:

        static T*   getInstance();
        static void freeInstance();

    protected:

        Singleton();
        virtual     ~Singleton();

    private:

        static T*   s_instance;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    T* Singleton<T>::s_instance = nullptr;

    template <class T>
    Singleton<T>::Singleton()
    {
    }

    template <class T>
    Singleton<T>::~Singleton()
    {
    }

    template <class T>
    T *Singleton<T>::getInstance()
    {
        if (s_instance == nullptr)
        {
            s_instance = new T;
        }

        return s_instance;
    }

    template <class T>
    void Singleton<T>::freeInstance()
    {
        if (s_instance)
        {
            delete s_instance;
        }

        s_instance = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////

}
#endif //_PV_SINGLETON_H_
