#include <stdio.h>
#include "logger.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/inotify.h>
#include <sys/types.h>

#define BUF_LEN sizeof(struct inotify_event) * 1024

int file, writer, reader;
char* p;
struct inotify_event *event;


void displayEvent(struct inotify_event* event);

int main(char argc, char** argv){

    if(argc < 2)
    {
        errorf("Input Invalido\n");
        return -1;
    }

    file = inotify_init1(O_NONBLOCK);
    writer = inotify_add_watch(file, argv[1], IN_ALL_EVENTS);
    char* buffer = (char*)malloc(BUF_LEN);

    while(1)
    {
        reader = read(file, buffer, BUF_LEN);
        p = buffer;
        event = (struct inotify_event*)p;
        for(p = buffer; p < buffer + reader; )
        {
            event = (struct inotify_event *) p;
            displayEvent(event);
            p += sizeof(struct inotify_event) + event -> len;
        }
    }
    close(file);
    return 0;
}

void displayEvent(struct inotify_event* event){
    if(event->mask & IN_ACCESS)     
    infof("ACCESS Folder\n");
    if(event->mask & IN_CREATE)     
    infof("CREATE Folder\n");
    if(event->mask & IN_DELETE)     
    errorf("DELETE Folder\n");
    if(event->mask & IN_OPEN)       
    infof("OPEN Folder\n");
    if(event->mask & IN_MODIFY)     
    warnf("MODIFY Folder\n");

}