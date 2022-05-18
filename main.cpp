#include <algorithm>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <string>
#include <locale>
#include <clocale>
#include <fstream>
#include <sstream>
#include <wchar.h>
#include <random>
#include <string_view>
#include <cstring>
#include <queue>



#define char string

#define you_can_edit_this
#define size_t unsigned long long

using namespace std;

struct pair_hash {
    template <class T1, class T2>
    size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        return h1 ^ h2;
    }
};


unordered_map<string, int> char_to_int;

unordered_map<int, pair<int, int>> int_to_coordinate;
unordered_map<pair<int, int>, int, pair_hash> coordinate_to_int;
unordered_map<char, int> eng_to_int;


bool Is_Russian_Letter(int c) {
    return (c >= 1 && c <= 33);
}

double my_generate_random(double from, double to) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(from, to);
    double random_double = dis(gen);
    return random_double;
}

int find_symmetric_vertex(int vertex) {
    pair<int, int> coordinate = int_to_coordinate[vertex];
    coordinate.first *= -1;
    auto search = coordinate_to_int.find(coordinate);
    if (search != coordinate_to_int.end()) {
        return coordinate_to_int[coordinate];
    } else {
        return -1;
    }

}


class Rat;

class Maze;
class Discrete_Operator {
public:
    double prob = 0;

    virtual void apply(Maze &maze, Rat &rat) {
        cerr << "Error: tried to apply general discrete operator\n";
        exit(1);
    }

    virtual void set_probability(double probability) {
        cerr << "Error: tried to set_probabilities for general discrete operator\n";
        exit(1);
    }

    virtual string get_name() {
        return "general";
    }
};

class Operator_Random : public Discrete_Operator {
public:
    void apply(Maze &maze, Rat &rat) override; // Написано ниже
    void set_probability(double probability) override; // Написано ниже
    string get_name() override {
        return "random";
    }
};

class Operator_Inversion : public Discrete_Operator {
public:
    int max_len_of_path_a_rat_can_invert = 5;
    void apply(Maze &maze, Rat &rate) override; // Написано ниже
    void set_probability(double probability) override; // Написано ниже
    string get_name() override {
        return "inversion";
    }
};

class Operator_Symmetry : public Discrete_Operator {
public:
    void apply(Maze &maze, Rat &rate) override; // Написано ниже
    void set_probability(double probability) override; // Написано ниже
    string get_name() override {
        return "symmetry";
    }
};

class Operator_Compression : public Discrete_Operator {
public:
    void apply(Maze &maze, Rat &rate) override; // Написано ниже
    void set_probability(double probability) override; // Написано ниже
    string get_name() override {
        return "compression";
    }
};

double approximately_one = 0.9; // Для выведения медиан
double eps = 0.001; // Для подтверждения точности вычисления в double

double INF = exp(100);

enum class abs_direction {
    left, right, up, down, stop, start
};

enum class relative_direction {
    to_left, to_right, forward, backwards
};


enum class operator_types {
    Discrete_Operator, Operator_Random, Operator_Inversion, Operator_Symmetry, Operator_Compression
};


int n;
unordered_map<int, string> int_to_char;
unordered_map<int, string> int_to_eng;


class Cell {
public:
    int num = 0;
    unordered_map<abs_direction, int> neighs;
};

abs_direction determine_new_direction(abs_direction cur_direction, relative_direction choice) {
    if (choice == relative_direction::forward) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::left;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::right;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::up;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::down;
        }
    }
    if (choice == relative_direction::backwards) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::right;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::left;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::down;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::up;
        }
    }
    if (choice == relative_direction::to_left) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::down;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::left;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::up;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::right;
        }
    }
    if (choice == relative_direction::to_right) {
        if (cur_direction == abs_direction::left) {
            return abs_direction::up;
        }
        if (cur_direction == abs_direction::up) {
            return abs_direction::right;
        }
        if (cur_direction == abs_direction::right) {
            return abs_direction::down;
        }
        if (cur_direction == abs_direction::down) {
            return abs_direction::left;
        }
    }
    cerr << "Error #2 from determine_new_direction!!!\n";
    exit(1);
} // OK

class Rat {
public:
    int cur_cell_int = 0;
    abs_direction cur_direction;

    abs_direction which_feeder_we_start_from = abs_direction::start;

    unordered_map<relative_direction, double> probabilities_of_relative_dirs;
    unordered_map<abs_direction, double> cur_probabilities_of_abs_dirs;

    void set_probabilities_of_relative_dirs(double pr_forward, double pr_right, double pr_left, double pr_back) {
        if (abs(pr_forward + pr_right + pr_left + pr_back - 1) > eps) {
            cerr << "Error! Incorrect probabilities for a rat!\n";
            exit(1);
        }
        probabilities_of_relative_dirs[relative_direction::to_right] = pr_right;
        probabilities_of_relative_dirs[relative_direction::to_left] = pr_left;
        probabilities_of_relative_dirs[relative_direction::forward] = pr_forward;
        probabilities_of_relative_dirs[relative_direction::backwards] = pr_back;
    }

    bool completed_route() {
        return (cur_cell_int == char_to_int["Р"]) || (cur_cell_int == char_to_int["Т"]);

    }

