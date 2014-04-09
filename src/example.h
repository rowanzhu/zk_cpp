/*
 * @brief:  example code
 * @author: Rowan Zhu <rowanzhu@yeah.net>
 */
#ifndef _ZK_CPP_INTERFACE_EXAMPLE_
#define _ZK_CPP_INTERFACE_EXAMPLE_

#include "zk_cpp.h"

class MyZkCpp: public ZkCpp
{
    protected:
        virtual void _OnDataCompletion(int p_pRet, const char * p_pData,
                int p_iDataLen, void *p_pUserData);
};

#endif
