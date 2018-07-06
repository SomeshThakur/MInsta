#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>

#define FILE_SIZE 1024*1024*100
#define LOG_TAG "SMA ERROR"
#define FILE_NAME "file_system.bin"
#define MAX_POSTS 50
#define MAXBUF 100
#define MAX_COMMENTS 20

short num_of_users = 0;
int post_position = 0;
int *freelist=NULL;
char username[21];

typedef struct
{
	short userid;        //id to uniquely identify users who commented on my post
	char del;            // del = 0 then comment deleted ,   del > 0 then it is comment size
	char comment[50];
}comment;

typedef struct
{
	short userid;                //this iunique;ly identifies a user
	short likes;                //no of likes for this post
	char filename[20];            //if filename == ""  then this post is deleted , else it consisits of filename
	int post_off_set;                //this offset refers to the position of 
	int size;
	comment comments[20];
} post;


typedef struct
{
	short userid;
	short no_of_posts;
	int user_post_offsets[50];
	char username[20];
	char password[20];
}user;

