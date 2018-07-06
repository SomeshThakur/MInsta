// Decorating Printer
void print_header(){
	printf("\n\t----------[ SOCIAL MEDIA APP ]---------\n\n");
}

// Initialize App
void init()
{
	FILE *fp;
	fp = fopen(FILE_NAME, "wb+");
	if (fp == NULL){
		perror(LOG_TAG);
		system("pause");
		exit(1);
	}

	int i = 0;
	// Short for number of users
	fwrite(&i, sizeof(short), 1, fp);

	// Offset of Post
	int end = FILE_SIZE - 1;
	fwrite(&end, sizeof(int), 1, fp);

	char c = 0;
	// Writing last byte
	fseek(fp, FILE_SIZE - 1, SEEK_SET);
	fwrite(&c, sizeof(char), 1, fp);

	fclose(fp);
}

/*
	Method : short search_user(char *username)
	Search for user with given username and returns userid

	Arguments :
		char * uername : Username to search
*/

user* search_user(char *username){
	FILE *fp = fopen(FILE_NAME, "rb+");
	if (fp == NULL){
		perror(LOG_TAG);
		system("pause");
		exit(1);
	}
	static user u;
	fread(&num_of_users, sizeof(short), 1, fp);
	rewind(fp);
	fseek(fp, sizeof(short)+sizeof(int), SEEK_SET);

	for (int i = 0; i < num_of_users; i++){
		fread(&u, sizeof(user), 1, fp);
		if (!strcmp(u.username, username)){
			// Adding Post id to user 
			return &u;
		}
	}
	fclose(fp);
	return NULL;
}

/*
	Method : post search_post(char *username, char*postname)
	Search for post with given username and postname

	Arguments :
		char * uername : Username to search
*/

post search_post(char *username, char*postname){
	FILE *fp = fopen(FILE_NAME, "rb+");
	if (fp == NULL){
		perror(LOG_TAG);
		system("pause");
		exit(1);
	}
	user *u = search_user(username);
	post p;

	for (int i = 0; i < u->no_of_posts; i++)
	{
		fseek(fp, u->user_post_offsets[i], SEEK_SET);
		fread(&p, sizeof(p), 1, fp);
		if (!strcmp(p.filename, postname))
		{
			return p;
		}
	}
	p.userid = -1;
	return p;
}

/*
	Method : void create_user(FILE *fp, char *username)
	Creates a new user in File system.

	Arguments:
	Filename : File name of File system.
	Username : new user's name.
	*/

void create_user(char *username,char *password)
{
	// Opening file
	FILE *fp = fopen(FILE_NAME, "rb+");

	if (fp == NULL){
		perror(LOG_TAG);
		system("pause");
		return;
	}

	// Searching if user already exits
	if (search_user(username) != NULL){
		printf("\n\t User Already exits..!\n");
		system("pause");
		return;
	}

	// Creating User Struct
	user _user;

	// Fetching num of users in F.S.
	fread(&num_of_users, sizeof(short), 1, fp);

	//Incrementing Users count 
	num_of_users++;

	// Rewind to override count
	rewind(fp);

	// Updating count
	fwrite(&num_of_users, sizeof(short), 1, fp);

	// Intializing User struct
	_user.no_of_posts = 0;
	_user.userid = num_of_users;
	strcpy(_user.username, username);
	strcpy(_user.password, password);
	for (int i = 0; i < MAX_POSTS; i++){
		_user.user_post_offsets[i] = -1;
	}

	// Seeking to position to append new user
	fseek(fp, sizeof(user)*(num_of_users - 1) + sizeof(short)+sizeof(int), SEEK_SET);

	// Appending User
	fwrite(&_user, sizeof(user), 1, fp);

	//Closing FS file
	fclose(fp);
}

/*
	Method : int check_available_free_block(int size)
	Checks for available free block in list

	Arguments :
	int size : Size of new post
	*/

