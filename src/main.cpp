#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <set>
#include <utility>
#include <string>
#include <string.h>
#include <math.h>

using namespace std;

// express terms that multiply together
class MulTerms{
public:
    unordered_map<string, int> terms;

    MulTerms(){terms.clear();}
    MulTerms(string temp){terms[temp]++;}

    MulTerms& operator*=(MulTerms& mul){
        for(auto it : mul.terms){
            if(terms[it.first] == 0) terms[it.first]++;
        }

        return *this;
    }

    bool inside(MulTerms& check){
        for(auto it : terms){
            if(check.terms[it.first] != 1) return false;
        }
        return true;
    }

    void output(){
        cout << "Multerm output: \n";
        for(auto it : terms){
            cout << it.first << " ";
        }
        cout << endl;
    }
};


// express MulTerms that add together and can call multiply
class PTerms{
public:
    vector<MulTerms> cover;

    PTerms(){cover.clear();}
    PTerms(vector<string> temp){
        for(auto it : temp){
            MulTerms adds(it);
            cover.push_back(adds);
        }
    }

    bool exist(MulTerms& check){
        for(auto it : this->cover){
            if(it.inside(check)) return true;
            if(check.inside(it)){
                it.terms = check.terms;
                return true;
            }
        }

        return false;
    }

    PTerms& operator*=(PTerms& mul){
        if(cover.empty()){
            this->cover = mul.cover;
            return *this;
        }

        PTerms temp;
        temp.cover = this->cover;
        this->cover.clear();
        for(auto it1 : temp.cover){
            for(auto it2 : mul.cover){
                MulTerms t = it1;
                t *= it2;
                if(!this->exist(t)){
                    cover.push_back(t);
                }
            }
        }

        return *this;
    }

    void output(){
        cout << "SOP output: \n";
        for(auto it1 : cover){
            for(auto it2 : it1.terms){
                cout << it2.first << " ";
            }
            cout << endl;
        }
    }
};


class QM{
private:
    int var_num;
    int term_num;
    int min_lit = 1e9, min_term = 1e9;

    vector<string> minterm;     // store minterm
    vector<string> check_prime;
    vector<string> prime;       // prime after QM 
    unordered_map<string, vector<int>> prime_cover; // check prime's minterm cover
    unordered_map<string, vector<string>> check_occur; // minterm cover
    vector<string> out_prime;   // final output prime 


public:  
    void init(ifstream &in){
        // read input from file
        in>>var_num;
        in>>term_num;
        string temp;
        for(int i = 0; i < term_num; i++){
            in>>temp;
            minterm.push_back(temp);
            check_prime.push_back(temp);
        }
    }

    //done
    bool hit(string a, string b, string* out){
        // check whether a and b can be merged 
        // if can merge, return true and store the merged term in out
        int flag = 0, index = -1;
        for(int i = 0; i < a.length();i++){
            if(a[i]!=b[i]){
                if(flag==0){
                    flag = 1;
                    index = i;
                }else{
                    return false;
                }
            }
        }
        *out = a;
        (*out)[index] = '-';
        return true;
    }

    //done
    bool check_exist(vector<string> vec, string str){
        int size = vec.size();
        
        for(int i=0; i<size; i++){
            if(vec[i] == str) return true;
        }

        return false;
    }
    
    //done
    void merge_implicants(){
        // Merges groups of minterms with one difference into a single term with a dash ("-").
        string tempout = "";
        int size = check_prime.size();
        vector<string> temp;
        vector<bool> haveMerged(size, false); // check if minterm have merged yet

        for(int i=0; i<size; i++){
            for(int j=i+1; j<size; j++){
                if(hit(check_prime[i], check_prime[j], &tempout)){
                    haveMerged[i] = haveMerged[j] = true;
                    if(!check_exist(temp, tempout)) temp.push_back(tempout);
                }
            }

            if(!haveMerged[i]) prime.push_back(check_prime[i]);
        }

        check_prime.clear();
        check_prime.insert(check_prime.end(), temp.begin(), temp.end());
    }
    
    //done
    void find_prime_implicants(){
        while(!check_prime.empty()){
            merge_implicants();
        }
    }

    //done
    bool check_prime_cover(string prime, string minterm){
        for(int i=0; i<var_num; i++){
            if(prime[i] != '-' && prime[i] != minterm[i]) return false;
        }

        return true;
    }

    //done
    bool check_in_outprime(string check){
        for(auto it : out_prime){
            if(it == check) return true;
        }

        return false;
    }

    //done
    void check_cover(){
        int primes = prime.size();

        for(int i=0; i<term_num; i++){
            for(int j=0; j<primes; j++){
                if(check_prime_cover(prime[j], minterm[i])){
                    check_occur[minterm[i]].push_back(prime[j]);
                    prime_cover[prime[j]].push_back(i);
                }
            }
        }
    }

