/**
 * @file RefCounterT.h
 * @brief
 * @author GongZhaohui
 * @version
 * @date 2016-02-25
 */

#ifndef _AMOS_REFCOUNTER_H_
#define _AMOS_REFCOUNTER_H_

namespace amos
{

    template <class DERIVED, class DELETER>
    class RefCounter
    {
    public:
        RefCounter() : counter_(0), deleter_(NULL)
        {
        }
        virtual ~RefCounter()
        {
            deleter_ = NULL;
        }

        void SetDeleter(DELETER * p)
        {
            deleter_ = p;
        }

        long AddRef()
        {
            return __sync_add_and_fetch(&counter_, 1);
        }

        long DelRef()
        {
            long ret = __sync_sub_and_fetch(&counter_, 1);
            if (ret == 0)
            {
                if (deleter_)
                {
                    DERIVED * p = dynamic_cast<DERIVED *>(this);
                    if (p) deleter_->Delete(p);
                }
            }
            return ret;
        }

    private:
        long counter_;
        DELETER * deleter_;
    };

};



#endif


