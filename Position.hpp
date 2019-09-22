#pragma once

#include <stdint.h>
#include <bitset>
using namespace std;

class Game_Constants {
   public:
    // generate masks for each row,column,position
    using grid_t = uint64_t;     // one grid, used for masks and played positions.
                                 // Bit numbering: 63,62,61...1,0
                                 // Bit order:
                                 // 5 12 19 26 33 40 47
                                 // 4 11 18 25 32 39 46
                                 // 3 10 17 24 31 38 45
                                 // 2  9 16 23 30 37 44
                                 // 1  8 15 22 29 36 43
                                 // 0  7 14 21 28 35 42
    using dense_rep = uint64_t;  // records game state as 1 for p1, 0 for p2, with highest 1 in each collumn indicating first empty space.

    static constexpr int HEIGHT = 6;
    static constexpr int WIDTH = 7;
    static constexpr int STATE_SIZE = (HEIGHT + 1) * WIDTH;
    template <int height, int width>
    struct Masks {
        grid_t rows[height];
        grid_t cols[width];
        grid_t bottom_row;
        grid_t board;
        constexpr Masks() : rows(), cols(), bottom_row(), board() {
            for (int i = 0; i < height; i++) {
                for (int n = 0; n < width; n++) {
                    rows[i] |= 1LL << (n * (height + 1) + i);
                    board |= 1LL << (n * (height + 1) + i);
                };
            };
            for (int i = 0; i < width; i++) {
                cols[i] = (1LL << ((i + 1) * (height + 1))) - (1LL << ((i) * (height + 1)));
            }
            bottom_row = rows[0];
        }
    };
};

class Position : Game_Constants {
   public:
    grid_t active_player;
    grid_t moves_mask;
    uint_fast8_t moves_count;
    static constexpr Masks<HEIGHT, WIDTH> MASK = Masks<HEIGHT, WIDTH>();
    static uint64_t heuristic_eval_count;
    Position(grid_t active_player = 0, grid_t moves_mask = 0, grid_t moves_count = 0 ) : active_player(active_player), moves_mask(moves_mask), moves_count(moves_count){
    }

    Position(string movestr) : active_player(0), moves_mask(0), moves_count(0){
        for(char& c : movestr) {
            // cout << ((int)c)-49 << endl;
            play(gen_moves()&MASK.cols[((int)c)-49]);
            // print(gen_moves());
        }
        cout<<endl;
    }

    inline const grid_t opponent_moves_mask() {
        return active_player ^ moves_mask;
    }
    

    inline void play(grid_t move) {
        // print(move);

        // print(moves_mask);
        active_player ^= moves_mask;
        moves_mask |= move;

        // print(moves_mask);
        moves_count += 1;
    }

    inline const grid_t gen_moves() {
        return (moves_mask + MASK.bottom_row) & MASK.board;  // since bits in a collumn are next to each other, this is (111 + 1) = (1000), getting top position
    }

    inline const void gen_moves_with_grid_arr(grid_t* arr) {
        grid_t moves = gen_moves();
        for (uint_fast8_t i = 0; i < WIDTH; i++) {
            arr[i] = moves & MASK.cols[i];
        }
    }

    inline const uint_fast8_t gen_moves_with_position_arr(Position* arr) {  // requires <Game_Constants::WIDTH> length array
        grid_t moves = gen_moves();
        uint_fast8_t write_index = 0;
        for (uint_fast8_t i = 0; i < WIDTH; i++) {
            if ((moves & MASK.cols[i]) != 0) {
                arr[write_index] = *this;
                // arr[write_index].print_self();
                arr[write_index].play(moves & MASK.cols[i]);
                // arr[write_index].print_self();
                // print(moves & MASK.cols[i]);
                write_index++;
            }
        }
        return write_index;  // length/how many moves were written
    }
    inline const uint_fast8_t gen_non_losing_moves_with_position_arr(Position* arr) {  // requires <Game_Constants::WIDTH> length array
        grid_t moves = gen_non_losing_moves();
        uint_fast8_t write_index = 0;
        for (uint_fast8_t i = 0; i < WIDTH; i++) {
            if ((moves & MASK.cols[i]) != 0) {
                arr[write_index] = *this;
                // arr[write_index].print_self();
                arr[write_index].play(moves & MASK.cols[i]);
                // arr[write_index].print_self();
                // print(moves & MASK.cols[i]);
                write_index++;
            }
        }
        return write_index;  // length/how many moves were written
    }

    inline const uint_fast8_t gen_moves_with_position_arr_and_moves(Position* arr, grid_t moves) {  // requires <Game_Constants::WIDTH> length array
        uint_fast8_t write_index = 0;
        for (uint_fast8_t i = 0; i < WIDTH; i++) {
            if ((moves & MASK.cols[i]) != 0) {
                arr[write_index] = *this;
                // arr[write_index].print_self();
                arr[write_index].play(moves & MASK.cols[i]);
                // arr[write_index].print_self();
                // print(moves & MASK.cols[i]);
                write_index++;
            }
        }
        return write_index;  // length/how many moves were written
    }

    inline const grid_t gen_non_losing_moves() {
        grid_t moves = gen_moves();
        grid_t opponent_wins = winning_moves(moves, moves_mask ^ active_player); // winning moves for opponent if they could play one now
        if (opponent_wins != 0) {
            if ((opponent_wins & (opponent_wins - 1)) != 0){
                return 0;
            }else{
                return opponent_wins;
            }
        } else {
            return moves;
        }
    }

