
#include"structs.h"
#include"files_systems_api.h"

char * scan_password()
{
	char * password = (char *)malloc(sizeof(char)*MAXBUF);
	int i = 0;
	char c;
	int j = 0;
	//scanf("%c", &c);
	while ((c = _getch()) != 13)
	{
		password[i] = c;
		if (c == 8)
		{
			if (i == 0)continue;
			printf("\b \b"); i--;
		}
		else
		{
			printf("*");
			i++;
		}
	}
	password[i] = '\0';
	return password;
}


int main(){
	char ch;
	char buffer[256], buffer1[256], *password;

	/*
		Intialising Global Varilables
		*/
	FILE *fp = fopen(FILE_NAME, "rb+");
	if (fp == NULL){
		perror(LOG_TAG);
		system("pause");
		exit(1);
	}
	fread(&num_of_users, sizeof(short), 1, fp);
	fread(&post_position, sizeof(int), 1, fp);
	fclose(fp);


	// Use this function to create empty file system
	//init();
	do{
		system("cls");
		print_header();
		printf("\n\t1. Create User");
		printf("\n\t2. Upload Post/File ");
		printf("\n\t3. View Posts");
		printf("\n\t(E). EXIT ");
		printf("\n\tEnter your choice : ");
		scanf("%c", &ch);
		switch (ch){
		case '1':
			system("cls");
			print_header();
			printf("\n\tEnter Username Name : ");
			scanf("%s", buffer);
			printf("\n\tEnter Password : ");
			password = scan_password();
			create_user(buffer, password);
			break;
		case '2':
			system("cls");
			print_header();
			printf("\n\tEnter Your Name : ");
			scanf("%s", buffer1);
			printf("\n\tEnter Your Post Name : ");
			scanf("%s", buffer);
			create_post(buffer1, buffer);
			break;
		case '3':
			system("cls");
			print_header();
			view_post();
			break;
		default:
			break;
		}
	} while (ch != 'E');

	return 0;
}