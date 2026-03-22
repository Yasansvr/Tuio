#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<unistd.h>
#include<ncurses.h>
#include<limits.h>
#include<string.h>


typedef struct window
{
    int height;
    int width;
    int start_x;
    int start_y;
}winCor;


int main(){
    initscr();
    if (has_colors() == FALSE)
    {
        printf("your terminal does not support colors.");
    }
    start_color();
    init_pair(1,COLOR_BLACK,COLOR_BLUE);
    init_pair(2,COLOR_BLACK , COLOR_YELLOW);
    


    char cwd[PATH_MAX];//current-dir
    char pwd[PATH_MAX];//parent-dir
    int row , col;
    char *w1_help = " [ use <arrow keys> to move along paths or folders | <q> for quit ]";
    char *w2_help = " [ use <w,a,s,d> Or vim key binding <H,J,K,L >to scroll through file content ]";
    //rows and cols of whole screen ( delete )
    
    int ch;
    int selected_index = 0;
    int total_folders = 0;
    char selected_folder_name[PATH_MAX] = "";
    int selected_item_type = 0;
    int win1_scroll_y = 0;
    int win1_scroll_x = 0;
    bool exit = false;
    keypad(stdscr , TRUE);
    cbreak();
    noecho();
    while (exit == false)
    {
        getmaxyx(stdscr , row ,col); 
        winCor w1 = {row,col/2,0,0};
        winCor w2 = {row,col/2,col/2,0};
    
        WINDOW *win = newwin(w1.height , w1.width , w1.start_y , w1.start_x);
        WINDOW *win1 = newwin(w2.height,w2.width,w2.start_y,w2.start_x);
        refresh();
        
        box(win , 0 , 0);
        box(win1 , 0 , 0);
        
        
        curs_set(0);
        wattron(win, COLOR_PAIR(2));
        mvwprintw(win, 0, 1, " Tui File Explorer ");
        wattroff(win, COLOR_PAIR(2));
        mvwprintw(win, w1.height-1, 1, " <%.*s> ", w1.width - 6, w1_help);
        mvwprintw(win1, w2.height-1, 1, " <%.*s> ", w2.width - 6, w2_help);
        wattron(win1, COLOR_PAIR(2));
        mvwprintw(win1,0,1," <cat observation> ");
        wattroff(win1, COLOR_PAIR(2));

        //printing current location
        if (getcwd(cwd,sizeof(cwd))!= NULL)
        {

            //counting number of folders inside a folder
            int line = 3;
            int j = 2;
            struct dirent **namelist;
            int n;
            int count = 0;

            n = scandir(".", &namelist, NULL, alphasort);
            if (n < 0) {
                perror("scandir");
            } else {
                int max_visible_items = w1.height - 5;
                if (max_visible_items < 1) max_visible_items = 1;
                int start_item = 0;
                if (selected_index >= max_visible_items) {
                    start_item = selected_index - max_visible_items + 1;
                }

                for (int i = 0; i < n; i++) {
                    struct dirent *entry = namelist[i];
                    if (entry->d_type == DT_DIR || entry->d_type == DT_REG) {
                        if (strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0) {
                            free(namelist[i]);
                            continue;
                        }

                        if (count == selected_index) {
                            strcpy(selected_folder_name, entry->d_name);
                            selected_item_type = entry->d_type;
                        }

                        if (count >= start_item && count < start_item + max_visible_items) {
                            if (count == selected_index) {
                                wattron(win, COLOR_PAIR(1));
                            }

                            int max_name_len = w1.width - j - 9;
                            if (max_name_len < 0) max_name_len = 0;
                            if (entry->d_type == DT_DIR) {
                                mvwprintw(win, line++, j, " |--> %.*s", max_name_len, entry->d_name);
                            } else {
                                mvwprintw(win, line++, j, " |--(%.*s)", max_name_len, entry->d_name);
                            }

                            if (count == selected_index) {
                                wattroff(win, COLOR_PAIR(1));
                            }
                        }
                        count++;
                    }
                    free(namelist[i]);
                }
                free(namelist);
            }
            total_folders = count;
            mvwprintw(win,1,1,"current Path ~> %s", cwd);
            mvwprintw(win,2,1,"items count ~> %d", count);

            if (selected_item_type == DT_REG) {
                FILE *f = fopen(selected_folder_name, "r");
                if (f) {
                    char header[512];
                    size_t bytes_read = fread(header, 1, sizeof(header), f);
                    int is_binary = 0;
                    for (size_t i = 0; i < bytes_read; i++) {
                        if (header[i] == '\0') {
                            is_binary = 1;
                            break;
                        }
                    }

                    if (is_binary) {
                        mvwprintw(win1, 1, 1, "NOT READABLE");
                    } else {
                        rewind(f);
                        char buf[1024];
                        int content_line = 2;
                        int current_line = 0;
                        while (fgets(buf, sizeof(buf), f) && content_line < w2.height - 1) {
                            buf[strcspn(buf, "\n")] = 0;
                            if (current_line >= win1_scroll_y) {
                                int len = strlen(buf);
                                if (win1_scroll_x < len) {
                                    mvwprintw(win1, content_line++, 1, "%.*s", w2.width - 2, buf + win1_scroll_x);
                                } else {
                                    content_line++;
                                }
                            }
                            current_line++;
                        }
                    }
                    fclose(f);
                } else {
                    mvwprintw(win1, 1, 1, "Cannot open file");
                }
            }
        }else{
            perror("getcwd() error");
        }
        wrefresh(win);
        wrefresh(win1);

        








        ch = getch();
        if (ch == 'q') {
            exit = true;
        } else if (ch == KEY_UP) {
            if (selected_index > 0) { selected_index--; win1_scroll_y = 0; win1_scroll_x = 0; }
        } else if (ch == KEY_DOWN) {
            if (selected_index < total_folders - 1) { selected_index++; win1_scroll_y = 0; win1_scroll_x = 0; }
        } else if (ch == 'w' || ch == 'k') {
            if (win1_scroll_y > 0) win1_scroll_y = win1_scroll_y - 5;
        } else if (ch == 's' || ch == 'j') {
            win1_scroll_y = win1_scroll_y + 5;
        } else if (ch == 'a' || ch == 'h') {
            if (win1_scroll_x > 0) win1_scroll_x = win1_scroll_x - 1;
        } else if (ch == 'd' || ch == 'l') {
            win1_scroll_x++;
        }
        if(ch == KEY_LEFT){
            strcpy(pwd , cwd);
            char *last_slash = strrchr(pwd , '/');
            if (last_slash != NULL && last_slash != pwd)
            {
                *last_slash = '\0'; // remove last folder
                strcpy(cwd, pwd);
            } else {
                strcpy(cwd, "/"); 
            }
            chdir(cwd);
            selected_index = 0;
            win1_scroll_y = 0;
            win1_scroll_x = 0;
        } else if (ch == '\n' || ch == KEY_ENTER || ch == KEY_RIGHT) {
            if (total_folders > 0 && strlen(selected_folder_name) > 0 && selected_item_type == DT_DIR) {
                chdir(selected_folder_name);
                selected_index = 0;
                win1_scroll_y = 0;
                win1_scroll_x = 0;
            }
        }
        
        
        delwin(win);
        delwin(win1);
    }

    



    //printing size of screen based on lines and chars : mine 52 , 238
    //printw("rows : %d\ncols : %d",row,col); 
    
    endwin();

    
    return 0;
}