    static inline const grid_t winning_moves(grid_t moves, grid_t active_player) {
        // checks via shift+mask

        // left and right along the digits
        grid_t left_wins;
        grid_t right_wins;
        grid_t wins;
        //horizontal (shift by HEIGHT+1)
        left_wins = moves & (active_player << HEIGHT + 1) & (active_player << 2 * (HEIGHT + 1));
        left_wins = left_wins & (((active_player << 3 * (HEIGHT + 1)) | (active_player >> HEIGHT + 1)));
        right_wins = moves & (active_player >> HEIGHT + 1) & (active_player >> 2 * (HEIGHT + 1));
        right_wins = right_wins & (((active_player >> 3 * (HEIGHT + 1)) | (active_player << HEIGHT + 1)));
        wins = left_wins | right_wins;
        // cout<<"horizontal"<<endl;
        // print(left_wins|right_wins);
        //vertical (shift by 1)
        left_wins = moves & (active_player << 1) & (active_player << 2 * (1)) & (active_player << 3 * (1));
        wins |= left_wins;
        // cout<<"vertical"<<endl;
        // print(left_wins|right_wins);
        // print(moves);
        // print(active_player);
        // print(active_player << 1);
        // print(active_player << 2);
        // print(active_player << 3);
        // print(active_player&(active_player << 1))
        // cout<<"endv"<<endl;
        //diagonal "/" (shift by HEIGHT+2)
        left_wins = moves & (active_player << HEIGHT + 2) & (active_player << 2 * (HEIGHT + 2));
        left_wins = left_wins & (((active_player << 3 * (HEIGHT + 2)) | (active_player >> HEIGHT + 2)));
        right_wins = moves & (active_player >> HEIGHT + 2) & (active_player >> 2 * (HEIGHT + 2));
        right_wins = right_wins & (((active_player >> 3 * (HEIGHT + 2)) | (active_player << HEIGHT + 2)));
        wins |= left_wins | right_wins;
        // cout<<"/diagonal"<<endl;
        // print(left_wins|right_wins);
        //diagonal "\" (shift by HEIGHT)
        left_wins = moves & (active_player << HEIGHT) & (active_player << 2 * (HEIGHT));
        left_wins = left_wins & (((active_player << 3 * (HEIGHT)) | (active_player >> HEIGHT)));
        right_wins = moves & (active_player >> HEIGHT) & (active_player >> 2 * (HEIGHT));
        right_wins = right_wins & (((active_player >> 3 * (HEIGHT)) | (active_player << HEIGHT)));
        wins |= left_wins | right_wins;
        // cout<<"\\diagonal"<<endl;
        // print(left_wins|right_wins);
        return wins;
    }

    inline const grid_t winning_moves(grid_t moves) {
        return winning_moves(moves, this->active_player);
    }

    inline const dense_rep gen_dense_rep() {  // records game state as 1 for p1, 0 for p2, with highest 1 in each collumn indicating first empty space. (uses extra row on top if no space is left)
        return (moves_mask + MASK.bottom_row) | active_player;
    }

    inline int8_t get_heuristic_score() {
        #if debug_stats == true
        heuristic_eval_count++;
        #endif
        //return count_ones(winning_moves(MASK.board ^ moves_mask, moves_mask ^ active_player));
        return -count_ones(winning_moves(MASK.board ^ moves_mask)) + count_ones(winning_moves(MASK.board ^ moves_mask, moves_mask ^ active_player));
    }

    static inline const void print(grid_t grid) {
        cout << to_string(grid) << endl;
        for (int i = HEIGHT - 1; i >= 0; i--) {
            for (int n = 0; n < WIDTH; n++) {
                cout << (grid >> n * (HEIGHT + 1) + i) % 2;
            }
            cout << endl;
        }
    }

    inline const void print_self() {
        cout << to_string(active_player) << endl;
        cout << to_string(moves_mask) << endl;
        cout << to_string(moves_count) << endl;
        for (int i = HEIGHT - 1; i >= 0; i--) {
            for (int n = 0; n < WIDTH; n++) {
                if ((moves_mask >> n * (HEIGHT + 1) + i) % 2 == 0) {
                    cout << ".";
                } else {
                    if ((active_player >> n * (HEIGHT + 1) + i) % 2 == moves_count % 2) {
                        cout << "O";
                    } else {
                        cout << "X";
                    }
                }
            }
            cout << endl;
        }
    }
    inline const void print_self(int indentation) {
        cout << string(indentation, '\t') << to_string(active_player) << endl;
        cout << string(indentation, '\t') << to_string(moves_mask) << endl;
        cout << string(indentation, '\t') << to_string(moves_count) << endl;
        for (int i = HEIGHT - 1; i >= 0; i--) {
            cout << string(indentation, '\t');
            for (int n = 0; n < WIDTH; n++) {
                if ((moves_mask >> n * (HEIGHT + 1) + i) % 2 == 0) {
                    cout << ".";
                } else {
                    if ((active_player >> n * (HEIGHT + 1) + i) % 2 == moves_count % 2) {
                        cout << "O";
                    } else {
                        cout << "X";
                    }
                }
            }
            cout << endl;
        }
    }

    static inline int count_ones(grid_t grid) {
        int c = 0;
        for (c = 0; grid != 0; c++) {
            grid &= grid - 1;
        }
        return c;
    }
};
constexpr Game_Constants::Masks<Game_Constants::HEIGHT, Game_Constants::WIDTH> Position::MASK;
uint64_t Position::heuristic_eval_count = 0;