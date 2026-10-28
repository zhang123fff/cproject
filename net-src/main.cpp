#pragma once
#include<sys/socket.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<netdb.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<iostream>
#include<vector>
#include<unistd.h>
#include<memory>
#include<functional>
#include<memory.h>

/*
    void test(int i)
{
  int n;
  char *p = malloc(n);
  int y = n;
  p[y] = 'a';

}

struct s
{
  int a; int b;
}s1;

void test()
{
  int n, i;
  struct s *p = malloc(n * sizeof(struct s));
  if (i <= n)
  p[i] = s1;
}

*/


void test(int i){
    if(i <= 0) return;
    int n = i;
    char *p = (char*)malloc(n);
    
    if(!p) return;

    //初始化
    memset(p, '\0', n*sizeof(char));
    int y = n - 1;
    
    p[y] = 'a';

    free(p);
}

struct s{
    //初始化
    int a = 0; int b = 0;
}s1;

//函数重载
void test(int n, int i){
    if(n <= 0 || i < 0) return;
    struct s *p = (struct s*)malloc(n * sizeof(struct s));
    if(!p) return;
    //初始化
    memset(p, 0, n*sizeof(struct s));
    if (i < n) p[i] = s1;
    free(p);
}



int main(){
    

    

    return 0;
}