int check_available_free_block(char *username ,int size){
	FILE *fp = fopen(FILE_NAME, "rb+");
	if (fp == NULL){
		perror(LOG_TAG);
		system("pause");
		exit(1);
	}
	user *u = search_user(username);
	post p;

	for (int i = 0; i < u->no_of_posts; i++)
	{
		fseek(fp, u->user_post_offsets[i], SEEK_SET);
		fread(&p, sizeof(p), 1, fp);
		
	}
	return -1;
}

/*
	Updates User's post offset
	*/

void update_user_post_offset(int size_of_num_post, char *username)
{
	FILE *fp = fopen(FILE_NAME, "rb+");
	if (fp == NULL){
		perror(LOG_TAG);
		system("pause");
		return;
	}
	user *tmp = search_user(username);
	if (tmp == NULL){
		printf("\n\tUser Not found!\n");
		return;
	}
	post_position -= sizeof(post)+size_of_num_post;

	// Updating user's post 
	for (int i = 0; i < MAX_POSTS; i++)
	{
		if (tmp->user_post_offsets[i] == -1){
			tmp->user_post_offsets[i] = post_position;
			break;
		}
	}
	tmp->no_of_posts++;

	user *u = (user*)malloc(sizeof(user)*num_of_users);
	fseek(fp, sizeof(short)+sizeof(int), SEEK_SET);
	int usize = sizeof(user);
	for (int i = 0; i < num_of_users; i++)
	{
		fread(&u[i], sizeof(user), 1, fp);
		if (!strcmp(u[i].username, username))
		{
			fseek(fp, -usize, SEEK_CUR);
			fwrite(tmp, sizeof(user), 1, fp);
		}
	}
	free(u);
	fclose(fp);
}

/*
	Method : void create_post(FILE *fp,char *postname)
	Creates new Post in File System

	Arguments :
	Char *postname : File/Post Name
	*/

void create_post(char *username, char *postname)
{
	// Opening posting file
	FILE *newpost = fopen(postname, "rb+");
	FILE *fp = fopen(FILE_NAME, "rb+");
	if (fp == NULL || newpost == NULL){
		perror(LOG_TAG);
		system("pause");
		return;
	}

	// Finding size of file
	fseek(newpost, 0, SEEK_END);
	int size_of_new_post = ftell(newpost);
	rewind(newpost);
	update_user_post_offset(size_of_new_post, username);

	// Copying Buffer
	char *buffer = (char*)malloc(sizeof(char)*size_of_new_post + 1);
	fread(buffer, sizeof(char), size_of_new_post, newpost);
	fclose(newpost);

	// Wrtie meta data then write file
	post p;
	p.likes = 0;
	p.userid = search_user(username)->userid;
	p.post_off_set = post_position;
	p.size = size_of_new_post;
	for (int i = 0; i < MAX_COMMENTS; i++){
		p.comments[i].del = 0;
	}
	p.likes = 0;
	strcpy(p.filename, postname);

	if (check_available_free_block(username,size_of_new_post) >0)
	{
		// Available free block wrtie 2 meta data

	}
	else
	{
		fseek(fp, post_position, SEEK_SET);
		fwrite(&p, sizeof(p), 1, fp);
	}

	fwrite(buffer, sizeof(char), size_of_new_post, fp);
	fwrite(&p, sizeof(post), 1, fp);

	// Goto start to update post position
	rewind(fp);
	fseek(fp, sizeof(short), SEEK_SET);
	fwrite(&post_position, sizeof(int), 1, fp);

	//Closing file
	fclose(fp);
}

/*
	Method : void display_all_users(char *filename)
	Display all users

	Arguments :
	char *filename : File System name
	*/

void display_all_users()
{
	FILE *fp = fopen(FILE_NAME, "rb+");
	if (fp == NULL){
		perror(LOG_TAG);
		system("pause");
		return;
	}
	fseek(fp, sizeof(short)+sizeof(int), SEEK_SET);

	user tmp;
	int t = 1;
	while (t <= num_of_users){
		if ((fread(&tmp, sizeof(user), 1, fp)) > 0){
			printf("\n\t%15s", tmp.username);
		}
		t++;
	}
	fclose(fp);
}

