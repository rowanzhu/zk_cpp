#include <cstdio>
#include <string.h>
#include "example.h"

void MyZkCpp::_OnDataCompletion(int p_pRet, const char * p_pData, int p_iDataLen, void *p_pUserData)
{
    if(0 != p_pRet)
    {
        printf("MyZkCpp::_OnDataCompletion|Ret|%d\n",p_pRet);
        return;
    }

    //TODO: p_pUserData could be transaction id or something else to dispatch message
    //In this example, only print it
    long lWhat = reinterpret_cast<long>(p_pUserData);
    if(p_pData)
    {
        char Data[1024] = {0};
        memcpy(Data, p_pData, p_iDataLen);
        
        printf("Data|%d|%s|%ld\n", p_pRet, Data, lWhat);
    }else
    {
        printf("MyZkCpp::_OnDataCompletion|NullData\n");
    }
}

int main()
{
    MyZkCpp zk;
    const char* host = "172.25.40.237:2181";

    if(0 != zk.Connect(host))
    {
        printf("%s\n", zk.GetErrorString());
        return 0;
    }

    while(true)
    {
        zk.ProcessRecv();
    }
    
    return 0;
}
