//// write c++: input n, m, output a filled nxm rectangle table with ABCD letters.
// How to fill: 
// Do it recursively: starting from (0,0) as current point, and initialize upper/left/right as empty.
// Each recurse: 
    // Define the current minimum lexical color (X) as one that's not in upper/left/right colors and is minimum in ABCD letters.
    // If X is A , or X is B and upper is A, or remaining column is 1, do this:
        // If the remaining # of rows is bigger than the remaining # of cols (let's say minimum of the two is W), fill all
        // W by W square from current point as X. Then,
        // go to the next recurse with the remaining not-filled rectangle, and the top-left corner as current point, but with
        // updated remembered upper color as the one you just filled.
        // If the remaining # of rows is smaller than or equal to the remaining # of cols (let's say minimum of the two is W),
        // similarly fill the W by W square using X, but this time pass to the next recurse the right hand side remaining rectangle, with
        // updated remembered left color as the one you just filled.
    // Otherwise,
        // fill the crrent point with X, set Y as the minimum color that's not upper or X, 
        // then fill the right side of current point using a biggest possible square using Y, assuming the biggest square is of size W,
        // then fill the W-1 cells below current point one by one, using left as left, Y as right, and the one above each one as upper to determine the color of each cell.
        // Then, if there is still space on the right side of W-sized-square, recurse from there; otherwise, recurse from the W-tih cell below current ponit
        // , and pass down the correct upper/left next recurse.
// After filling all the cells, output the full rectangle.
#include <iostream>
#include <vector>

using namespace std;

char get_min_color(char upper, char left, char right) {
    for (char c = 'A'; c <= 'D'; ++c) {
        if (c != upper && c != left && c!= right) {
            return c;
        }
    }
    return ' '; // Should not happen
}

void fill_rectangle(vector<vector<char> >& rect, int row_start, int col_start, int row_end, int col_end, char upper_color, char left_color) {
    if (row_start >= row_end || col_start >= col_end) {
        return;
    }

    int rows = row_end - row_start;
    int cols = col_end - col_start;
    
    char fill_color = get_min_color(upper_color, left_color, ' ');
    
    if (fill_color == 'A' || (fill_color == 'B' && upper_color == 'A') || cols == 1) {
        int min_dim = min(rows, cols);
        for (int i = row_start; i < row_start + min_dim; ++i) {
            for (int j = col_start; j < col_start + min_dim; ++j) {
                rect[i][j] = fill_color;
            }
        }

        if (rows > cols) {
            fill_rectangle(rect, row_start + min_dim, col_start, row_end, col_end, fill_color, left_color);
        } else {
            fill_rectangle(rect, row_start, col_start + min_dim, row_end, col_end, upper_color, fill_color);
        }
    } else {
        rect[row_start][col_start] = fill_color;
        char second_color = get_min_color(upper_color, fill_color, ' ');
        int square_size = 1;
        while(col_start + square_size < col_end - 1 && row_start + square_size - 1 < row_end - 1) {
            square_size++;
        }
        
        for(int i = row_start; i < row_start + square_size; ++i) {
            for(int j = col_start + 1; j <= col_start + square_size; ++j) {
                rect[i][j] = second_color;
            }
        }
        
        for(int i = row_start + 1; i < row_start + square_size; ++i) {
            char cell_color = get_min_color(rect[i-1][col_start], left_color, second_color);
            rect[i][col_start] = cell_color;
        }
        
        if(col_start + square_size + 1 < col_end) {
            fill_rectangle(rect, row_start, col_start + square_size + 1, row_end, col_end, upper_color, second_color);
        } else if (row_start + square_size < row_end) {
            fill_rectangle(rect, row_start + square_size, col_start, row_end, col_end, rect[row_start + square_size -1][col_start], left_color);
        }
    }
}

int main() {
    int n, m;
    cin >> n >> m;

    vector< vector<char> > rect(n, vector<char>(m, 0));
    fill_rectangle(rect, 0, 0, n, m, ' ', ' ');

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            cout << rect[i][j];
        }
        cout << endl;
    }

    return 0;
}