/*
	Method : void download_post(char *username,char* postname,char *newfilename)
	Downloads a posted file from FS to user's local disk

	Arguments:
	char* username : Username of given post
	char* postname : Post name to identify post
	char* newfilenam : New file to save downloaded
	*/

void download_post(char *username, char* postname, char *newfilename)
{
	FILE *fp = fopen(FILE_NAME, "rb+");
	FILE *newfp = fopen(newfilename, "wb");

	user *u = search_user(username);
	post p;

	for (int i = 0; i < u->no_of_posts; i++)
	{
		fseek(fp, u->user_post_offsets[i], SEEK_SET);
		fread(&p, sizeof(p), 1, fp);
		if (!strcmp(p.filename, postname))
		{
			char *buffer = (char*)malloc(sizeof(char)*p.size + 1);
			buffer[p.size] = '\0';
			fread(buffer, sizeof(char), p.size, fp);
			fwrite(buffer, sizeof(char), p.size, newfp);
			fclose(newfp);
			fclose(fp);
			printf("\n\t File Download successfully...\n");
			break;
		}
	}
}

/*
	Method : void open(char*username, char *postname)
	Opens specific file from user post in FS

	Arguments:
		char *username : To which user does the post belongs to.
		char *postname : Postname
	
*/

void open(char *username, char *postname)
{
	// Download file to open
	download_post(username, postname, postname);

	// Opening
	system(postname);
}

/*
	Method : void add_comment(char* username, char* postname, char *comment)
	Add comment to particular file/image.

	Argumenta :
	char *username : To which user does the post belongs to.
	char *postname : Postname
	char *comment : Comment's content
	*/

void add_comment(char* username, char* postname, char *comment){
	FILE *fp = fopen(FILE_NAME, "rb+");

	user *u = search_user(username);
	post p;

	for (int i = 0; i < u->no_of_posts; i++)
	{
		fseek(fp, u->user_post_offsets[i], SEEK_SET);
		fread(&p, sizeof(p), 1, fp);
		if (!strcmp(p.filename, postname))
		{
			for (int j = 0; j < MAX_COMMENTS; j++)
			{
				if (p.comments[j].del == 0){
					strcpy(p.comments[j].comment, comment);
					p.comments[j].del = 1;
					p.comments[j].userid = u->userid;
					int psize = sizeof(post);
					fseek(fp, -psize, SEEK_CUR);
					fwrite(&p, sizeof(post), 1, fp);
					goto done;
				}
			}
		}
	}
done:
	fclose(fp);
	return;
}

/*
	Method : void add_like(char* username, char* postname, char *comment)
	Add like to particular file/image.

	Argumenta :
	char *username : To which user does the post belongs to.
	char *postname : Postname to add likes
	*/

void add_like(char* username, char* postname){

	FILE *fp = fopen(FILE_NAME, "rb+");

	user *u = search_user(username);
	post p;
	for (int i = 0; i < u->no_of_posts; i++)
	{
		fseek(fp, u->user_post_offsets[i], SEEK_SET);
		fread(&p, sizeof(p), 1, fp);
		if (!strcmp(p.filename, postname))
		{
			p.likes++;
			int psize = sizeof(post);
			fseek(fp, -psize, SEEK_CUR);
			fwrite(&p, sizeof(post), 1, fp);
			goto done;
		}
	}
done:
	fclose(fp);
	return;
}

/*
	Method : void display_comments(char*username, char *postname)
	Displays comments of particular post/image/file
	User can add comment, delete comment

	Arguments :
	char *username : To which user does the post belongs to.
	char *postname : Image/File name
	*/

