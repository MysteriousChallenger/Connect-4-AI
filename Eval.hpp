#pragma once

#include <cmath>
#include "Position.hpp"
#include "TranspositionTable.hpp"
class Evaluator {
   public:
    static uint64_t node_explored_count;
    union Evaluation_record {
        uint32_t raw;
        struct {
            uint8_t flag; // 0-max, 1-min, 2-exact (@depth), 3-true, finalized falue
            uint8_t depth;
            int8_t score;
        };
    };
    static inline int_fast8_t negamax(Position position, uint_fast8_t depth, int8_t alpha, int8_t beta) {
        int8_t initial_alpha = alpha;
        int8_t value;
        #if transposition_table == true
        Evaluation_record eval;
        #endif
        #if debug_stats == true
        node_explored_count++;
        #endif

        if(position.moves_count == 42){
            return 0;
        }

        // if (position.winning_moves(position.gen_moves()) != 0) {  // any winning position would result in nonzero state
        //     //position.print_self();
        //     // position.print(position.winning_moves(position.gen_moves()));
        //     //position.print(position.gen_dense_rep());
        //     return 85 + position.moves_count;  // int8 max - 2
        // }



        uint64_t moves = position.gen_non_losing_moves();
        if (moves == 0) {
            return -85 - position.moves_count;
        }

        if (depth == 0) {
            return position.get_heuristic_score();
        }

        #if transposition_table == true
        eval.raw = TranspositionTable::get(position.gen_dense_rep());
        if (eval.raw != 0) {
            #if transposition_table_ignore_shallow == true
            if (eval.flag == 3){ // finalized score
                return eval.score;
            }   
            if (eval.depth >= depth) {
                // switch(eval.flag){
                //     case 0: alpha = max(alpha, eval.score); break;// upper bound
                //     case 1: beta = min(beta, eval.score); break; // lower bound
                //     default:
                //         return eval.score;
                // }
                return eval.score;
            } 
            #else
            if (true) {
                if (eval.flag == 0) {  // upper bound
                    alpha = max(alpha, eval.score);
                } else if (eval.flag == 1) {  // lower bound
                    beta = min(beta, eval.score);
                } else if (eval.flag = 3) { // final score
                    return eval.score;
                }
            }
            #endif
            if (alpha >= beta) {
                return eval.score;
            }
        }
        #endif
        
        Position children[Game_Constants::WIDTH];  // one variation per column
        uint_fast8_t length = position.gen_moves_with_position_arr_and_moves(children,moves);
        sort_positions_by_heuristic(children, length);

        value = -85;  // int8 min
        int_fast8_t score = -negamax(children[0],depth-1, -beta, -alpha);
        value = max(value, score);
        alpha = max(alpha, value);
        if ((alpha < beta)) {
            for (uint_fast8_t i = 1; i < length; i++) {
            score = -negamax(children[i], depth - 1, -alpha - 1, -alpha);
            
            if (alpha < score && score < beta){
                score = -negamax(children[i], depth - 1, -beta, -score);
            }
            // for (int j = 0; j < depth - 1; j++) {
            //     cout << "\t";
            // }
            // cout << to_string(score) << " " << to_string(-beta) << " " << to_string(-alpha) << endl;
            // children[i].print_self(depth-1);
            // children[i].print(children[i].gen_moves());

            value = max(value, score);
            alpha = max(alpha, value);
            if (alpha >= beta) {
                break;
            }
        }
        }

        // for (int j = 0; j < depth; j++) {
        //     cout << "\t";
        // }
        // cout << to_string(length) << " children" << endl;
        // position.print_self(depth);
        
        #if transposition_table == true
        eval.score = value;
        eval.depth = depth;
        if (value >= 85 || value <= -85) {
            eval.flag = 3; // true value
            TranspositionTable::set(position.gen_dense_rep(), eval.raw);
        } else if ( value <= initial_alpha) {
            // eval.flag = 0;  // upperbound max
            // TranspositionTable::set(position.gen_dense_rep(), eval.raw);
        } else if (value >= beta ){
            // eval.flag = 1;  // lowerbound min
            // TranspositionTable::set(position.gen_dense_rep(), eval.raw);
        } else {
            eval.flag = 2;  // exact
            TranspositionTable::set(position.gen_dense_rep(), eval.raw);
        }
        #endif
        return value;
        
    }
    static int8_t negamax(Position position, uint_fast8_t depth) {
        return negamax(position, depth, -85, 85);
    }

    inline static const void sort_positions_by_heuristic(Position *positions, uint_fast8_t length) {
        if (length == 1) return;
        int_fast8_t select_index;
        int_fast8_t insert_index;
        Position temp;
        select_index = 1;
        while (select_index < length) {
            temp = positions[select_index];
            insert_index = select_index - 1;
            while (insert_index >= 0 && positions[insert_index].get_heuristic_score() < temp.get_heuristic_score()) {
            //while (insert_index >= 0 && scores[insert_index] < scores[select_index]) {
                positions[insert_index + 1] = positions[insert_index];
                insert_index--;
            }
            positions[insert_index + 1] = temp;
            select_index++;
        }
    }
    static uint8_t get_mainline(Position position, Position *mainline, uint_fast8_t depth) {
        for (int i = 0; i < depth; i++) {
            TranspositionTable::init();
            mainline[i] = get_best_move(position, depth - i);
            position = mainline[i];
            if (position.winning_moves(position.gen_moves()) != 0) {
                Position clone = position;
                clone.play(position.winning_moves(position.gen_moves()));
                mainline[i + 1] = clone;
                return i + 2;
            }
        }
        return depth;
    }
    static Position get_best_move(Position position, uint_fast8_t depth) {
        Position children[7];
        Position best_child = position;
        int8_t max_score, score, i;
        max_score = -127;
        uint_fast8_t length = best_child.gen_non_losing_moves_with_position_arr(children);
        sort_positions_by_heuristic(children, length);
        for (i = 0; i < length; i++) {
            // children[i].print_self();
            
            TranspositionTable::init();
            score = -negamax(children[i], depth);
            // cout<<to_string(score)<<endl;
            if (score > max_score) {
                max_score = score;
                best_child = children[i];
            }
        }
        return best_child;
    }
    static int_fast8_t negamax_with_memory(Position position, uint_fast8_t depth, int_fast8_t alpha, int_fast8_t beta){
        
    }

};
uint64_t Evaluator::node_explored_count = 0;
