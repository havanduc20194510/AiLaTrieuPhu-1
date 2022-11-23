#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ctype.h>
#include <time.h>
#include <poll.h>

#define MAX_LINE 1024
#define BUFF_SIZE 1024

int choice1, choice2, choice3, sockfd, show1, show2, show3;
int recvBytes, sendBytes;
char sendBuff[MAX_LINE] = {0}, recvBuff[MAX_LINE];
struct sockaddr_in server, client;
char serverIP[20];
int serverPort;

enum msg_type
{
  LOGIN,
  LOGIN_SUCCESS,
  LOGIN_FAIL,
  SIGNUP,
  ACCOUNT_EXIST,
  SIGNUP_CONTINUE,
  SIGNUP_SUCCESS,
  SIGNUP_FAIL,
  CHANGE_PASS,
  CHANGE_PASS_SUCCESS,
  PLAY_ALONE,
  PLAY_PVP,
  LOGOUT
};

typedef struct _message
{
  enum msg_type type;
  char data_type[25];
  int length;
  char value[BUFF_SIZE];
} Message;

typedef struct _account
{
  char username[MAX_LINE];
  int login_status; // 0: not login; 1: logged in
} Account;
Account acc;

/*--------------------- Function Declaration -------------------------*/
int is_number(const char *s);
int validate_ip(char *ip);
int menu_start();
int menu_not_login();
int menu_logged();
int connect_to_server();
int show_menu_not_login();
int show_menu_logged();

/*--------------------- Ultilities -------------------------*/
int is_number(const char *s)
{
  while (*s)
  {
    if (!isdigit(*s))
    { // if the character is not a number, return false
      return 0;
    }
    s++; // point to next character
  }
  return 1;
}

int validate_ip(char *ip)
{ // check whether the IP is valid or not
  int i, num, dots = 0;
  char *ptr;
  if (ip == NULL)
    return 0;
  ptr = strtok(ip, "."); // cut the string using dor delimiter
  if (ptr == NULL)
    return 0;
  while (ptr)
  {
    if (!is_number(ptr)) // check whether the sub string is holding only number or not
      return 0;
    num = atoi(ptr); // convert substring to number
    if (num >= 0 && num <= 255)
    {
      ptr = strtok(NULL, "."); // cut the next part of the string
      if (ptr != NULL)
        dots++; // increase the dot count
    }
    else
      return 1;
  }
  if (dots != 3) // if the number of dots are not 3, return false
    return 0;
  return 1;
}

int menu_start()
{
  int op;
  char input[MAX_LINE];
  do
  {
    printf("\n------------------------------\n");
    printf("1. Bắt đầu.\n");
    printf("2. Cách chơi.\n");
    printf("3. Tác giả.\n");
    printf("4. Thoát.\n");
    printf("Nhập lựa chọn của bạn: ");
    scanf(" %[^\n]", input);
    if (strlen(input) != 1 || !isdigit(input[0]))
      break;
    op = atoi(input);
  } while (op > 5 || op < 1);
  return op;
}

int menu_not_login()
{
  char input[MAX_LINE];
  int op;
  do
  {
    printf("\n************************************\n");
    printf("\t1. Đăng nhập\n");
    printf("\t2. Đăng ký\n");
    printf("\t3. Trở về\n");
    scanf(" %[^\n]", input);
    if (strlen(input) != 1 || !isdigit(input[0]))
      break;
    op = atoi(input);
  } while (op > 3 || op < 1);
  return op;
}

int menu_logged()
{
  char input[MAX_LINE];
  int op;
  do
  {
    printf("\n************************************\n");
    printf("\t1. Thay đổi mật khẩu.\n");
    printf("\t2. Play Alone\n");
    printf("\t3. Play PvP.\n");
    printf("\t4. Đăng xuất.\n");
    scanf(" %[^\n]", input);
    if (strlen(input) != 1 || !isdigit(input[0]))
      break;
    op = atoi(input);
  } while (op > 4 || op < 1);
  return op;
}

