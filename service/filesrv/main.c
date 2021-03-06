#include <sys/res.h>
#include <sys/vmm.h>
#include <sys/proc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/trigger.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/kfile.h>
#include <sys/ipc.h>
#include <ff.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define DEBUG_LOCAL 0

#define LIST_FILES 1

#include "filesrv.h"
#include "fatfs.h"

/**
 * filesrv - 文件服务
 */
int main(int argc, char *argv[])
{
    //printf("%s: started.\n", SRV_NAME);
    
    /* 绑定成为服务调用 */
    if (srvcall_bind(SRV_FS))  {
        printf("%s: bind srvcall failed, service stopped!\n", SRV_NAME);
        return -1;
    }
    
    /* 启动其它程序 */
    if (filesrv_execute()) {
        printf("%s: execute failed, service stopped!\n", SRV_NAME);
        return -1;
    }
    
    /* 初始化文件系统 */
    if (filesrv_init()) { /* 创建文件系统 */
        printf("%s: init filesystem failed, service stopped!\n", SRV_NAME);
        return -1;
    }

    /* 初始化文件表 */
    if (filesrv_file_table_init()) {
        printf("%s: init file table failed, service stopped!\n", SRV_NAME);
        return -1;
    }
    
    /* 初始化对外接口 */
    if (filesrv_init_interface()) {
        printf("%s: init interface failed, service stopped!\n", SRV_NAME);
        return -1;
    }

    if (filesrv_create_files()) {
        printf("%s: create file failed, service stopped!\n", SRV_NAME);
        return -1;
    }
#if LIST_FILES == 1
    char scan_path[256];
    memset(scan_path, 0, 256);
    strcpy(scan_path, "0:");
    /* 扫描文件 */
    fatfs_scan_files(scan_path);
#endif
    printf("\n%s: enter receving request state.\n", SRV_NAME);
    
    int seq;
    srvarg_t srvarg;
    int callnum;
    while (1)
    {
        memset(&srvarg, 0, sizeof(srvarg_t));
        /* 1.监听服务 */
        if (srvcall_listen(SRV_FS, &srvarg)) {  
            continue;
        }

#if DEBUG_LOCAL == 1
        printf("%s: srvcall seq=%d.\n", SRV_NAME, seq);
#endif 
        /* 2.处理服务 */
        callnum = GETSRV_DATA(&srvarg, 0, int);
        if (callnum >= 0 && callnum < FILESRV_CALL_NR) {
            filesrv_call_table[callnum](&srvarg);
        }
       
        seq++;
        /* 3.应答服务 */
        srvcall_ack(SRV_FS, &srvarg);   

    }
    return 0;
}
