#include <iostream>
#include <vector>
#include <random>
#include <unordered_map>
#include <fstream>
#include <chrono>
#include <cmath>
#include <algorithm>

using namespace std;

// S -> TST ;  T1#a < 2 ^ (T2#a - 1)
// S -> SbS
// S -> aaa
// T -> bb
// T -> TaT

//генерация случайной строки из языка
void get_random_string_from_lang(string& s, int max_count_rules, int max_count_a_in_T2){
    s = "S";

    random_device rd;
    mt19937 gen(rd());

    //для выбора случайного числа правил (S -> TST и S -> SbS)
    uniform_int_distribution<int> dist1(1, max_count_rules);
    int count_rules = dist1(gen);

    //для выбора между правилами S -> TST и S -> SbS
    uniform_int_distribution<int> dist2(0, 1);

    //будем пользоваться пока только правилами S -> TST и S -> SbS
    for (int i = 0; i < count_rules; i++){
        if (dist2(gen) == 0){
            vector<int> possible_positions;
            int pos = s.find("S");
            if (pos == string::npos){
                continue;
            }
            while (pos != string::npos) {
                possible_positions.push_back(pos);
                pos = s.find("S", pos + 1);
            }

            //для выбора позиции в векторе
            uniform_int_distribution<int> dist3(0, possible_positions.size() - 1);

            //для выбора сколько букв "a" взять для T2
            uniform_int_distribution<int> dist4(1, max_count_a_in_T2);
            int count_a_in_T2 = dist4(gen);

            
            string T2 = "bb";
            for (int i = 0; i < count_a_in_T2; i++) {
                T2 += "abb";
            }

            //для выбора сколько букв "a" взять для T1
            uniform_int_distribution<int> dist5(0, pow(2, count_a_in_T2 - 1) - 1);
            int count_a_in_T1 = dist5(gen);

            string T1 = "bb";
            for (int i = 0; i < count_a_in_T1; i++) {
                T1 += "abb";
            }

            string TST = T1 + "S" + T2;

            s.replace(possible_positions[dist3(gen)], 1, TST);

        }else{
            vector<int> possible_positions;
            int pos = s.find("S");
            if (pos == string::npos){
                continue;
            }
            while (pos != string::npos) {
                possible_positions.push_back(pos);
                pos = s.find("S", pos + 1);
            }

            //для выбора позиции в векторе
            uniform_int_distribution<int> dist6(0, possible_positions.size() - 1);
            s.replace(possible_positions[dist6(gen)], 1, "SbS");
        }
        
    }
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == 'S') {
            s.replace(i, 1, "aaa");
            i += 2;
        }
    }
}

//генерация просто случайной строки, скорее всего будет не в языке
//(не знаю, как случайно генерировать строки не из языка)
string get_random_string(int n, string alphabet = "ab") {
    string s = "";
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, alphabet.size() - 1);
    for (int i = 0; i < n; i++){
        s += alphabet[dist(gen)];
    }
    return s;
}

vector<int> S(int pos, const string& s);
vector<int> P(int pos, const string& s);
vector<int> T(int pos, const string& s);
vector<int> Q(int pos, const string& s);


//основная идея - после обработки каждого нетерминала
//возвращать список позиций в строке, где корректно мог закончиться нетерминал

//S -> aaaP | aaa | TSTP | TST
vector<int> S(int pos, const string& s){
    if (s[pos] == '$' || s[pos + 1] == '$' || s[pos + 2] == '$'){
        return {};
    }
    if (s[pos] == 'a' && s[pos + 1] == 'a' && s[pos + 2] == 'a'){
        pos += 3;
        int prev_pos = pos;
        vector<int> all_pos;
        all_pos.push_back(prev_pos); //так как есть правило S -> aaa
        vector<int> all_pos1 = P(pos, s); 
        all_pos.insert(all_pos.end(), all_pos1.begin(), all_pos1.end());
        return all_pos;

    }else{
        int copy_pos1 = pos;  //для подсчёта букв "a"
        vector<int> all_pos = {};
        vector<int> all_pos1 = T(pos, s);
        

        if (all_pos1.empty()){
            return {};
        }

        for (int i: all_pos1){
            string substring = s.substr(copy_pos1, i - copy_pos1);
    
            int count1_a = count(substring.begin(), substring.end(), 'a');

            pos = i;
            
            vector<int> all_pos2 = S(pos, s);

            if (all_pos2.empty()){
                continue;
            }

            for (int i: all_pos2){
                int copy_pos2 = i; //для подсчёта букв "a"
                pos = i;
                vector<int> all_pos3 = T(pos, s);

                if (all_pos3.empty()){
                    continue;;
                }

                all_pos.insert(all_pos.end(), all_pos3.begin(), all_pos3.end()); //так как есть правило S->TST

                for (int i: all_pos3){
                    string substring = s.substr(copy_pos2, i - copy_pos2);
                    int count2_a = count(substring.begin(), substring.end(), 'a');

                    if ((count2_a < 1) || (count1_a >= pow(2, count2_a - 1))){
                        continue;
                    }

                    pos = i;
                    vector<int> all_pos4 = {};
                    if (s[pos] != '$'){
                        all_pos4 = P(pos, s);
                        all_pos.insert(all_pos.end(), all_pos4.begin(), all_pos4.end());
                    }
                }
            }
        }
        return all_pos;
    }
}