    abs_direction make_decision() {
        {
            double check_sum = 0;
            for (auto &pair_direction_double: cur_probabilities_of_abs_dirs) {
                double cur_prob = pair_direction_double.second;
                if (cur_prob < 0.0) {
                    cerr << "Error! Negative probability in make_decision!\n";
                    exit(1);
                }
                check_sum += cur_prob;
            }
            if (abs(check_sum - 1) > eps) {
                cerr << "Error! Wrong sum of current probabilities in make_decision!\n";
                exit(1);
            }
        }

        // Link: https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
        double random_from_0_to_1 = my_generate_random(0.0, 1.0);
        double cur_sum = 0.0;
        for (auto &pair_direction_double: cur_probabilities_of_abs_dirs) {
            abs_direction dir = pair_direction_double.first;
            double prob = pair_direction_double.second;
            if (prob != 0.0) {
                cur_sum += prob;
                if (cur_sum + eps > random_from_0_to_1) {
                    return dir;
                }
            }
        }
        cerr << "Error in make_decision. NO direction had been chosen\n";
        exit(1);
    }
};

class Maze {
public:
    vector<Cell> cells;
    vector<int> cur_generated_route;
    vector<vector<int>> vector_of_generated_routes;

    vector<string> vector_of_real_routes;
    map<string, vector<vector<string>>> rats_name_to_experiment_results;
#ifdef you_can_edit_this
    Discrete_Operator discrete_operator;
    Operator_Random operator_random;
    Operator_Inversion operator_inversion;
    Operator_Symmetry operator_symmetry;
    Operator_Compression operator_compression;

#endif
    unordered_map<string, Discrete_Operator *> map_from_name_to_operator;
    unordered_map<string, double> operators_to_probabilities;

    bool flag_has_to_stop = false;

    bool route_from_input_is_correct(std::string_view s) {
        return ((s.back() == "Р"[0]) || (s.back() == "Т"[0])) && (s.front() == "О"[0]);
    }

    size_t get_number_of_mistakes_in_route(string_view route) {
        unordered_map<char, size_t> from_char_to_counter_of_visits;
        size_t ans = 0;
        for (auto c: route) {
            string cell_char(1, c);
            ++from_char_to_counter_of_visits[cell_char];
            if (from_char_to_counter_of_visits[cell_char] > 1) {
                ++ans;
            }
        }
        return ans;
    }

    Maze() {
        int_to_char[0] = "_";
        int_to_char[1] = "А";
        int_to_char[2] = "Б";
        int_to_char[3] = "В";
        int_to_char[4] = "Г";
        int_to_char[5] = "Д";
        int_to_char[6] = "Е";
        int_to_char[7] = "Ё";
        int_to_char[8] = "Ж";
        int_to_char[9] = "З";
        int_to_char[10] = "И";
        int_to_char[11] = "Й";
        int_to_char[12] = "К";
        int_to_char[13] = "Л";
        int_to_char[14] = "М";
        int_to_char[15] = "Н";
        int_to_char[16] = "О";
        int_to_char[17] = "П";
        int_to_char[18] = "Р";
        int_to_char[19] = "С";
        int_to_char[20] = "Т";
        int_to_char[21] = "У";
        int_to_char[22] = "Ф";
        int_to_char[23] = "Х";
        int_to_char[24] = "Ц";
        int_to_char[25] = "Ч";
        int_to_char[26] = "Ш";
        int_to_char[27] = "Щ";
        int_to_char[28] = "Ъ";
        int_to_char[29] = "Ы";
        int_to_char[30] = "Ь";
        int_to_char[31] = "Э";
        int_to_char[32] = "Ю";
        int_to_char[33] = "Я";
        int_to_char[34] = "R";
        int_to_char[35] = "S";
        int_to_char[36] = "W";
        int_to_char[37] = "Z";
        int_to_char[38] = "D";
        int_to_char[39] = "F";
        int_to_char[40] = "Q";
        int_to_char[41] = "V";
        int_to_char[42] = "U";
        int_to_char[43] = "I";
        int_to_char[44] = "J";
        int_to_char[45] = "G";

        eng_to_int["A"] = 1;
        eng_to_int["B"] = 2;
        eng_to_int["C"] = 3;
        eng_to_int["D"] = 4;
        eng_to_int["E"] = 5;
        eng_to_int["F"] = 6;
        eng_to_int["G"] = 8;
        eng_to_int["H"] = 9;
        eng_to_int["I"] = 10;
        eng_to_int["J"] = 12;
        eng_to_int["K"] = 13;
        eng_to_int["L"] = 14;
        eng_to_int["M"] = 16;
        eng_to_int["N"] = 18;
        eng_to_int["O"] = 19;
        eng_to_int["P"] = 20;
        eng_to_int["Q"] = 21;
        eng_to_int["R"] = 22;
        eng_to_int["S"] = 23;
        eng_to_int["T"] = 24;
        eng_to_int["U"] = 26;
        eng_to_int["V"] = 27;
        eng_to_int["W"] = 29;
        eng_to_int["X"] = 31;
        eng_to_int["Y"] = 33;

        int_to_eng[1] = "A";
        int_to_eng[2] = "B";
        int_to_eng[3] = "C";
        int_to_eng[4] = "D";
        int_to_eng[5] = "E";
        int_to_eng[6] = "F";
        int_to_eng[8] = "G";
        int_to_eng[9] = "H";
        int_to_eng[10] = "I";
        int_to_eng[12] = "J";
        int_to_eng[13] = "K";
        int_to_eng[14] = "L";
        int_to_eng[16] = "M";
        int_to_eng[18] = "N";
        int_to_eng[19] = "O";
        int_to_eng[20] = "P";
        int_to_eng[21] = "Q";
        int_to_eng[22] = "R";
        int_to_eng[23] = "S";
        int_to_eng[24] = "T";
        int_to_eng[26] = "U";
        int_to_eng[27] = "V";
        int_to_eng[29] = "W";
        int_to_eng[31] = "X";
        int_to_eng[33] = "Y";




        int_to_coordinate[34] = {0, 0};
        int_to_coordinate[16] = {0, 1};
        int_to_coordinate[29] = {0, 2};
        int_to_coordinate[22] = {0, -1};
        int_to_coordinate[41] = {0, -2};

        int_to_coordinate[10] = {-1, 0};
        int_to_coordinate[23] = {-1, -2};

        int_to_coordinate[36] = {-2, 0};
        int_to_coordinate[13] = {-2, 1};
        int_to_coordinate[37] = {-2, 2};
        int_to_coordinate[2] = {-2, 3};
        int_to_coordinate[40] = {-2, -2}; // это d??
        int_to_coordinate[1] = {-2, -3};
        int_to_coordinate[12] = {-2, -1};

        int_to_coordinate[6] = {-3, 2};
        int_to_coordinate[5] = {-3, -2};

        int_to_coordinate[38] = {-4, 2};
        int_to_coordinate[18] = {-4, 3};
        int_to_coordinate[26] = {-4, 1};
        int_to_coordinate[19] = {-4, -1};
        int_to_coordinate[39] = {-4, -2};

        int_to_coordinate[14] = {1, 0};
        int_to_coordinate[24] = {1, -2};

        int_to_coordinate[35] = {2, 0};
        int_to_coordinate[31] = {2, 1};
        int_to_coordinate[45] = {2, 2};
        int_to_coordinate[3] = {2, 3};
        int_to_coordinate[33] = {2, -1};
        int_to_coordinate[42] = {2, -2};
        int_to_coordinate[4] = {2, -3};

        int_to_coordinate[8] = {3, 2};
        int_to_coordinate[9] = {3, -2};

        int_to_coordinate[21] = {4, -1};
        int_to_coordinate[27] = {4, 1};
        int_to_coordinate[44] = {4, 2};
        int_to_coordinate[20] = {4, 3};
        int_to_coordinate[43] = {4, -2};




        n = int_to_char.size();
        {
            for (size_t i = 0; i < int_to_char.size(); ++i) {
                char_to_int[int_to_char[i]] = i;
            }
        }
        cells.resize(n);
        {
            for (size_t i = 0; i < int_to_coordinate.size(); ++i) {
                coordinate_to_int[int_to_coordinate[i]] = i;
            }
        }

    }

