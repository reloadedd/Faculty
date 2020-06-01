#include <map>
#include <cmath>
#include <vector>
#include <iostream>

using namespace std;

vector<int> getSol(int i, int j){
    vector<int> response;
    for(int a = 2; a < j; a++){
        int rez = a;
        bool isFirst = 1;
        for(int b = 1; b < i; b++){
            if(rez == 1)
                isFirst = 0;
            rez = (rez * a) % j;
        }
        if(rez == 1 && isFirst){
            response.push_back(a);
        }
    }
    return response;
}

int cmmdc(int a, int b){
    int r = 1;
    while(r != 0){
        r = a % b;
        a = b; 
        b = r;
    }
    return a;
}

bool isPrime(int n){
    if(n == 1)
        return false;
    for(int i = 2; i * i <= n; i++){
        if(n % i == 0)
            return false;
    }
    return true;
}

map<int, int> decompose(int n){
    map<int, int> powers;
    int i = 2;
    while(n != 1){
        if(isPrime(i) && n % i == 0){
            n /= i;
            powers[i]++;
        }
        else{
            i++;
        }
    }
    return powers;
}

int phi(int n){
   map<int, int> decomposition = decompose(n);
   int res = 1;
   for(auto i : decomposition){
       res *= pow(i.first, i.second) - pow(i.first, i.second - 1);
   }
   return res;
}

int main(){
    int i, j;
    vector<int> response;
    cout << "X^i = +-1 mod j" << endl << "insert i, j: ";
    cin >> i >> j;
    int x;
    cout << "+1 sau -1?";
    cin >> x;
    if(x > 0){
        cout << endl << "Solutiile ec. sunt de forma alpha^i mod " << j << 
        ", unde alpha este radacina primitiva mod 17, si i apartine {k * phi(" << 
        j << ")/(" << i << ", phi(" << j << ")) | 0 <= k < (" << i << ", phi(" <<
        j << ")) }, phi(" << j << ") = " << phi(j) << endl << endl;

        cout << "Asadar, i apartine {" << phi(j) << "k/" << cmmdc(i, phi(j)) <<
        " | 0 <= k < " << cmmdc(i, phi(j)) << "} = {" << phi(j) / cmmdc(i, phi(j)) <<
        "k | 0 <= k < " << cmmdc(i, phi(j)) << " } = { ";
        for(int k = 0; k < cmmdc(i, phi(j)) - 1; k++){
            cout << k * phi(j) / cmmdc(i, phi(j)) << ", ";
        }
        cout << (cmmdc(i, phi(j)) - 1) * phi(j) / cmmdc(i, phi(j)) << "}." << endl << endl;
        
        cout << "Radacinile primitive modulo " << j << " sunt in numar de" << 
        " phi(phi(" << j << ")) = phi(" << phi(j) << ") = " << phi(phi(j)) <<
        " si anume {";
        response = getSol(phi(j), j);
        for(int i = 0; i < response.size() - 1; i++){
            cout << response[i] << ", ";
        }
        cout << response[response.size() - 1] <<
        "}, alpha poate fi orice valoare din aceasta multime" << endl;


        cout << "The actual solutions:" << endl;
        response = getSol(i, j);
        for(int i = 0; i < response.size(); i++){
            cout << response[i] << " ";
        }
        cout << endl;
    }
    else{
        cout << endl << "Solutiile ec. sunt de forma alpha^i mod " << j << 
        ", unde alpha este radacina primitiva mod " << j <<
        ", si i apartine { (" << j << "-1)/2(" << i << ", " << j << 
        "-1) + k(" << j << "-1)/(" << i << ", " << j << 
        "-1) | 0 <= k < (" << i << ", " << j << "-1) } = { " << j-1 <<
        "/" << 2 * cmmdc(i, j - 1) << " + k" << j - 1 << "/" << i <<
        " | 0 <= k < " << i << "}." << endl;

        cout << "Asadar, i apartine {" << (j - 1) / (2 * cmmdc(i, j - 1)) <<
        " + " << (j - 1) / i << "k | 0 <= k < " << i << "} = {";
        for(int k = 0; k < cmmdc(i, j-1) - 1; k++){
            cout << (j - 1) / (2 * cmmdc(i, j - 1)) + (j - 1) / i * k << ", ";
        }
        cout << (j - 1) / (2 * cmmdc(i, j - 1)) + (j - 1) / i * (cmmdc(i, j-1) - 1) << 
        "}." << endl << endl;

        cout << "Radacinile primitive modulo " << j << " sunt in numar de " << 
        "phi(phi(" << j << ")) = phi(" << phi(j) << ") = " << phi(phi(j)) << 
        " si anume {";
        response = getSol(phi(j), j);
        for(int i = 0; i < response.size() - 1; i++){
            cout << response[i] << ", ";
        }
        cout << response[response.size() - 1] <<
        "}, alpha poate fi orice valoare din aceasta multime" << endl;
    }
}