// P -> bSP | bS
vector<int> P(int pos, const string& s){
    if (s[pos] == '$'){
        return {};
    }
    
    if (s[pos] != 'b'){
        return {};
    }else{
        
        pos += 1;
        if (s[pos] == '$'){
            return {};
        }
        vector<int> all_pos = {};
        vector<int> all_pos1 = S(pos, s);
        if (all_pos1.empty()){
            return {};
        }

        all_pos.insert(all_pos.end(), all_pos1.begin(), all_pos1.end()); //так как есть правило P -> bS
        for (int i: all_pos1){
            pos = i;
            vector<int> all_pos2 = {};
            if (s[pos] != '$'){
                all_pos2 = P(pos, s);
                all_pos.insert(all_pos.end(), all_pos2.begin(), all_pos2.end());
            }
        }
        return all_pos;
    }
}

// T -> bbQ | bb
vector<int> T(int pos, const string& s){
    if (s[pos] == '$' || s[pos + 1] == '$'){
        return {};
    }
    if (s[pos] != 'b' || s[pos + 1] != 'b'){
        return {};
    }else{
        pos += 2;
        vector<int> all_pos;
        all_pos.push_back(pos);

        if (s[pos] == '$'){
            return all_pos;
        }
        vector<int> all_pos1 = Q(pos, s);
        all_pos.insert(all_pos.end(), all_pos1.begin(), all_pos1.end());
        return all_pos;
    }
}



// Q -> aTQ | aT
vector<int> Q(int pos, const string& s){
    if (s[pos] == '$'){
        return {};
    }
    
    if (s[pos] != 'a'){
        return {};
    }else{
        
        pos += 1;
        if (s[pos] == '$'){
            return {};
        }
        vector<int> all_pos;
        vector<int> all_pos1 = T(pos, s);
        if (all_pos1.empty()){
            return {};
        }

        all_pos.insert(all_pos.end(), all_pos1.begin(), all_pos1.end()); //так как есть правило Q -> aT
        for (int i: all_pos1){
            pos = i;
            vector<int> all_pos2 = {};
            if (s[pos] != '$'){
                all_pos2 = Q(pos, s);
                all_pos.insert(all_pos.end(), all_pos2.begin(), all_pos2.end());
            }
        }
        return all_pos;
    }
}

bool check(const vector<int>& v, int len){
    for (int p: v){
        if (p == len - 1){
            return true;
        }
    }
    return false;
}













unordered_map<int, vector<int>> cS;
unordered_map<int, vector<int>> cP;
unordered_map<int, vector<int>> cT;
unordered_map<int, vector<int>> cQ;

//i - improved
vector<int> iS(int pos, const string& s);
vector<int> iP(int pos, const string& s);
vector<int> iT(int pos, const string& s);
vector<int> iQ(int pos, const string& s);

//основная идея - после обработки каждого нетерминала
//возвращать список позиций в строке, где корректно мог закончиться нетерминал

//S -> aaaP | aaa | TSTP | TST
vector<int> iS(int pos, const string& s){
    int copy_pos = pos;  //заведем copy_pos, потому что pos может потеряться (когда будут вложенные циклы, например)
    auto it = cS.find(copy_pos);
    if (it != cS.end()) {
        return it->second;
    }

    if (s[pos] == '$' || s[pos + 1] == '$' || s[pos + 2] == '$'){
        cS[copy_pos] = {};
        return {};
    }
    if (s[pos] == 'a' && s[pos + 1] == 'a' && s[pos + 2] == 'a'){
        pos += 3;
        int prev_pos = pos;
        vector<int> all_pos;
        all_pos.push_back(prev_pos); //так как есть правило S -> aaa
        vector<int> all_pos1 = iP(pos, s); 
        all_pos.insert(all_pos.end(), all_pos1.begin(), all_pos1.end());
        cS[copy_pos] = all_pos;
        return all_pos;

    }else{
        int copy_pos1 = pos;  //для подсчёта букв "a"
        vector<int> all_pos = {};
        vector<int> all_pos1 = iT(pos, s);
        cS[copy_pos] = {};

        if (all_pos1.empty()){
            return {};
        }

        for (int i: all_pos1){
            string substring = s.substr(copy_pos1, i - copy_pos1);
            int count1_a = count(substring.begin(), substring.end(), 'a');
            pos = i;
            
            vector<int> all_pos2 = iS(pos, s);

            if (all_pos2.empty()){
                continue;
            }

            for (int i: all_pos2){
                int copy_pos2 = i; //для подсчёта букв "a"

                pos = i;
                vector<int> all_pos3 = iT(pos, s);

                if (all_pos3.empty()){
                    continue;;
                }

                all_pos.insert(all_pos.end(), all_pos3.begin(), all_pos3.end()); //так как есть правило S->TST
                for (int i: all_pos3){
                    string substring = s.substr(copy_pos2, i - copy_pos2);
                    int count2_a = count(substring.begin(), substring.end(), 'a');

                    if ((count2_a < 1) || (count1_a >= pow(2, count2_a - 1))){
                        continue;
                    }

                    pos = i;
                    vector<int> all_pos4 = {};
                    if (s[pos] != '$'){
                        all_pos4 = iP(pos, s);
                        all_pos.insert(all_pos.end(), all_pos4.begin(), all_pos4.end());
                    }
                }
            }
        }
        cS[copy_pos] = all_pos;
        return all_pos;
    }
}