    void construct(istream &input, ostream &output) {
        string line;
        while (getline(input, line)) {
            stringstream ss(line);
            char cur_cell_char, left_neigh_char, up_neigh_char, right_neigh_char, down_neigh_char;
            ss >> cur_cell_char >> left_neigh_char >> up_neigh_char >> right_neigh_char >> down_neigh_char;
            int cur_cell_int = char_to_int[cur_cell_char];
            int left_neigh_int = char_to_int[left_neigh_char];
            int up_neigh_int = char_to_int[up_neigh_char];
            int right_neigh_int = char_to_int[right_neigh_char];
            int down_neigh_int = char_to_int[down_neigh_char];

            add_neighs_to_cell(cur_cell_int, left_neigh_int, up_neigh_int, right_neigh_int, down_neigh_int);
        }
    }

    void add_neighs_to_cell(int cur_int, int left_int, int up_int, int right_int, int down_int) {
        cells[cur_int].num = cur_int;
        cells[cur_int].neighs[abs_direction::left] = left_int;
        cells[cur_int].neighs[abs_direction::up] = up_int;
        cells[cur_int].neighs[abs_direction::right] = right_int;
        cells[cur_int].neighs[abs_direction::down] = down_int;
    }


    void print_info_about_cells_in_letters(ostream &output) {
        for (size_t i = 1; i < n; ++i) {
            output << "Клетка " << int_to_char[i] << ' ';
            if (cells[i].num == 0) {
                output << "не существует";
            } else {
                unordered_map<abs_direction, int> &neighs = cells[i].neighs;
                output << i << " имеет соседей " << int_to_char[neighs[abs_direction::left]] << ' '
                       << int_to_char[neighs[abs_direction::up]]
                       << ' ' << int_to_char[neighs[abs_direction::right]] << ' '
                       << int_to_char[neighs[abs_direction::down]];
            }
            output << '\n';
        }
    } // OK

