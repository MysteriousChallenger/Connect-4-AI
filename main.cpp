#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#define debug_stats true
#define transposition_table true
#define transposition_table_ignore_shallow true

#include "Eval.hpp"
#include "Position.hpp"
#include "TranspositionTable.hpp"

using namespace std;
int main() {
    //444333555222
    Position p = Position("444111777444171717563235");
    //Position p = Position(0,0,0);
    p.print_self();
    p.print(p.gen_non_losing_moves());
    Position positions[7];
    int length2 = p.gen_moves_with_position_arr_and_moves(positions,p.gen_non_losing_moves());
    Evaluator::sort_positions_by_heuristic(positions,length2);
    for(Position p2: positions){
        // p2.print_self();
        // p2.print(Position::MASK.board ^ p2.moves_mask);
        // p2.print(p2.winning_moves(Position::MASK.board ^ p2.moves_mask));
        // p2.print(p2.winning_moves(Position::MASK.board ^ p2.moves_mask, p2.moves_mask ^ p2.active_player));
        cout<<"score: "<<to_string(p2.get_heuristic_score())<<endl;
    }
    // string s;
    // cin >> s;
    // cout << s;
    TranspositionTable::init();
    auto start = chrono::high_resolution_clock::now();
    auto stop = chrono::high_resolution_clock::now();
    auto truestart = start;
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    auto total_duration = duration;
    cout << to_string(Evaluator::negamax(p, 0)) << endl;
    cout << endl;
    for (int i = 0; i < 42; i++) {
        uint64_t positions_explored = Evaluator::node_explored_count;
        start = chrono::high_resolution_clock::now();
        int score = Evaluator::negamax(p, i,-127,127);
        stop = chrono::high_resolution_clock::now();
        cout << to_string(i) << "-score:\t\t" << to_string(score) << endl;
        cout << "heuristic calls:\t\t" << to_string(Position::heuristic_eval_count) << endl;
        cout << "Positions expored:\t\t" << to_string(Evaluator::node_explored_count) << endl;
        cout << "Transpositions stored:\t\t" << to_string(TranspositionTable::set_count) << endl;
        cout << "Transpositions overwritten:\t" << to_string(TranspositionTable::overwrite_count) << endl;
        cout << "Cache-hits:\t\t\t" << to_string(TranspositionTable::cache_hit) << endl;
        cout << "Cache-misses:\t\t\t" << to_string(TranspositionTable::cache_miss) << endl;
        uint64_t fullness = TranspositionTable::count_fullness();
        cout << "Table utilization:\t\t" << to_string(fullness) << " of " << to_string(TranspositionTable::num_entries) << " (" << ((long double)fullness) / TranspositionTable::num_entries * 100 << "%)" << endl;

        duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
        total_duration += duration;
        cout << duration.count() << "(ms)" << endl;
        cout << ((long double)(Evaluator::node_explored_count - positions_explored)) / duration.count() * 1000 << "(nps)" << endl;
        if(score >= 85 || score <= -85){
            break;
        }
    }
    cout << total_duration.count() << "(ms) total" << endl;
    TranspositionTable::init();
    start = chrono::high_resolution_clock::now();
    int score = Evaluator::negamax(p, 42,-127,127);
    stop = chrono::high_resolution_clock::now();
    cout << to_string(42 - p.moves_count) << "-score:\t" << to_string(score) << endl;
    cout << "heuristic calls:\t" << to_string(Position::heuristic_eval_count) << endl;
    cout << "Positions expored:\t" << to_string(Evaluator::node_explored_count) << endl;
    cout << "Transpositions stored:\t" << to_string(TranspositionTable::set_count) << endl;
    cout << "Transpositions overwritten:\t" << to_string(TranspositionTable::overwrite_count) << endl;
    cout << "Cache-hits:\t\t" << to_string(TranspositionTable::cache_hit) << endl;
    cout << "Cache-misses:\t\t" << to_string(TranspositionTable::cache_miss) << endl;
    duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << duration.count() << "(ms)" << endl;

    // cout << "testphaste" << endl;
    // Position arr[30];
    // int8_t length = Evaluator::get_mainline(p, arr, 20);
    // cout << to_string(length) << endl;
    // for (int i = 0; i < length; i++) {
    //     cout << "depth: " << to_string(i) << endl;
    //     arr[i].print_self();
    // }
    cout << "testphaste" << endl;
    TranspositionTable::init();
    Position* arr2 = (Position*)malloc(sizeof(Position) * 15);
    uint8_t length = Evaluator::get_mainline(p,arr2,30);
    cout<<"length: "<<to_string(length)<<endl;
    p.print_self();
    for (int i = 0; i < length; i++) {
        cout << to_string(i) << endl;
        arr2[i].print_self();
    }
    cout << "theend" << endl;

    // // Position p = Position();
    // // string s;
    // // int col;
    // // while (true) {
    // //     cout << endl;
    // //     p.print_self();
    // //     cout << "0123456" << endl;
    // //     cin >> s;
    // //     col = stoi(s);
    // //     p.play(p.gen_moves() & Position::MASK.cols[col]);
    // // }
}