// P -> bSP | bS
vector<int> iP(int pos, const string& s){
    int copy_pos = pos;
    auto it = cP.find(copy_pos);
    if (it != cP.end()) {
        return it->second;
    }
    cP[copy_pos] = {};

    if (s[pos] == '$'){
        return {};
    }
    
    if (s[pos] != 'b'){
        return {};
    }else{
        
        pos += 1;
        if (s[pos] == '$'){
            return {};
        }
        vector<int> all_pos = {};
        vector<int> all_pos1 = iS(pos, s);
        if (all_pos1.empty()){
            return {};
        }

        all_pos.insert(all_pos.end(), all_pos1.begin(), all_pos1.end()); //так как есть правило P -> bS
        for (int i: all_pos1){
            pos = i;
            vector<int> all_pos2 = {};
            if (s[pos] != '$'){
                all_pos2 = iP(pos, s);
                all_pos.insert(all_pos.end(), all_pos2.begin(), all_pos2.end());
            }
        }
        cP[copy_pos] = all_pos;
        return all_pos;
    }
}

// T -> bbT2 | bb
vector<int> iT(int pos, const string& s){
    int copy_pos = pos;
    auto it = cT.find(copy_pos);
    if (it != cT.end()) {
        return it->second;
    }

    cT[copy_pos] = {};


    if (s[pos] == '$' || s[pos + 1] == '$'){
        return {};
    }
    if (s[pos] != 'b' || s[pos + 1] != 'b'){
        return {};
    }else{
        pos += 2;
        vector<int> all_pos;
        all_pos.push_back(pos);

        if (s[pos] == '$'){
            return all_pos;
        }
        vector<int> all_pos1 = iQ(pos, s);
        all_pos.insert(all_pos.end(), all_pos1.begin(), all_pos1.end());
        cT[copy_pos] = all_pos;
        return all_pos;
    }
}



// Q -> aTQ | aT
vector<int> iQ(int pos, const string& s){
    int copy_pos = pos;
    auto it = cQ.find(copy_pos);
    if (it != cQ.end()) {
        return it->second;
    }
    cQ[copy_pos] = {};

    if (s[pos] == '$'){
        return {};
    }
    
    if (s[pos] != 'a'){
        return {};
    }else{
        
        pos += 1;
        if (s[pos] == '$'){
            return {};
        }
        vector<int> all_pos;
        vector<int> all_pos1 = iT(pos, s);
        if (all_pos1.empty()){
            return {};
        }

        all_pos.insert(all_pos.end(), all_pos1.begin(), all_pos1.end()); //так как есть правило Q -> aT
        for (int i: all_pos1){
            vector<int> all_pos2 = {};
            if (s[pos] != '$'){
                all_pos2 = iQ(pos, s);
                all_pos.insert(all_pos.end(), all_pos2.begin(), all_pos2.end());
            }
        }
        cQ[copy_pos] = all_pos;
        return all_pos;
    }
}