    vector<int> direct(int from, int to) { // BFS для поиска кратчайшего расстояния между вершинами
        vector<int> ans;
        vector<int> len(49, 50);
        vector<int> parent(49, -1);
        len[from] = 0;
        queue<int> q;
        q.push(from);
        while (!q.empty()) {
            int cur = q.front();
            q.pop();
            if (cur == to) {
                //if (len[to] != 0) {
                for (int i = to; i != -1; i = parent[i]) {
                    ans.push_back(i);
                }
                reverse(ans.begin(), ans.end());
                return ans;
                //}
            }
            unordered_map<abs_direction, int> &neighs = cells[cur].neighs;

            if (neighs[abs_direction::left] != 0 && len[neighs[abs_direction::left]] == 49 + 1) {
                len[neighs[abs_direction::left]] = len[cur] + 1;
                q.push(neighs[abs_direction::left]);
                parent[neighs[abs_direction::left]] = cur;

            }

            if (neighs[abs_direction::up] != 0 && len[neighs[abs_direction::up]] == 49 + 1) {
                len[neighs[abs_direction::up]] = len[cur] + 1;
                q.push(neighs[abs_direction::up]);
                parent[neighs[abs_direction::up]] = cur;

            }
            if (neighs[abs_direction::right] != 0 && len[neighs[abs_direction::right]] == 49 + 1) {
                len[neighs[abs_direction::right]] = len[cur] + 1;
                q.push(neighs[abs_direction::right]);
                parent[neighs[abs_direction::right]] = cur;

            }
            if (neighs[abs_direction::down] != 0 && len[neighs[abs_direction::down]] == 49 + 1) {
                len[neighs[abs_direction::down]] = len[cur] + 1;
                q.push(neighs[abs_direction::down]);
                parent[neighs[abs_direction::down]] = cur;

            }
        }
        if (len[to] == 49 + 1) {
            return {-1};
        } else {
            //if (len[to] != 0) {
            for (int i = to; i != -1; i = parent[i]) {
                ans.push_back(i);
            }
            reverse(ans.begin(), ans.end());
            return ans;
            //}
        }
    }

    string choose_an_operator() {
        {
            // Проверка того, что данные корректные
            double sum_of_probs = 0;
            for (auto &pair_string_double: operators_to_probabilities) {
                double el = pair_string_double.second;
                if (el < 0) {
                    cerr << "Error in choose_an_operator. There is a negative probability!\n";
                    exit(1);
                }
                sum_of_probs += el;
            }
            if (abs(1 - sum_of_probs) > eps) {
                cerr << "Error in choose_an_operator. Sum of probabilities is not equal to 1.0!\n";
                exit(1);
            }
        }
        double random_from_0_to_1 = my_generate_random(0, 1.0);
        double cur_sum = 0.0;
        for (auto &pair_string_double: operators_to_probabilities) {
            string name_of_operator = pair_string_double.first;
            double prob = pair_string_double.second;
            if (prob != 0.0) {
                cur_sum += prob;
                if (cur_sum + eps > random_from_0_to_1) {
                    return name_of_operator;
                }
            }
        }
        cerr << "Error in choose_an_operator. NO operator had been chosen\n";
        exit(1);
    }

    void conduct_experiment(Rat &roger) {
        cout << "\n\nnew experiment began\n\n";
        vector<int> cur_generated_route;
        char start_char = "0";
        start_char = "Ы";
        roger.cur_direction = abs_direction::down;
        if (start_char == "0") {
            cerr << "Error in     void conduct_experiment(Rat &roger) :: strange start_char\n";
            exit(1);
        }
        roger.cur_cell_int = char_to_int[start_char];

        int i = 0;
        while (!roger.completed_route() && i <= 1000) {
            string chosen_operator = choose_an_operator();
            // Здесь можно выводить информацию от том, какой дискретный оператор в данный момент работает.
            // chosen_operator - это имя дискретного оператора, применяемого в данный момент.
            cout << chosen_operator;

#ifdef you_can_edit_this
            if (chosen_operator != "random" && chosen_operator != "inversion"
                && chosen_operator != "symmetry" && chosen_operator != "compression") {

                cerr << "Error: operator other than random or inversion has been chosen!..\n";
                exit(1);
            }
            map_from_name_to_operator[chosen_operator]->apply(*this, roger);
            if (chosen_operator == "random") {
                cout << " ";
                if (roger.cur_direction == abs_direction::right) {
                    cout << "right";
                } else if (roger.cur_direction == abs_direction::up) {
                    cout << "up";
                } else if (roger.cur_direction == abs_direction::left) {
                    cout << "left";
                } else if (roger.cur_direction == abs_direction::down) {
                    cout << "down";
                }
            } else if (chosen_operator == "inversion" || chosen_operator == "symmetry"
                       || chosen_operator == "compression") {
                cout << " applied";
            } else{
                cerr << "Error: strange operator!\n";
                exit(1);
            }
            cout << '\n';
#endif
            ++i;
        }
        if (i > 1000) {
            flag_has_to_stop = true;
        }
    }

    abs_direction get_abs_direction_from_cell_number_to_cell_number(int from_num, int to_num) {
        vector<abs_direction> all_abs_directions{abs_direction::left,
                                                 abs_direction::up,
                                                 abs_direction::right,
                                                 abs_direction::down};

        for (auto cur_abs_dir: all_abs_directions) {
            if (cells[from_num].neighs[cur_abs_dir] == to_num) {
                return cur_abs_dir;
            }
        }
        return abs_direction::left;
        cerr << "Error in get_abs_direction_from_cell_number_to_cell_number! Cannot determine direction\n";
        cerr << "from_num = " << from_num << "\nto_num = " << to_num << '\n';
        exit(1);
    }
#ifdef you_can_edit_this
    void construct_map_from_name_to_operator() {
        map_from_name_to_operator[discrete_operator.get_name()] = &discrete_operator;
        map_from_name_to_operator[operator_random.get_name()] = &operator_random;
        map_from_name_to_operator[operator_inversion.get_name()] = &operator_inversion;
        map_from_name_to_operator[operator_symmetry.get_name()] = &operator_symmetry;
        map_from_name_to_operator[operator_compression.get_name()] = &operator_compression;
        //map_from_name_to_operator[operator_ring.get_name()] = &operator_ring;
    }
#endif

