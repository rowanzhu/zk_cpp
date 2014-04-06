/*
 * @brief: C++ interface of Zooleeper client based on Zooleeper C client
 * @author: Rowan Zhu <rowanzhu@yeah.net>
 */
#ifndef _ZK_CPP_INTERFACE_
#define _ZK_CPP_INTERFACE_

#include <zookeeper/zookeeper.h>

class ZkCpp
{
    public:
        ZkCpp();
        virtual ~ZkCpp();

        int Connect(const char *p_szHostList);
        int Close();

        int SendGetData(const char *p_szPath, void *p_pUserData);

        int ProcessRecv();

        const char *GetErrorString() const { return m_ErrorString; }
    protected:
        /*
         * User-defined behavior by inherit
         */
        virtual void _OnDataCompletion(int p_pRet, const char * p_pData, 
                int p_iDataLen, void *p_pUserData) = 0;
    private:
        struct ComplexUserData
        {
            ZkCpp *pSelf;
            void *pUserData;
        };
        static int _DataCompletion(int rc, const char *value, int value_len, 
                const struct Stat *stat, const void *data);
    private:
        zhandle_t *m_ZkHdl;
        char m_ErrorString[256];
};

#endif