int main(){
    string s = "";

    int is_error = 0;
    string error_word = "";

    //храним пары (суммарное время, число тестов)
    vector<pair<double, double>> t1(5, {0, 0}); //для наивного парсера
    vector<pair<double, double>> t2(5, {0, 0}); //для умного парсера
    //фаззинг на словах из языка
    for (int i = 0; i < 500; i++){
        get_random_string_from_lang(s, 10, 8);
        s += "$";
        int len = s.size();
        cS.clear();
        cP.clear();
        cT.clear();
        cQ.clear();

        //на длинных словах наивный фаззинг полностью оправдывает своё название
        if (len > 70){
            continue;
        }
        

        double time1;
        auto start1 = std::chrono::high_resolution_clock::now();
        int res1 = check(S(0, s), len);
        auto end1 = std::chrono::high_resolution_clock::now();
        chrono::duration<double> duration1 = end1 - start1;
        time1 = duration1.count();

        double time2;
        auto start2 = std::chrono::high_resolution_clock::now();
        int res2 = check(iS(0, s), len);
        auto end2 = std::chrono::high_resolution_clock::now();
        chrono::duration<double> duration2 = end2 - start2;
        time2 = duration2.count();


        if ((len > 65) && (len <= 70)){
            t1[0].first += time1;
            t1[0].second += 1;
            t2[0].first += time2;
            t2[0].second += 1;
        }

        if ((len > 60) && (len <= 65)){
            t1[1].first += time1;
            t1[1].second += 1;
            t2[1].first += time2;
            t2[1].second += 1;
        }

        if ((len > 55) && (len <= 60)){
            t1[2].first += time1;
            t1[2].second += 1;
            t2[2].first += time2;
            t2[2].second += 1;
        }

        if ((len > 50) && (len <= 55)){
            t1[3].first += time1;
            t1[3].second += 1;
            t2[3].first += time2;
            t2[3].second += 1;
        }

        if ((len > 45) && (len <= 50)){
            t1[4].first += time1;
            t1[4].second += 1;
            t2[4].first += time2;
            t2[4].second += 1;
        }
        

        if (!(res1 && res2)){
            is_error = 1;
            error_word = s;
            break;
        }
    }
    if (!is_error){
        cout << "Все тесты для слов из языка успешны" << endl;
    }else{
        cout << "Ошибка для слов из языка" << endl;
        cout << error_word << endl;
    }

    cout << "резы для наивного (из языка):" << endl;
    for (pair<double, double> p: t1){
        if (p.second != 0){
           cout << (p.first / p.second) << endl; 
        } 
    }

    cout << "резы для умного (из языка):" << endl;
    for (pair<double, double> p: t2){
        if (p.second != 0){
           cout << (p.first / p.second) << endl; 
        } 
    }


    t1 = vector<pair<double, double>>(5, {0, 0});
    t2 = vector<pair<double, double>>(5, {0, 0});

    //фаззинг на словах НЕ из языка
    for (int i = 0; i < 10; i++){
        for (int j = 100000; j < 200000; j+= 101){
            s = get_random_string(j);
            s += "$";
            int len = s.size();
            if (check(iS(0, s), len)){
                continue;
            }
            cS.clear();
            cP.clear();
            cT.clear();
            cQ.clear();
            
            
            double time1;
            auto start1 = std::chrono::high_resolution_clock::now();
            int res1 = check(S(0, s), len);
            auto end1 = std::chrono::high_resolution_clock::now();
            chrono::duration<double> duration1 = end1 - start1;
            time1 = duration1.count();

            double time2;
            auto start2 = std::chrono::high_resolution_clock::now();
            int res2 = check(iS(0, s), len);
            auto end2 = std::chrono::high_resolution_clock::now();
            chrono::duration<double> duration2 = end2 - start2;
            time2 = duration2.count();


            if ((len > 180000) && (len <= 200000)){
                t1[0].first += time1;
                t1[0].second += 1;
                t2[0].first += time2;
                t2[0].second += 1;
            }

            if ((len > 160000) && (len <= 180000)){
                t1[1].first += time1;
                t1[1].second += 1;
                t2[1].first += time2;
                t2[1].second += 1;
            }

            if ((len > 140000) && (len <= 160000)){
                t1[2].first += time1;
                t1[2].second += 1;
                t2[2].first += time2;
                t2[2].second += 1;
            }

            if ((len > 120000) && (len <= 140000)){
                t1[3].first += time1;
                t1[3].second += 1;
                t2[3].first += time2;
                t2[3].second += 1;
            }

            if ((len > 100000) && (len <= 120000)){
                t1[4].first += time1;
                t1[4].second += 1;
                t2[4].first += time2;
                t2[4].second += 1;
            }
        

            if (!res1){  //не знаю, как по-другому сделать случайное слово не из языка
                if (res2){
                    is_error = 1;
                    error_word = s;
                    break;
                }
            }

        }

    }

    cout << "резы для наивного (не из языка):" << endl;
    for (pair<double, double> p: t1){
        if (p.second != 0){
           cout << (p.first / p.second) << endl; 
        } 
    }

    cout << "резы для умного (не из языка):" << endl;
    for (pair<double, double> p: t2){
        if (p.second != 0){
           cout << (p.first / p.second) << endl; 
        } 
    }

    if (!is_error){
        cout << "Все тесты для слов не из языка успешны" << endl;
    }else{
        cout << "Ошибка для слов не из языка" << endl;
        cout << error_word << endl;
    }
}
