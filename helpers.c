#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

#include "helpers.h"

#define WIDTH 24
#define HEIGHT 14

int startx = 0;
int starty = 0;

int row, col;

char check_msg[] = "CHECK the status of the API";
char list_msg[] = "DOWNLOAD a 'list.txt' file with a list of all the available tokens";
char historic_msg[] = "CHECK the price from a specific date";
char search_msg[] = "SEARCH for possible matches";
char exit_msg[] = "EXIT the program";
char token_msg[] = "CHECK the current price of a token";

char* choices[] = {
    "CHECK API",
    "DOWNLOAD LIST",
    "CHECK FROM DATE",
    "SEARCH",
    "CHECK CURRENT PRICE",
    "EXIT",
};
int n_choices = sizeof(choices) / sizeof(char *);
void print_menu(WINDOW *menu_win, int highlight);

char user_input[30];
char date[20];
char token[30];
char substring_to_search_for[64];

int get_input(void)
{
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;

    initscr();
    clear();
    noecho();
    cbreak();

////
    
        
    if (has_colors() == FALSE)
    {
        endwin();
        printf("Your terminal does not support color \n");
        exit(1);
    }

    WINDOW *desc_win;
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_WHITE, COLOR_BLACK);

      
    getmaxyx(stdscr, row, col);


    attron(COLOR_PAIR(1));
    mvprintw(4, (col-strlen(check_msg))/2, "%s", check_msg);
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    mvprintw(6, (col-strlen(list_msg))/2, "%s", list_msg);
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(3));
    mvprintw(8, (col-strlen(historic_msg))/2, "%s", historic_msg);
    attroff(COLOR_PAIR(3));

    attron(COLOR_PAIR(4));
    mvprintw(10, (col-strlen(search_msg))/2, "%s", search_msg);
    attroff(COLOR_PAIR(4));

    attron(A_BOLD);
    mvprintw(12, (col-strlen(token_msg))/2, "%s", token_msg);
    attroff(A_BOLD);

    attron(A_UNDERLINE);
    mvprintw(14, (col-strlen(exit_msg))/2, "%s", exit_msg);
    attroff(A_UNDERLINE);


    //getstr(user_input);
    //mvprintw(LINES -2, 0, "You Entered: %s", user_input);
    //refresh();      

    startx = (40 - WIDTH) / 2;
    starty = (20 - HEIGHT) / 2;

    menu_win = newwin(HEIGHT, WIDTH, starty, startx);
    keypad(menu_win, TRUE);
    mvprintw(0, 0, "Use arrow keys to go up and down, Press enter to select a choice");
    refresh();
    print_menu(menu_win, highlight);

    while(1)
        {	c = wgetch(menu_win);
            switch(c)
            {	case KEY_UP:
                    if(highlight == 1)
                        highlight = n_choices;
                    else
                        --highlight;
                    break;
                case KEY_DOWN:
                    if(highlight == n_choices)
                        highlight = 1;
                    else 
                        ++highlight;
                    break;
                case 10:
                    choice = highlight;
                    return choice;
                    break;
                default:
                    mvprintw(24, 0, "Character pressed is = %3d Hopefully it can be printed as '%c'", c, c);
                    refresh();
                    break;
            }
            print_menu(menu_win, highlight);
            if(choice != 0)	/* User did a choice come out of the infinite loop */
                break;
        }	
        mvprintw(23, 0, "You chose choice %d with choice string %s\n", choice, choices[choice - 1]);
        clrtoeol();
        refresh();
        return choice;
}
   
     
   
    // scanf("%s", user_input); //Here, scanf knows it's going to receive a string (char*), that char* is user_input. Then, it's going to modify the received variable with the user input
    // We should probably should convert all input to lowercase, I don't remember how
        /*
    lowercase(user_input);
    return user_input;
    */
/*
   return 0;
    // In C, we can't compare directly two strings, because a string is actually a char*, which is a pointer (the same 2 strings will have different addresses, and thus will never be the same) 
}
*/


char* get_input_date(void)
{
    do 
    {
        printf("\n");
        system("clear");
        printf("Please, enter the date in dd-mm-YYYY format: ");
        scanf("%s", date);
    }
    while ((strlen(date) != 10));

    return date;
}

char* get_input_token(void)
{
    printf("\n");
    system("clear");
    printf("Please, enter the token you are looking for: ");
    scanf("%s", token);
    lowercase(token);

    return token;
}

char* get_input_search(void)
{
    printf("\n");
    system("clear");
    printf("Please, enter the part of the name of the token you are looking for in order to display possible matches on your screen.\nDepending on the OS you are using, the first result displayed might be an echo from your terminal\nIt is recommended you execute 'list' first in order to have an updated working list of tokens: ");
    scanf("%s", substring_to_search_for);
    lowercase(substring_to_search_for);

    printf("\n\n");
    return substring_to_search_for;
}

// I want to create a function that receives a string, converts all of its characters to lower case, and returns the lowercased string
// I think I don't actually need to return the new string, as the original string will be modified (char* is a pointer to the original string)
int lowercase(char* word)
{
    for (int i = 0; i < strlen(word); i++)
    {
        if (word[i] >  64 && word[i] < 91)
        {
            word[i] = word[i] + 32;
        }
    }
    printf("%s", word);
    return 0;
}


int print_title(void)
{
    printf("\n");
    printf("\n");
    printf("*     /$$$$$$$        /$$$$$$$        /$$$$$$        /$$$$$$        /$$$$$$$$\n");
    printf("*    | $$__  $$      | $$__  $$      |_  $$_/       /$$__  $$      | $$_____/\n");
    printf("*    | $$  \\ $$      | $$  \\ $$        | $$        | $$  \\__/      | $$      \n");
    printf("*    | $$$$$$$/      | $$$$$$$/        | $$        | $$            | $$$$$   \n");
    printf("*    | $$____/       | $$__  $$        | $$        | $$            | $$__/   \n");
    printf("*    | $$            | $$  \\ $$        | $$        | $$    $$      | $$      \n");
    printf("*    | $$            | $$  | $$        /$$$$$$     |  $$$$$$/      | $$$$$$$$\n");
    printf("*    |__/            |__/  |__/       |______/       \\______/       |________/\n");
    printf("*                                                                            \n");
}

void print_menu(WINDOW *menu_win, int highlight)
{
	int x, y, i;	

	x = 2;
	y = 2;
	box(menu_win, 0, 0);
	for(i = 0; i < n_choices; ++i)
	{	if(highlight == i + 1) /* High light the present choice */
		{	wattron(menu_win, A_REVERSE); 
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
}