    void conduct_series_of_experiments_memorizing_routes(size_t number_of_experiments, Rat& roger) {
        vector_of_generated_routes.clear();
        flag_has_to_stop = false;
        long long counter_of_wrong_routes = 0;
        for (size_t i = 0; i < number_of_experiments; ++i) {
            conduct_experiment(roger);
            if (flag_has_to_stop) {
                cout << "NO Routes\n";
                return;
            }
            vector<int> route_to_be_added = cur_generated_route;
            cur_generated_route.clear(); // чтобы маршруты не сливались в один
            if (route_to_be_added.size() < 2000) {
                vector_of_generated_routes.push_back(route_to_be_added);
            } else {
                if (++counter_of_wrong_routes > 0.3 * number_of_experiments) {
                    flag_has_to_stop = true;
                    return;
                }
            }
        }
    }

    void set_probabilities_for_operators(vector<double> vec) {
        if (vec.size() != 5) {
            cerr << "Error in set_probabilities_for_operators. Wrong size of vector of doubles!\n";
            exit(1);
        }
        double sum_of_probs = 0;
        for (double el: vec) {
            if (el < 0) {
                cerr << "Error in set_probabilities_for_operators. There is a negative probability!\n";
                exit(1);
            }
            sum_of_probs += el;
        }
        if (abs(1 - sum_of_probs) > eps) {
            cerr << "Error in set_probabilities_for_operators. Sum of probabilities is not equal to 1.0!\n";
            exit(1);
        }
#ifdef you_can_edit_this
        // general, random, inversion
        operators_to_probabilities[discrete_operator.get_name()] = vec[0];
        operators_to_probabilities[operator_random.get_name()] = vec[1];
        operators_to_probabilities[operator_inversion.get_name()] = vec[2];
        operators_to_probabilities[operator_symmetry.get_name()] = vec[3];
        operators_to_probabilities[operator_compression.get_name()] = vec[4];
        //operators_to_probabilities[operator_ring.get_name()] = vec[5];
#endif
    }


    void print_all_generated_routes(ostream &o) {
        o << "\nВозможная последовательность " << ":\n";
        /*for (size_t i = 0; i < cur_generated_route.size(); ++i) {
            if (int_to_char[cur_generated_route[i]] >= "А" &&
                    int_to_char[cur_generated_route[i]] <= "Я") {
                o << int_to_char[cur_generated_route[i]];
            }
        }
        o << "\n";*/
        for (size_t i = 0; i < vector_of_generated_routes.size(); ++i) {
            o << i +  1 << " вариант:\n";
            for (int j = 0; j < vector_of_generated_routes[vector_of_generated_routes.size() - i - 1].size(); ++j) {
                if (int_to_char[vector_of_generated_routes[vector_of_generated_routes.size() - i - 1][j]] >= "А" &&
                    int_to_char[vector_of_generated_routes[vector_of_generated_routes.size() - i - 1][j]] <= "Я") {
                    o << int_to_char[vector_of_generated_routes[vector_of_generated_routes.size() - i - 1][j]];
                }
            }
            o << "\n";
        }
    } // OK

    int process_random_move_of_rat(
            Rat *rat_ptr) { // функция, отвечающая за перемещение крысы из текущего участка лабиринта в соседний.
        // Возвращает char -- букву (русскую или английскую), характеризующую позицию крысы в лабиринте
        int cur_cell_int = rat_ptr->cur_cell_int;
        abs_direction cur_direction = rat_ptr->cur_direction;
        // int trying_to_move_to_cell = cells[cur_cell_int].neighs[cur_direction];
        vector<relative_direction> all_relative_directions = {relative_direction::forward,
                                                              relative_direction::to_left,
                                                              relative_direction::to_right,
                                                              relative_direction::backwards};
        double sum_of_probabilities_impossible = 0;
        for (relative_direction choice: all_relative_directions) {
            abs_direction new_direction = determine_new_direction(cur_direction, choice);
            if (0 == cells[cur_cell_int].neighs[new_direction]) {
                sum_of_probabilities_impossible += rat_ptr->probabilities_of_relative_dirs[choice];
                rat_ptr->cur_probabilities_of_abs_dirs[new_direction] = 0;
            }
        } // насчитали суммарную вероятность направлений, в которых двигаться невозможно

        if (abs(1 - sum_of_probabilities_impossible) < eps) {
            cerr
                    << "Error in function process_move_of_rat! We have no possible directions! (In processing rat move we almost divided by zero!)\n";
            cerr << "cur_cell = " << rat_ptr->cur_cell_int << "\n";
            exit(1);
        }
        for (relative_direction choice: all_relative_directions) {
            abs_direction new_direction = determine_new_direction(cur_direction, choice);
            if (0 != cells[cur_cell_int].neighs[new_direction]) {
                rat_ptr->cur_probabilities_of_abs_dirs[new_direction] =
                        rat_ptr->probabilities_of_relative_dirs[choice] *
                        (1 + sum_of_probabilities_impossible / (1 - sum_of_probabilities_impossible));
            }
        }
        abs_direction dir = rat_ptr->make_decision();
        int new_cell_int = cells[cur_cell_int].neighs[dir];
        rat_ptr->cur_cell_int = new_cell_int;
        rat_ptr->cur_direction = dir;
        cout << "NEW CELL " << new_cell_int << "\n";
        return new_cell_int;
    } // OK