    bool same_cover(vector<string> v1, vector<string> v2){
        //check if v1 in v2
        if(v1.size() > v2.size() || v1.empty() || v2.empty()) return false;
        for(auto it : v1){
            if(find(v2.begin(), v2.end(), it) == v2.end()){
                return false;
            }
        }
        return true;
    }

    void eliminate_same_cover(){
        for(int i=0; i<term_num; ++i){
            for(int j=i+1; j<term_num; ++j){
                //check check_occur[minterm[i]] in check_occur[minterm[j]]
                if(same_cover(check_occur[minterm[i]], check_occur[minterm[j]])){
                    check_occur[minterm[j]].clear();
                }else if(same_cover(check_occur[minterm[j]], check_occur[minterm[i]])){
                    check_occur[minterm[i]].clear();
                }                
            }
        }
    }

    //done
    void find_essential_prime_implicants(){
        // Finds essential prime implicants from the prime implicants.
        check_cover();

        for(int i=0; i<term_num; i++){
            if(check_occur[minterm[i]].size() == 1){
                string temp = check_occur[minterm[i]][0]; //the prime
                if(check_in_outprime(temp)) continue;
                out_prime.push_back(temp);

                //find all minterm that has covered by the prime and clear it
                for(auto it : prime_cover[temp]){
                    check_occur[minterm[it]].clear();
                }
            }
        }

        eliminate_same_cover();
    }

    void find_min_cover(){
        // Finds the minimum cover of the left minterms 
        // after finding the essential prime implicants.
        if(out_prime.size() == prime.size()) return;

        PTerms allcover;
        for(int i=0; i<term_num; ++i){
            if(check_occur[minterm[i]].size() <= 1) continue;
            PTerms temp(check_occur[minterm[i]]);

            allcover *= temp;
        }

        int lit_count;
        MulTerms min_cover;
        for(auto it : allcover.cover){
            lit_count = 0;
            for(auto it1 : it.terms){
                lit_count += var_num - count(it1.first.begin(), it1.first.end(), '-');
            }

            if(lit_count < min_lit || (lit_count == min_lit && min_term > it.terms.size())){
                min_lit = lit_count;
                min_term = it.terms.size();
                min_cover = it;
            }
        }

        for(auto it : min_cover.terms){
            out_prime.push_back(it.first);
        }
    }

    //done
    bool check_equal(){
        // check whether the out prime implicants are equal to the minterms
        // if equal, return true
        // Please remain vector minterm unchanged and store the result in vector out_prime before call this function

        queue<string> q;
        unordered_set<string> temp;
        unordered_map<string, bool> mp, mp2;
        for(auto &a : minterm){
            mp[a] = true;
        }
        for(auto &a : out_prime){  
            if(!mp2[a]){
                q.push(a);
                mp2[a] = true;
            }
        }
        while(!q.empty()){
            auto a = q.front();
            q.pop();
            
            int i = 0;
            for(i = 0; i < a.length(); i++){
                if(a[i]=='-'){
                    auto b = a;
                    b[i] = '0';
                    q.push(b);
                    b[i] = '1';
                    q.push(b);
                    break;
                }
            }
            if(i == a.length()){
                if(mp[a]){
                    temp.insert(a);
                }
                else{
                    cout<<"output prim cover more than minterm"<<endl;
                    cout<<"output prime implicants are not equal to the minterms\n"<<endl;
                    return false;
                }
            }
        }
        if(temp.size() == minterm.size()){
            cout<<"output prime implicants are equal to the minterms\n"<<endl;
            return true;
        }
        else{
            // cout<<"minterm size : "<<minterm.size()<<endl;
            // cout<<"Output prime expand minterm size : "<<temp.size()<<endl;
            cout<<"output prime implicants are not equal to the minterms\n"<<endl;
            
            return false;
        }
        
    }
    
    void output_prime(ofstream & out){
        int litteral_count = 0;
        for(auto &a : out_prime){
            litteral_count += var_num - count(a.begin(), a.end(), '-');
        }
        out<<litteral_count<<endl<<out_prime.size()<<endl;
        for(auto &a : out_prime){
            out << a << endl;
        }
    }


};

int main(int argc, char* argv[]){
    QM qm;
    
    // command example: ./main ../testcase/case00.in ../output/case00.out
    ifstream in(argv[1]);
    ofstream out(argv[2]);

    // read input from file
    qm.init(in);

    // Finds the prime implicants from the minterms.
    qm.find_prime_implicants();

    // Finds essential prime implicants from the prime implicants.
    qm.find_essential_prime_implicants();

    // Finds the minimum cover of the left minterms 
    // after finding the essential prime implicants.
    qm.find_min_cover();

    // check whether the out prime implicants are equal to the minterms
    // Please remain vector minterm unchanged and store the result in vector out_prime before call this function
    qm.check_equal();

    qm.output_prime(out);
    
    return 0;
}