int connect_to_server()
{
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Socket creation error \n");
    return 0;
  }
  printf("Server Address: %s", serverIP);

  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(serverIP);
  inet_aton(serverIP, &server.sin_addr);
  server.sin_port = htons(serverPort);

  printf("\n************************************\n\nConnecting....\n\n");
  if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    printf("\nConnection Failed \n");
    return 0;
  }
  printf("Connected!\n");
  return 1;
}

int show_menu_not_login()
{
  Message msg;
  char username[100], password[100];
  int show_menu_not_login = 1;
  while (show_menu_not_login)
  {
    int choice = menu_not_login();

    switch (choice)
    {
    case 1:
      msg.type = LOGIN;
      printf("Username: ");
      scanf(" %[^\n]", username);
      printf("Password: ");
      scanf(" %[^\n]", password);
      strcpy(msg.data_type, "string");
      strcpy(msg.value, username);
      strcat(msg.value, " ");
      strcat(msg.value, password);
      msg.length = strlen(msg.value);
      if (send(sockfd, &msg, sizeof(msg), 0) < 0)
      {
        printf("Send failed\n");
      }
      else
      {
        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
        if (recvBytes < 0)
        {
          printf("Receive failed\n");
        }
        else
        {
          if (msg.type == LOGIN_SUCCESS)
          {
            acc.login_status = 1;
            strcpy(acc.username, username);
            printf("%s\n", msg.value);
            show_menu_logged();
          }
          else
          {
            printf("%s\n", msg.value);
          }
        }
      }
      break;
    case 2:
      msg.type = SIGNUP;
      printf("Username: ");
      scanf(" %[^\n]", username);
      strcpy(msg.value, username);
      strcpy(msg.data_type, "string");
      if (send(sockfd, &msg, sizeof(msg), 0) < 0)
      {
        printf("Send failed\n");
      }
      else
      {
        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
        if (recvBytes < 0)
        {
          printf("Receive failed\n");
        }
        else
        {
          if (msg.type == ACCOUNT_EXIST)
          {
            printf("%s!\n", msg.value);
            break;
          }
        }
      }
      printf("Password: ");
      scanf(" %[^\n]", password);
      strcat(msg.value, " ");
      strcat(msg.value, password);
      msg.length = strlen(msg.value);
      if (send(sockfd, &msg, sizeof(msg), 0) < 0)
      {
        printf("Send failed\n");
      }
      else
      {
        recvBytes = recv(sockfd, &msg, sizeof(msg), 0);
        if (recvBytes < 0)
        {
          printf("Receive failed\n");
        }
        else
        {
          if (msg.type == SIGNUP_SUCCESS)
          {
            printf("%s\n", msg.value);
          }
        }
      }
      break;
    case 3:
      printf("Trở về\n");
      show_menu_not_login = 0;
      break;
    default:
      printf("Lựa chọn không hợp lệ\n");
      break;
    }
  }
  return 0;
}

int show_menu_logged()
{
  int show_menu_login = 1;
  Message msg;
  char pass[BUFF_SIZE], re_pass[BUFF_SIZE];

  while (show_menu_login)
  {
    int choice = menu_logged();
    switch (choice)
    {
    case 1:
      while (1)
      {
        printf("Mật khẩu mới: ");
        scanf(" %[^\n]", pass);
        printf("Nhập lại mật khẩu mới: ");
        scanf(" %[^\n]", re_pass);
        if (strcmp(pass, "") == 0 || strcmp(re_pass, "") == 0)
        {
          printf("Mật khẩu không được để trống\n");
          continue;
        }    

        if (strcmp(pass, re_pass) == 0)
        {
          msg.type = CHANGE_PASS;
          strcpy(msg.value, pass);
          send(sockfd, &msg, sizeof(msg), 0);
          recv(sockfd, &msg, sizeof(msg), 0);
          printf("%s\n", msg.value);
          break;
        }
        else
        {
          printf("Mật khẩu không khớp!\n");
        }
      }
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      msg.type = LOGOUT;
      send(sockfd, &msg, sizeof(msg), 0);
      printf("Đăng xuất\n");
      acc.login_status = 0;
      show_menu_login = 0;
      break;
    default:
      printf("Lựa chọn không hợp lệ\n");
      break;
    }
  }
  return 0;
}