    void read_experiment_results(istream &in, ostream &out) {
        string string_for_nothing;
        getline(in, string_for_nothing);
        getline(in, string_for_nothing); // пропустить первые две строки файла с экспериментальными данными
        // теперь можно читать данные
        string line;
        while (getline(in, line)) {
            if (line.size() < 3) {
                continue;
            }
            stringstream ss(line);
            string rats_name, session_str, trial_str, route;
            ss >> rats_name >> session_str >> trial_str >> route;

            vector<vector<string>> &vv = rats_name_to_experiment_results[rats_name]; // vv = vector of vectors
            //cout << session_str << '\n';
            int session = stoi(session_str);
            //cout << trial_str << '\n';
            int trial = stoi(trial_str);
            if (vv.size() == session) {
                vv.emplace_back();
            } else if (vv.size() < session) {
                vv.resize(session + 1, vector<string>());
            }
            vector<string> &v = vv[session]; // v = vector
            if (v.size() == trial) {
                v.emplace_back();
            } else if (v.size() < trial) {
                v.resize(trial + 1);
            }
            // проверить на ошибку. Добавить
            if (route_from_input_is_correct(route)) {
                v[trial] = route;
            }
        }
    } // OK

    void write_real_experiment_results_to_one_vector() {
        for (auto &pair: rats_name_to_experiment_results) {
            // string name_of_rat = pair.first;
            vector<vector<string>> &vv = pair.second;
            for (int cur_session = 1; cur_session < vv.size(); ++cur_session) {
                vector<string> &v = vv[cur_session];
                for (int cur_trial = 1; cur_trial < v.size(); ++cur_trial) {
                    string &cur_route = v[cur_trial];
                    if (cur_route.length() > 3) {
                        vector_of_real_routes.push_back(cur_route);
                    }
                }
            }
        }
    }
};

void Operator_Random::apply(Maze &maze, Rat &rat) {
    // cout << "Discrete operator of RANDOM applied!\n";
    maze.cur_generated_route.push_back(maze.process_random_move_of_rat(&rat));
}

void Operator_Random::set_probability(double probability) {
    prob = probability;
    cout << "Discrete operator RANDOM's probability has been set to " << prob << "\n";
}

void Operator_Inversion::apply(Maze &maze, Rat &rat) {
    // К текущему пути добавить последние len_of_path_to_inverse "русских" вершин в обратном порядке.
    // Начинаем с предпоследней вершины.

    int len_of_path_to_invert = round(my_generate_random(0.5 + eps, max_len_of_path_a_rat_can_invert + 0.5 - eps));
    int len_of_path_to_invert_saved = len_of_path_to_invert;
    // написать функцию, которая проходит по пути назад и добавляет максимум len_of_path_to_inverse "русских" вершин

    vector<int> &path = maze.cur_generated_route;
    int len_of_path = int(path.size());
    int i = len_of_path - 2; // начинаем с предпоследней вершины
    if (i < 1) {
        return;
    }
    while (len_of_path_to_invert > 0 && i >= 0) {

        int str_path = path[i];

        if (Is_Russian_Letter(str_path)) {
            --len_of_path_to_invert;
        }
        path.push_back(path[i]);
        --i;
    }

    if (path.size() >= 2) {
        rat.cur_cell_int = path[path.size() - 1];
        int str_path2 = path[path.size() - 2];
        int from_num = str_path2;
        int to_num = rat.cur_cell_int;
        rat.cur_direction = maze.get_abs_direction_from_cell_number_to_cell_number(from_num, to_num);
    }
}

void Operator_Inversion::set_probability(double probability) {
    prob = probability;
    cout << "Discrete operator INVERSION's probability has been set to " << prob << "\n";
}

void Operator_Symmetry::apply(Maze &maze, Rat &rat) {
    // Проверяем, можно ли построить симметричный путь, т.е. для каждой точки пройденного пути проверяем наличие симметричной
    // Ищем кратчайшее расстояние от последней точки текущего пути до первой точки симметричного и идем
    vector<int> &path = maze.cur_generated_route;
    int len = len = 5; // взял пока что такую константу
    if (path.size() < len) {
        //len = path.size();
        return;
    }
    vector<int> symmetric_path;

    /*for (int i = path.size() - 1; i > path.size() - len - 1; --i) { // находим для последних len точек пути симметричные им
        int symmetric_vertex = find_symmetric_vertex(path[i]);
        if (symmetric_vertex != -1) {
            symmetric_path.push_back(symmetric_vertex);
        } else {
            return;
        }
        cout << "LOGGING\n";
        for (auto i : symmetric_path) {
            cout << i << " ";
        }
        cout << "\n";
    }
    reverse(symmetric_path.begin(), symmetric_path.end());*/

    for (int i = path.size() - len; i < path.size(); ++i) { // находим для последних len точек пути симметричные им
        int symmetric_vertex = find_symmetric_vertex(path[i]);
        if (symmetric_vertex != -1) {
            symmetric_path.push_back(symmetric_vertex);
        } else {
            return;
        }
    }

    vector<int> direction_to_symmetric_path = maze.direct(path[path.size() - 1], symmetric_path[0]); // строим кратчайший путь
    if (direction_to_symmetric_path[0] != -1) {
        for (int i = 1; i < direction_to_symmetric_path.size(); ++i) {
            path.push_back(direction_to_symmetric_path[i]);
        }
        for (int i = 1; i < symmetric_path.size(); ++i) {
            path.push_back(symmetric_path[i]);
        }
        rat.cur_cell_int = path[path.size() - 1];
        int str_path2 = path[path.size() - 2];
        int from_num = str_path2;
        int to_num = rat.cur_cell_int;
        rat.cur_direction = maze.get_abs_direction_from_cell_number_to_cell_number(from_num, to_num);
    }

}

