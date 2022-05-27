#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include"cgic.h"
#define BufferLen 1024
int cgiMain(void){

    cgiHeaderContentType("text/html");

    cgiFilePtr file;
	char name[1024];
	char contentType[1024];
	char buffer[1024];
	int size;
	int got;
	if (cgiFormFileName("file", name, sizeof(name)) != cgiFormSuccess) {
		printf("<p>No file was uploaded.<p>\n");
		return 0;
	} 
	// fprintf(cgiOut, "The filename submitted was: ");
	// cgiHtmlEscape(name);
	// fprintf(cgiOut, "<p>\n");
	// cgiFormFileSize("file", &size);
	// fprintf(cgiOut, "The file size was: %d bytes<p>\n", size);
	// cgiFormFileContentType("file", contentType, sizeof(contentType));
	// fprintf(cgiOut, "The alleged content type of the file was: ");
	// cgiHtmlEscape(contentType);
	// fprintf(cgiOut, "<p>\n");
	// fprintf(cgiOut, "Of course, this is only the claim the browser made when uploading the file. Much like the filename, it cannot be trusted.<p>\n");
	// fprintf(cgiOut, "The file's contents are shown here:<p>\n");
	if (cgiFormFileOpen("file", &file) != cgiFormSuccess) {
		fprintf(cgiOut, "Could not open the file.<p>\n");
		return 0;
	}

    //打开或创建文件在当前目录，即/var/www/cgi-bin
    //注意文件存取权限，否如会出现文件无法访问
    mode_t mode=S_IRWXU|S_IRGRP|S_IROTH; //
    int fd = open(name,O_RDWR|O_CREAT|O_TRUNC,mode);
    if(fd < 0){
        fprintf(cgiOut, "<p>Could not create the file,error:%d<p>\n",fd);
    }

    // fprintf(cgiOut, "<pre>\n");

    while (cgiFormFileRead(file, buffer, sizeof(buffer), &got) ==
        cgiFormSuccess)
    {   
        //写入文件

        if(got>0){
            write(fd,buffer,got);
        }

        // cgiHtmlEscapeData(buffer, got);
    }
    fprintf(cgiOut, "OK!\n");
    cgiFormFileClose(file);
    close(fd);
}