#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <ctype.h>
#include <ncurses.h>

#include "helpers.h"

int check_api_status(void);
int fetch_data(char* token);
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
int historic_price(void);
int list_of_coins(void);
int search(void);

int get_input(void);
int lowercase(char* word);
char* get_input_date(void);
char* get_input_token(void);
char* get_input_search(void);
int print_title(void);

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


struct MemoryStruct {
    char* memory;
    size_t size;
};

int row, col;

int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        printf("You can execute the program without command line arguments, or with a single one: the name of the cryptocurrency you are looking for\n");
        return 1;
    }

    print_title();
    //sleep(2);

    if (argc == 2)
    {
    //    fetch_data(argv[1]);
    }

    while(1)
    {
        start:
        int user_input;
        user_input = get_input();
        if (user_input == 1)
        {
            check_api_status();
            goto start;
        }
        if (user_input == 2)
        {
            list_of_coins();
            sleep(1);
            goto start;
        }
        if (user_input == 3)
        {
            historic_price();
            goto start;
        }
    }
    return 0;
}


int check_api_status(void)
{
    //I need to ping the api
    char *ping_api_url = "https://api.coingecko.com/api/v3/ping";
    // But first I need to initialize libcurl (aka create an easy handle)
    CURL *curl = curl_easy_init();
    CURLcode response;
    //FILE *fp;
    //FILE *fp2;
    //fp = fopen("body.txt", "w");
    //fp2 = fopen("header.txt", "w");
    
    if (curl)
    {
        // Before doing anything further, we need to set the options in the handle we created to control the upcoming transfer (minimum set the target URL)
        curl_easy_setopt(curl, CURLOPT_URL, ping_api_url);
        // I think here I specify the callback function I want to use to write the data received
        //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
        // And I think like this I set up the pointer I want to use to store the data in memory?
        //curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        // I think here I set the handle up to operate with the header data
        //curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, fwrite);
        //curl_easy_setopt(curl, CURLOPT_HEADERDATA, fp2);
        //Once the handle configuration is done, we need to fire off the actual transfer
        

        response = curl_easy_perform(curl);

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code == 200 && response != CURLE_ABORTED_BY_CALLBACK)
        {
            printf("\n");
            system("clear"); // system("clear") clears the terminal
            getmaxyx(stdscr, row, col);
            mvprintw(row / 2, col / 2, "%s", "THE API IS OK");
            refresh();
            sleep(1);
        }
        else
        {
            system("clear");
            getmaxyx(stdscr, row, col);
            mvprintw(row / 2, col/ 2, "%s", "THE API IS NOT OK");
            refresh();
            sleep(1);
        }


        // We are doing our curl to the ping site and we handle a possible error != CURLE_OK
        if (response != CURLE_OK)
        {
            printw("transfer fail: %s\n", curl_easy_strerror(response));
            //fclose(fp);
            //fclose(fp2);
            return 1;
        }
    }

    // I have now the function writing the body of the reply into "file.txt". However, I think I need the headers with the HTTP data,
    // in order to check whether the request is ok or not, although I'm thinking that I already have the response != CURLE_OK, so if that's not triggered
    // we can assume everything went fine. The headers are being stored now in "header.txt", even if I'm not sure I need it right now.
    //printf("%i", response);
    curl_easy_cleanup(curl);
    //fclose(fp);
    //fclose(fp2);
    return 0;
}


int list_of_coins(void)
{
    CURL *handle = curl_easy_init();
    CURLcode response;
    FILE * file;
    file = fopen("list.txt", "w");
    if (file == NULL)
    {
        system("clear");
        getmaxyx(stdscr, row, col);
        mvprintw(row / 2, col/ 2, "%s", "Error while opening file");
        refresh();
        return 1;
    }
    
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    char *url;
    url = "https://api.coingecko.com/api/v3/coins/list";

    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    
    response = curl_easy_perform(handle);
    
    if (response != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(response));
        free(chunk.memory);
        return 1;
    }
    long http_code = 0;
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200 || response == CURLE_ABORTED_BY_CALLBACK)
    {
        system("clear"); // system("clear") clears the terminal
        getmaxyx(stdscr, row, col);
        mvprintw(row / 2, col / 4, "%s", "Not possible to access the list at this time. Please try again later.");
        refresh();
        sleep(3);
    }  
    else
    {
        // Now, our chunk.memory points to a memory block that is chunk.size bytes big and contains the remote file.
        // Do something nice with it
        // chunk.size gives us the length / size? of the bytes stored in memory
        // I'm going to keep the storage in memory, and then printed it formatted into 'list.txt'
        // I can use matches to track and print the total amount of coins available at the moment
        int matches = 0;
        int str_length = 0;
        int end_of_line = 0;
        for (int i = 0; i < chunk.size; i++)
        {
            if (chunk.memory[i] == 58)
            {             
                if (chunk.memory[i - 1] == 34)
                { 
                    if (chunk.memory[i - 2] == 100)
                    {
                        if (chunk.memory[i - 3] == 105)
                        {
                            if (chunk.memory[i - 4] == 34)
                            {   
                                for (int k = i + 2; k < (i + 2 + 20); k++)
                                {
                                    if (chunk.memory[k] == 34)
                                    {
                                        // end_of_line will be set to the index of the last delimiter, helping me isolate the substring I need (id of token)
                                        end_of_line = k;
                                        // str_length will be set to the length of the substring I need (so I can iterate and copy into the variable 
                                        // the characters I need)
                                        str_length = (end_of_line - (i + 2));
                                        matches = matches + 1;
                                        //printf("The length of this string is  %i\n", str_length);
                                        break;
                                    }
                                }

                                
                                //printf("This is the end of the line %i\n", end_of_line);
                                //printf("This is the string length: %i\n", str_length);
                                char *str_print = malloc(sizeof(char) * str_length + 1);
                                for (int l = 0; l < str_length; l++)
                                {
                                    
                                    str_print[l] = chunk.memory[i + 2 + l];
                                    if (l == str_length)
                                    {
                                        break;
                                    }
                                    if (chunk.memory[i + 2 + l] == 34 || chunk.memory[i + 2 + l] == 44)
                                    {
                                        break;
                                    }                                   
                                }
                                // I can't find why for some lines it prints additional characters, str_length is correct (maybe garbage values?), so I will simply print
                                // char by char. This gets the desired output.
                                for (int b = 0; b < str_length; b++)
                                {
                                    //printf("%c", str_print[b]);
                                    fprintf(file, "%c", str_print[b]);
                                }
                                //printf("\n");
                                fprintf(file, "\n");

                                free(str_print);
                            }
                        }
                    }
                }
            }
            
            //printf("%s\n", str_print);
            
        }
        fprintf(file, "%i unique tokens recorded\n", matches);
        
        getmaxyx(stdscr, row, col);
        system("clear");
        mvprintw(row / 2, col / 4, "%s", "list.txt download successful, check the main folder where the price application is located");
        refresh();
        sleep(3);
    }
    
    curl_easy_cleanup(handle);
    free(chunk.memory);    
    curl_global_cleanup();
    fclose(file);
    
    return 0;
}


