#include <cstdio>
#include <string.h>
#include <unistd.h>
#include <zookeeper/zookeeper.h>

zhandle_t* zkhandle;
bool bStop = false;

int startsWith(const char *line, const char *prefix) 
{
    int len = strlen(prefix);
    return strncmp(line, prefix, len) == 0;
}

static const char* state2String(int state){
  if (state == 0)
    return "CLOSED_STATE";
  if (state == ZOO_CONNECTING_STATE)
    return "CONNECTING_STATE";
  if (state == ZOO_ASSOCIATING_STATE)
    return "ASSOCIATING_STATE";
  if (state == ZOO_CONNECTED_STATE)
    return "CONNECTED_STATE";
  if (state == ZOO_EXPIRED_SESSION_STATE)
    return "EXPIRED_SESSION_STATE";
  if (state == ZOO_AUTH_FAILED_STATE)
    return "AUTH_FAILED_STATE";

  return "INVALID_STATE";
}

static const char* type2String(int state){
  if (state == ZOO_CREATED_EVENT)
    return "CREATED_EVENT";
  if (state == ZOO_DELETED_EVENT)
    return "DELETED_EVENT";
  if (state == ZOO_CHANGED_EVENT)
    return "CHANGED_EVENT";
  if (state == ZOO_CHILD_EVENT)
    return "CHILD_EVENT";
  if (state == ZOO_SESSION_EVENT)
    return "SESSION_EVENT";
  if (state == ZOO_NOTWATCHING_EVENT)
    return "NOTWATCHING_EVENT";

  return "UNKNOWN_EVENT_TYPE";
}

void zktest_watcher_g(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx)
{
    printf("watcher|%s|%s\n", type2String(type), state2String(state));
    if(state == ZOO_CONNECTED_STATE)
    {
    }
}

void my_data_completion(int rc, const char *value, int value_len,
        const struct Stat *stat, const void *data) {
    
    printf("my_data_completion:%d\n", reinterpret_cast<long>(data));
    if (value) {
        fprintf(stderr, "value_len = %d\n", value_len);
        write(2, value, value_len);
        printf("\n");
    }
    //free((void*)data);
}

void processline(char *line) 
{
    int rc = 0;
    printf("processline:%s\n", line);
    if(startsWith(line, "get"))
    {
        line += 4;
        char *pTmp = strchr(line, '\n');
        if(pTmp)
        {
            pTmp = '\0';
        }
        rc = zoo_aget(zkhandle, "/rowan", 1, my_data_completion, /*strdup("/zk_test")*/(const void *)1);
        rc = zoo_aget(zkhandle, "/rowan", 1, my_data_completion, /*strdup("/rowan")*/(const void *)2);

        if (rc) 
        {
            printf("This is Error %d for %s\n", rc, line);
        }
    }else if(startsWith(line, "quit"))
    {
        bStop = true;
    }
}

int main()
{
    const char* host = "172.25.40.237:2181";
    int timeout = 30000;

    zkhandle = zookeeper_init(host, zktest_watcher_g, timeout, 0, 0, 0);
    if(NULL == zkhandle)
    {
        printf("NULL\n");
        return 0;
    }

    int events = 0;
    do
    {
        fd_set rfds, wfds, efds;
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&efds);

        int fd;
        int interest;
        events = 0;
        struct timeval tv;
        int rc;
        zookeeper_interest(zkhandle, &fd, &interest, &tv);
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
        FD_SET(0, &rfds);
        rc = select(fd+1, &rfds, &wfds, &efds, &tv);
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

        //printf("events:%d\n",events);
        zookeeper_process(zkhandle, events);

        if (FD_ISSET(0, &rfds)) {
            char buffer[512] = {0};
            int rc;
            int len = sizeof(buffer);
            if (len <= 0) {
                fprintf(stderr, "Can't handle lines that long!\n");
                exit(2);
            }
            rc = read(0, buffer, len);
            if (rc <= 0) {
                fprintf(stderr, "bye\n");
                break;
            }
            processline(buffer);
        }
    }while(!bStop);

    zookeeper_close(zkhandle);
    printf("end:zookeeper_close\n");

    return 0;
}