void display_comments(char*username, char *postname){

	int ch, index;
	char content[25];
	int psize;
	FILE *fp = fopen(FILE_NAME, "rb+");
	if (fp == NULL){
		perror(LOG_TAG);
		system("pause");
		return;
	}

	user *u = search_user(username);
	post p;
	for (int i = 0; i < u->no_of_posts; i++)
	{
		fseek(fp, u->user_post_offsets[i], SEEK_SET);
		fread(&p, sizeof(post), 1, fp);
		if (!strcmp(p.filename, postname))
		{
			printf("\n\tIndex\tcomment");
			for (int j = 0; j < MAX_COMMENTS; j++){
				if (p.comments[j].del == 1){
					printf("\n\t%d %10s", j, p.comments[j].comment);
				}
			}
		}
	}

	printf("\n\n\t 1. Add comment");
	printf("\n\t 2. Delete comment\n");
	printf("\n\tEnter your choice : ");
	scanf("%d", &ch);
	switch (ch)
	{
	case 1:
		printf("\n\tEnter comment : ");
		scanf("%s", content);
		add_comment(username, postname, content);
		break;
	case 2:
		printf("\n\tEnter comment index : ");
		scanf("%d", &index);
		p.comments[index].del = 0;
		psize = sizeof(post);
		fseek(fp, -psize, SEEK_CUR);
		fwrite(&p, sizeof(post), 1, fp);
		fclose(fp);
		break;
	default:
		break;
	}
}

/*
	Method : void view_post()
	View all post of specific user and perform operations like download, comment,like,delete

	Arguments :
	None
	*/

void view_post()
{
	FILE *fp = fopen(FILE_NAME, "rb+");
	if (fp == NULL){
		perror(LOG_TAG);
		system("pause");
		return;
	}

	char buff[256], buff1[256];
	int ch, ch1;
	int psize = sizeof(post);
	char username[21];
	user *u = (user*)malloc(sizeof(user)*num_of_users);

	fseek(fp, sizeof(int)+sizeof(short), SEEK_SET);
	fread(u, sizeof(user), num_of_users, fp);

	printf("\t\tNames");
	display_all_users();
	printf("\n\tEnter Username (-b to go back): ");
	scanf("%s", &username);
	if (!strcmp(username, "-B"))
		return;
	user *curr_user = search_user(username);
	system("cls");

	post p;
show_posts:
	print_header();
	printf("\n\t All Posts of %s are :", curr_user->username);
	for (int i = 0; i < MAX_POSTS; i++)
	{
		if (curr_user->user_post_offsets[i] == -1)
			continue;
		else{
			fseek(fp, curr_user->user_post_offsets[i], SEEK_SET);
			if ((fread(&p, sizeof(post), 1, fp))>0)
			{
			if (p.userid >= 0){	
					printf("\n\t%15s", p.filename);
				}
			}
		}
	}
	printf("\n\t Enter post full file (-b to go back) : ");
	scanf("%s", buff);
	if (!strcmp(buff, "-B"))
		return;
loop_back:
	p = search_post(username, buff);
	printf("\n\t Number of Likes : %d", p.likes);
	printf("\n\t1. Download");
	printf(" 2. Comment");
	printf(" 3. Like");
	printf(" 4. Delete");
	printf(" 5. Open post");
	printf(" 6. Back\n");

	printf("\n\tEnter your choice : ");
	scanf("%d", &ch);
	switch (ch)
	{
	case 1:
		printf("\n\tEnter download filename : ");
		scanf("%s", buff1);
		download_post(username, buff, buff1);
		break;
	case 2:
		display_comments(username, buff);
		break;
	case 3:
		add_like(username, buff);
		system("cls");
		print_header();
		printf("\n\t Post name : %s", p.filename);
		goto loop_back;
		break;
	case 4:
		p.userid = -1;
		fseek(fp, -psize,SEEK_CUR);
		fwrite(&p, psize, 1, fp);
		goto show_posts;
		break;
	case 5:
		open(username,buff);
		break;
	default:
		break;
	}

	system("pause");
	fclose(fp);
}

/*
	Method : int Login(char *username,char* password)
*/
int login(char *username, char *password){
	FILE *fp = fopen(FILE_NAME, "rb+");
	if (fp == NULL){
		perror(LOG_TAG);
		system("pause");
		exit(1);
	}
	user* u = search_user(username);
	if (u == NULL){
		return 0;
	}

	if (!strcmp(u->password, password)){
		return 1;
	}
}