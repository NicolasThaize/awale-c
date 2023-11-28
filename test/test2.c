#include <ncurses.h>
#include <string.h>
#include <unistd.h>

void print_message(const char* user, const char* message, int row) {
    mvprintw(row, 0, "%s: %s", user, message);
}

int main() {
    // Initialize ncurses
    initscr();
    cbreak(); // Line buffering disabled
    noecho(); // Don't display input characters
    scrollok(stdscr, TRUE); // Enable scrolling

    int row, col;
    getmaxyx(stdscr, row, col);

    char user[20] = "client a";
    char input[100];

    while (1) {
        // Move the cursor to the user input area
        move(row - 1, 0);
        printw("%s: ", user);

        // Allow the user to type
        echo();
        getstr(input);
        noecho();

        // Move the cursor to the penultimate line and print the new message
        move(row - 2, 0);
        print_message(user, input, row - 2);

        // Move the cursor back to the user input area
        move(row - 1, 0);
        clrtoeol(); // Clear the user's input line

        // Scroll the window up
        scroll(stdscr);

        // Refresh the screen
        refresh();

        // Simulate delay
        sleep(1);
    }

    // End ncurses
    endwin();

    return 0;
}