void Operator_Symmetry::set_probability(double probability) {
    prob = probability;
    cout << "Discrete operator Symmetry's probability has been set to " << prob << "\n";
}

void Operator_Compression::apply(Maze &maze, Rat &rat) {
    // Берем последние len точек пути и проверяем, можем ли мы добраться от path.size() - len до последней точки путем короче
    vector<int> &path = maze.cur_generated_route;
    if (path.size() < 3) {
        return;
    }
    int len = 10;
    if (path.size() < len) {
        return;
    }
    vector<int> path_to_compress, shortest_distance = {};
    for (int i = path.size() - len; i < path.size(); ++i) {
        path_to_compress.push_back(path[i]);
    }
    shortest_distance.push_back(path_to_compress[0]);
    int prev = 0;
    bool A = false, G = false;
    for (int i = 0; i < path_to_compress.size(); ++i) {
        if (path_to_compress[i] == 1 && !A && path_to_compress[prev] != path_to_compress[i]) {
            A = true;
            vector<int> shortest_distance_to_A = maze.direct(path_to_compress[prev], path_to_compress[i]);
            for (int j = 1; j < shortest_distance_to_A.size(); ++j) {
                shortest_distance.push_back(shortest_distance_to_A[j]);
            }
            prev = i;
        }
        else if (path_to_compress[i] == 4 && !G && path_to_compress[prev] != path_to_compress[i]) {
            G = true;
            vector<int> shortest_distance_to_G = maze.direct(path_to_compress[prev], path_to_compress[i]);
            for (int j = 1; j < shortest_distance_to_G.size(); ++j) {
                shortest_distance.push_back(shortest_distance_to_G[j]);
            }
            prev = i;
        }
    }
    if (path_to_compress[prev] != path_to_compress[path_to_compress.size() - 1]) {
        vector<int> last_short = maze.direct(path_to_compress[prev],
                                             path_to_compress[path_to_compress.size() - 1]); // cтроим кратчайший путь
        for (int j = 1; j < last_short.size(); ++j) {
            shortest_distance.push_back(last_short[j]);
        }
    }
    if (len > shortest_distance.size()) { // если получилось сжать - переписываем путь
        for (int i = 0; i < len; ++i) {
            path.pop_back();
        }
        for (int i = 0; i < shortest_distance.size(); ++i) {
            path.push_back(shortest_distance[i]);
        }
        if (shortest_distance.size() == 1) {
            rat.cur_cell_int = path[path.size() - 1];
        } else {
            rat.cur_cell_int = path[path.size() - 1];
            int str_path2 = path[path.size() - 2];
            int from_num = str_path2;
            int to_num = rat.cur_cell_int;
            rat.cur_direction = maze.get_abs_direction_from_cell_number_to_cell_number(from_num, to_num);
        }
    }

}

void Operator_Compression::set_probability(double probability) {
    prob = probability;
    cout << "Discrete operator Compression's probability has been set to " << prob << "\n";
}

double acc = 0.001;
std::vector<std::vector<double>> gen_grid(std::vector<double> l_bound, std::vector<double> u_bound, double step) {
    std::vector<std::vector<double>> grid;
    grid.reserve(1000000);
    std::vector<double> probs(4);
    probs[0] = l_bound[0];
    while (probs[0] < u_bound[0] + acc) {
        probs[1] = l_bound[1];
        while (probs[1] < u_bound[1] + acc) {
            probs[2] = l_bound[2];
            while (probs[2] < u_bound[2] + acc) {
                probs[3] = l_bound[3];
                while (probs[3] < min(u_bound[3], 1. - probs[0] - probs[1] - probs[2]) + acc) {
                    if (probs[3] + probs[2] + probs[1] + probs[0] < 1 + acc &&
                        probs[3] + probs[2] + probs[1] + probs[0] > 1 - acc) {
                        grid.push_back(probs);
                    }

                    probs[3] += step;
                }

                probs[2] += step;
            }

            probs[1] += step;
        }

        probs[0] += step;
    }

    return grid;
}

int do_lvn(std::string st_1, std::string st_2) {
    if (st_1.size() > st_2.size()) {
        return do_lvn(st_2, st_1);
    }

    const int min_size = st_1.size();
    const int max_size = st_2.size();
    std::vector<int> lev_dist(min_size + 1);

    for (int i = 0; i <= min_size; i++) {
        lev_dist[i] = i;
    }

    for (int j = 1; j <= max_size; j++) {
        int prev_diag = lev_dist[0], prev_diag_2;
        ++lev_dist[0];

        for (int i = 1; i <= min_size; i++) {
            prev_diag_2 = lev_dist[i];
            if (st_1[i - 1] == st_2[j - 1]) {
                lev_dist[i] = prev_diag;
            } else {
                lev_dist[i] = std::min(std::min(lev_dist[i - 1], lev_dist[i]), prev_diag) + 1;
            }
            prev_diag = prev_diag_2;
        }
    }

    return lev_dist[min_size];
}