int historic_price(void)
{
    curl_global_init(CURL_GLOBAL_ALL);

    CURL *handle = curl_easy_init();
    CURLcode response;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    char url[200];

    char *token;
    token = get_input_token();

    char *date;
    date = get_input_date();


    sprintf(url, "https://api.coingecko.com/api/v3/coins/%s/history?date=%s&localization=false", token, date);

    
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    
    response = curl_easy_perform(handle);

   
    
    if (response != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(response));
        free(chunk.memory);
        curl_easy_cleanup(handle);
        return 1;
    }
    
    long http_code = 0;
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &http_code);

    if (http_code != 200 || response == CURLE_ABORTED_BY_CALLBACK)
    {
        system("clear"); // system("clear") clears the terminal
        getmaxyx(stdscr, row, col);
        system("clear");
        mvprintw(row / 2, col / 4, "%s", "THAT TOKEN DOESN'T EXIST OR THERE ARE NO VALUES FOR THAT SPECIFIC DATE, PLEASE TRY AGAIN");
    }  
    else
    {
        // Now, our chunk.memory points to a memory block that is chunk.size bytes big and contains the remote file.
        // Do something nice with it
        char *buffer = malloc(chunk.size);

        if (buffer == NULL)
        {
            getmaxyx(stdscr, row, col);
            system("clear");
            mvprintw(row / 2, col / 2, "%s", "Pointer error");
            sleep(3);
            return 1;
        }
        strcpy(buffer, chunk.memory);

        //The value I need (the price) is almost right after "usd", which I will use as a target for strstr(), that will find that substring inside of a bigger string.
        char target[6] = "usd";

        char *find = strstr(buffer, target);
        if (find == NULL)
        {
            system("clear"); // system("clear") clears the terminal
            getmaxyx(stdscr, row, col);
            mvprintw(row / 2, col / 4, "%s", "THAT TOKEN DOESN'T EXIST OR THERE ARE NO VALUES FOR THAT SPECIFIC DATE, PLEASE TRY AGAIN");
            sleep(3);
            return 1;
        }
        char price[20];

        int k = 0;

        for (int i = 0; i < strlen(find); i++)
        {
            if (find[i] == 44)
            {
                break;
            }
            if (find[i] > 47 && find[i] < 58 || find[i] == 46)
            {
                price[k] = find[i];
                k = k + 1;
            }
            price[k] = '\0';
        }
        double price_ = atof(price);           
        //printf("%f\n", price_);
        
        system("clear");                    
        getmaxyx(stdscr, row, col);
        mvprintw(row / 2, col / 4, "The price of %s, on the date %s, was $%.4f\n", token, date, price_);
        free(buffer);
    }
    

    curl_easy_cleanup(handle);
    free(chunk.memory);    
    curl_global_cleanup();

    return 0;

}


static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}


///HELPERS
/// HELPERS
/////////////////



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
        clear();
        getmaxyx(stdscr, row, col);
        mvprintw(row / 2, col / 4, "Please, enter the date in dd-mm-YYYY format: ");
        getstr(date);
        clear();
               

        mvprintw(row / 2 + 2, col / 4, "You have entered %s", date);
        refresh();
        sleep(1);

    }
    while ((strlen(date) != 10));

    return date;
}

char* get_input_token(void)
{

    clear();
    getmaxyx(stdscr, row, col);
    mvprintw(row / 2, col / 4, "%s", "Please, enter the token you are looking for: ");
    refresh();
    
    getstr(token);
    clear();
    mvprintw(row / 2, col / 4, "You have entered %s", token);
    refresh();
    sleep(2);
    lowercase(token);

    return token;
}

    /*
    char* get_input_token(void)
{
    printf("\n");
    system("clear");
    printf("Please, enter the token you are looking for: ");
    scanf("%s", token);
    lowercase(token);

    return token;
}
    */

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