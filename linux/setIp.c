/*
 * 设置linux ip需要有root权限方可， 并且在rk3399上运行的可执行程序，在编译的时候需要加上 -pie方可正常执行
*/

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <errno.h>

#define SET_OK 0
#define SET_ERR 1

/*
 * ifname : 接口名称
 * ipaddr : 需要设置的ip地址
 * mast   : 掩码
 * gateway: 网关地址
 */
int SetIfAddr(char *ifname, char *ipaddr, char *mask,char *gateway)
{
    int fd;
    int rc = 0;
    struct ifreq ifr; 
    struct sockaddr_in *sin;
    struct rtentry  rt;
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
            perror("socket   error");     
            return -1;     
    }
    memset(&ifr,0,sizeof(ifr)); 
    strcpy(ifr.ifr_name,ifname); 
    sin = (struct sockaddr_in*)&ifr.ifr_addr;     
    sin->sin_family = AF_INET;  
   
    //ip地址
    if(inet_aton(ipaddr,&(sin->sin_addr)) < 0)   
    {     
        perror("inet_aton   error");     
        return -2;     
    }    
    
    if(ioctl(fd, SIOCSIFADDR, &ifr) < 0)   
    {     
        perror("ioctl   SIOCSIFADDR   error");     
        return -3;     
    }

    //子网掩码
    if(inet_aton(mask,&(sin->sin_addr)) < 0)   
    {     
        perror("inet_pton   error");     
        return -4;     
    }    
    if(ioctl(fd, SIOCSIFNETMASK, &ifr) < 0)
    {
        perror("ioctl");
        return -5;
    }

    //网关
    memset(&rt, 0, sizeof(struct rtentry));
    memset(sin, 0, sizeof(struct sockaddr_in));
    sin->sin_family = AF_INET;
    sin->sin_port = 0;
    
    if(inet_aton(gateway, &sin->sin_addr)<0)
    {
       printf ( "inet_aton error\n" );
    }
    
    memcpy ( &rt.rt_gateway, sin, sizeof(struct sockaddr_in));
    ((struct sockaddr_in *)&rt.rt_dst)->sin_family=AF_INET;
    ((struct sockaddr_in *)&rt.rt_genmask)->sin_family=AF_INET;
    rt.rt_flags = RTF_GATEWAY;
    
    if (ioctl(fd, SIOCADDRT, &rt)<0)
    {
        printf( "ioctl(SIOCADDRT) error in set_default_route\n");
        close(fd);
        return -1;
    }
    close(fd);
    return rc;
}

int main(void)
{
	if (SetIfAddr("eth0", "192.168.0.111", "255.255.255.0", "192.168.0.1")) 
	{
		printf("Set ip error  %d \n", errno);
	}

	printf("Set ip ok \n");
	
	return SET_OK;
}