int lvn(string &now, vector<string> &all_routes) { //вычисление метрики Л. для каждой пары (сгенерированная последовательность, реальный матршут)
    int minim = 1000000;
    for (int i = 1; i < all_routes.size(); ++i) {
        int ans_now = do_lvn(now, all_routes[i]);
        if (ans_now < minim) {
            minim = ans_now;
        }
    }
    return minim;
}

vector<double> good_probs(Maze &maze, Rat &roger, vector<string> &all_routes, ofstream &out, int &min) {
    double step = 0.1, maxi = 0.9, minim = 0.05;
    std::vector<std::vector<double>> all = gen_grid({0.1, 0.1, 0.1, 0.1}, {0.9, 0.9, 0.9, 0.9}, step); //создаем всевозможные комбинации вероятностей
    vector<double> ans_probs;
    for (vector<double> prob: all) {
        maze.set_probabilities_for_operators({0.0, prob[0], prob[1], prob[2], prob[3]});
        maze.conduct_series_of_experiments_memorizing_routes(3, roger);
        if (maze.vector_of_generated_routes.size() == 0) {
            continue;
        }
        std::string now = "";
        vector<int> gen_routes = maze.vector_of_generated_routes[0];
        for (int j = 0; j < gen_routes.size(); ++j) {
            int elem = maze.vector_of_generated_routes[0][j];
            if (elem >= 1 && elem <= 33) { // сохранение последовательности без дополнительных букв
                now += int_to_eng[elem];
            }
        }
        int lvn_now = lvn(now, all_routes);
        out << "\n";
        out << "Значения вероятностей:" << prob[0] << " " << prob[1] << " " << prob[2] << " " << prob[3] << "\n";
        out << "Значение метрики: " << lvn_now << "\n";
        if (lvn_now < min) { // если с текущими вероятностями значение метрики меньше, то сохраняем эти вероятности в ans_probs
            min = lvn_now;
            ans_probs = prob;
        }
    }
    return ans_probs; // возвращаем оптимальные вероятности
}

int main() {

    setlocale(LC_ALL, "ru");
    Maze maze = Maze();
    std::ifstream in_for_maze("Nikolskaya_maze_info.txt");

    if (in_for_maze.is_open()) {
        cout << "File with maze description has been opened for input successfully!\n";
    } else {
        cerr << "ERROR! Cannot open file with maze description for input!\n";
        exit(1);
    }

    std::ofstream out;
    out.open("output2.txt");
    if (in_for_maze.is_open()) {
        cout << "File for output opened successfully!\n";
    } else {
        cerr << "ERROR! Cannot open file for output!\n";
        exit(1);
    }
    maze.construct(in_for_maze, out);
    in_for_maze.close();
    maze.print_info_about_cells_in_letters(
            out);


    std::ifstream in_for_experiments("Nikolskaya_experiments.txt");
    if (in_for_experiments.is_open()) {
        cout << "File for input of experiment_results opened successfully!\n";
    } else {
        cerr << "ERROR! Cannot open file of experiment_results for input!\n";
        exit(1);
    }
    cout << "Started reading experiment results!\n";
    cout << "Finished reading experiment results!\n";
    cout << "Started writing experiment results!\n";
    cout << "Finished writing experiment results!\n";

    Rat roger;
    maze.construct_map_from_name_to_operator();

    double delimiter = 100;
    std::ifstream in_for_probs("Nikolskaya_last_exp.txt");


    roger.set_probabilities_of_relative_dirs(54 / delimiter, 20 / delimiter, 20 / delimiter, 6 / delimiter);

    int min = 1000000;

    vector<string> all_routes;

    string line;
    while (getline(in_for_probs, line)) { //сохранение всех реальных последних маршрутов в вектор
        all_routes.push_back(line);
    }


    vector<double> pr = good_probs(maze, roger, all_routes, out, min); // h
    out << "\n";
    out << "Оптимальные значения вероятностей:" << pr[0] << " " << pr[1] << " " << pr[2] << " " << pr[3] << "\n";
    out << "Метрика Левенштейна: " << min << "\n";

    maze.set_probabilities_for_operators({0.0, pr[0], pr[1], pr[2], pr[3]});
    maze.conduct_series_of_experiments_memorizing_routes(10, roger);

    std::string now = "";

    if (maze.vector_of_generated_routes.size() != 0) {
        for (int i = 0; i < maze.vector_of_generated_routes.size(); ++i) {
            vector<int> gen_routes = maze.vector_of_generated_routes[i];

            for (int j = 0; j < gen_routes.size(); ++j) {
                int elem = maze.vector_of_generated_routes[i][j];
                if (elem >= 1 && elem <= 33) { // сохранение последовательности без дополнительных букв
                    now += int_to_eng[elem];
                }
            }
            int lvn_now = lvn(now, all_routes);
            out << "\nЗначения вероятностей:" << pr[0] << " " << pr[1] << " " << pr[2] << " " << pr[3] << "\n";
            out << "Значение метрики: " << lvn_now;


            maze.print_all_generated_routes(out);
        }
    }

    in_for_experiments.close();
    in_for_probs.close();
    out.close();
    out << "\nDONE!\n";
    std::cout << "End of program" << "\n";

    return 0;
}
