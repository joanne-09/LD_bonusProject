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
    vector<string> mulT;
    int size = 0;

    MulTerms(){mulT.clear();}
    MulTerms(string temp){
        mulT.clear();
        mulT.push_back(temp);
        size = 1;
    }
    MulTerms(vector<string> temp): mulT(temp){size = temp.size();};

    bool check(string ch){
        for(int i=0; i<mulT.size(); ++i){
            if(ch == mulT[i]) return false;
        }
        return true;
    }

    bool check_include(MulTerms& ch){
        // check if ch include in this
        for(auto it : ch.mulT){
            if(!check(it)) return false;
        }

        return true;
    }

    MulTerms& operator*=(MulTerms& mul){
        for(int i=0; i<mul.mulT.size(); ++i){
            if(check(mul.mulT[i]))
                this->mulT.push_back(mul.mulT[i]);
        }

        return *this;
    }
};


// express MulTerms that add together and can call multiply
class PTerms{
public:
    vector<MulTerms> cover;
    int size = 0;

    PTerms(){cover.clear();}
    PTerms(vector<MulTerms> par): cover(par){size = par.size();};

    PTerms& operator*=(PTerms& mul){
        PTerms merge(this->cover);
        // indicate the number of multerms
        int size1 = this->cover.size(), size2 = mul.cover.size();

        this->cover.clear();
        if(size1 == 0){
            *this = mul;
            return *this;
        }

        for(int i=0; i<size1; ++i){
            for(int j=0; j<size2; ++j){
                MulTerms temp(merge.cover[i].mulT);

                temp *= mul.cover[j];
                this->cover.push_back(temp);
            }
        }

        return *this;
    }

    bool check_include(PTerms& ch){
        ;
    }
};


class QM{
private:
    int var_num;
    int term_num;

    vector<string> minterm;     // store minterm
    vector<string> check_prime;
    vector<string> prime;       // prime after QM 
    unordered_map<string, PTerms> check_occur;
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
                    MulTerms t1(minterm[i]), t2(prime[j]);
                    check_occur[prime[j]].cover.push_back(t1);
                    check_occur[minterm[i]].cover.push_back(t2);
                    check_occur[minterm[i]].size ++;
                    check_occur[prime[j]].size ++;
                }
            }
        }
    }

    void check_same_cover(){
        for(int i=0; i<term_num; ++i){
            for(int j=i+1; j<term_num; ++j){
                if(check_occur[minterm[i]].size == 1) continue;
                
            }
        }
    }

    //done
    void find_essential_prime_implicants(){
        // Finds essential prime implicants from the prime implicants.
        check_cover();

        for(int i=0; i<term_num; i++){
            if(check_occur[minterm[i]].cover.size() == 1){
                string temp = check_occur[minterm[i]].cover[0].mulT[0]; //the prime
                if(check_in_outprime(temp)) continue;
                out_prime.push_back(temp);

                //find all minterm that has covered by the prime and clear it
                MulTerms mt(temp);
                for(auto it : check_occur[temp].cover){
                    check_occur[it.mulT[0]].cover.clear();
                    check_occur[it.mulT[0]].cover.push_back(mt);
                    check_occur[it.mulT[0]].size = 1;
                }
            }
        }
    }


    void find_min_cover(){
        // Finds the minimum cover of the left minterms 
        // after finding the essential prime implicants.
        if(out_prime.size() == prime.size()) return;

        PTerms allCovers;
        for(int i=0; i<term_num; i++){
            if(check_occur[minterm[i]].size == 1) continue;
            allCovers *= check_occur[minterm[i]];
        }

        int min_lit = 1e9, min_terms = 1e9;
        MulTerms min_cover;
        for(auto it : allCovers.cover){
            int total_lit = 0, terms = it.mulT.size();
            for(auto it1 : it.mulT){
                total_lit += var_num - count(it1.begin(), it1.end(), '-');
            }

            if(total_lit < min_lit || (total_lit == min_lit && terms < min_terms)){
                min_lit = total_lit;
                min_terms = terms;
                min_cover = it;
            }
        }

        for(auto it : min_cover.mulT){
            out_prime.push_back(it);
        }
    }

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