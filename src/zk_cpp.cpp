#include <cstdio>
#include <unistd.h>
#include "zk_cpp.h"

ZkCpp::ZkCpp()
{
    m_ZkHdl = NULL;
    m_ErrorString[0] = '\0';
}

ZkCpp::~ZkCpp()
{
    Close();
}

int ZkCpp::Connect(const char *p_szHostList)
{
    const int TIME_OUT = 30000;
    m_ZkHdl = zookeeper_init(p_szHostList, ZkCpp::_GlobalWatcher, TIME_OUT, 0, this, 0);
    if(NULL == m_ZkHdl)
    {
        snprintf(m_ErrorString, ERROR_STRING_LEN, "zookeeper_init failed");
        return -1;
    }

    return 0;
}

int ZkCpp::Close()
{
    if(NULL != m_ZkHdl)
    {
        zookeeper_close(m_ZkHdl);
        m_ZkHdl = NULL;
    }
    
    return 0;
}

int ZkCpp::SendGetData(const char *p_szPath, void *p_pUserData)
{
    if(NULL == p_szPath)
    {
        snprintf(m_ErrorString, ERROR_STRING_LEN, "NULL Path");
        return -1;
    }

    if(!IsConnectedState())
    {
        snprintf(m_ErrorString, ERROR_STRING_LEN, "StateError|%d", zoo_state(m_ZkHdl));
        return -1;
    }

    ComplexUserData *pCUserData = new ComplexUserData();
    if(NULL == pCUserData)
    {
        snprintf(m_ErrorString, ERROR_STRING_LEN, "Heap Error");
        return -2;
    }
    pCUserData->pSelf = this;
    pCUserData->pUserData = p_pUserData;

    int iRet = zoo_aget(m_ZkHdl, p_szPath, 0, ZkCpp::_DataCompletionCB, pCUserData);

    if(0 != iRet)
    {
        snprintf(m_ErrorString, ERROR_STRING_LEN, "zoo_aget|%d|%s", iRet, p_szPath);
        return -3;
    }

    return 0;
}

int ZkCpp::ProcessRecv()
{
    fd_set rfds, wfds, efds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);

    int fd = -1;
    int interest = 0;
    int events = 0;
    struct timeval tv;
    int rc = 0;
    zookeeper_interest(m_ZkHdl, &fd, &interest, &tv); //TODO: confirm no waiting
    if(fd != -1)
    {
        if (interest&ZOOKEEPER_READ) 
        {
            FD_SET(fd, &rfds);
        }
        if (interest&ZOOKEEPER_WRITE)
        {
            FD_SET(fd, &wfds);
        }
    }
    
    rc = select(fd+1, &rfds, &wfds, &efds, &tv); //TODO: confirm no waiting
    if (rc > 0) 
    {
        if (FD_ISSET(fd, &rfds))
        {
            events |= ZOOKEEPER_READ;
        }
        if (FD_ISSET(fd, &wfds))
        {
            events |= ZOOKEEPER_WRITE;
        }
    }

    zookeeper_process(m_ZkHdl, events);

    return 0;
}

void ZkCpp::_DataCompletionCB(int rc, const char *value, int value_len, 
        const struct Stat *stat, const void *data)
{
    if(NULL == data)
    {
        printf("ZkCpp::_DataCompletionCB|NULLData\n");
        return;
    }

    ComplexUserData *pCUserData = reinterpret_cast<ComplexUserData*>(const_cast<void*>(data));
    if(NULL == pCUserData->pSelf)
    {
        printf("ZkCpp::_DataCompletionCB|NULLSelf\n");
        return;
    }

    pCUserData->pSelf->_OnDataCompletion(rc, value, value_len, pCUserData->pUserData);

    delete pCUserData;
}

void ZkCpp::_GlobalWatcher(zhandle_t *p_pZH, int p_iType, int p_iState, 
        const char *p_szPath, void *p_pWatcherCtx)
{
    printf("GlobalWatcher|%s|%s\n", Type2String(p_iType), State2String(p_iState));
}

const char *ZkCpp::State2String(int p_iState)
{
    if (p_iState == 0)
        return "CLOSED_STATE";
    if (p_iState == ZOO_CONNECTING_STATE)
        return "CONNECTING_STATE";
    if (p_iState == ZOO_ASSOCIATING_STATE)
        return "ASSOCIATING_STATE";
    if (p_iState == ZOO_CONNECTED_STATE)
        return "CONNECTED_STATE";
    if (p_iState == ZOO_EXPIRED_SESSION_STATE)
        return "EXPIRED_SESSION_STATE";
    if (p_iState == ZOO_AUTH_FAILED_STATE)
        return "AUTH_FAILED_STATE";

    return "INVALID_STATE";
}

const char *ZkCpp::Type2String(int p_iType)
{
    if (p_iType == ZOO_CREATED_EVENT)
        return "CREATED_EVENT";
    if (p_iType == ZOO_DELETED_EVENT)
        return "DELETED_EVENT";
    if (p_iType == ZOO_CHANGED_EVENT)
        return "CHANGED_EVENT";
    if (p_iType == ZOO_CHILD_EVENT)
        return "CHILD_EVENT";
    if (p_iType == ZOO_SESSION_EVENT)
        return "SESSION_EVENT";
    if (p_iType == ZOO_NOTWATCHING_EVENT)
        return "NOTWATCHING_EVENT";

    return "UNKNOWN_EVENT_TYPE";
}

bool ZkCpp::IsConnectedState()
{
    if(NULL == m_ZkHdl)
    {
        return false;
    }
    
    if(ZOO_CONNECTED_STATE == zoo_state(m_ZkHdl))
    {
        return true;
    }else
    {
        return false;
    }
}
