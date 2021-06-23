// Kartikeya Kansal https://github.com/kartikeyakansal/SoBTC

#include<bits/stdc++.h>
using namespace std;

long long totalBlockWeight = 4000000; 
set<string> included_trx_ids;

class Transaction{
    public:
        string transaction_ids;
        int fees;
        int weight;
        vector<string> parent_ids;
};

pair<string, Transaction*> addTransaction(vector<string>& arr){
    
    auto transaction = new Transaction();
    transaction->transaction_ids = arr[0]; 
    transaction->fees = stoi(arr[1]); 
    transaction->weight = stoi(arr[2]);
    vector<string> pid;
    for (int i = 3; i < arr.size(); i++){
        pid.push_back(arr[i]);
    }
    transaction->parent_ids = pid;
    return {arr[0], transaction};

}


void read_csv_file(unordered_map<string, Transaction*>& transactions){
    
    ifstream fin("mempool.csv");
    vector<string> arr;
    string line,sub;
    getline(fin,line);
    while(getline(fin,line)){
        arr.clear();
        stringstream s(line);
        while(getline(s,sub,',')){
            arr.push_back(sub);
        }
        pair<string,Transaction*> pair1 = addTransaction(arr);
        transactions[pair1.first] = pair1.second;
    }
    fin.close();

    cout << "Total Transactions : " << transactions.size() << endl;

}

bool isParentIncluded(Transaction* trx,set<string>& included_trx_ids){
    for(auto parent : trx->parent_ids){
        if(included_trx_ids.find(parent) == included_trx_ids.end())
            return false;
    }
    return true;
}

void PrintOut(vector<string>& output, string filename){
    ofstream myfile(filename);
    for(auto s:output){
        myfile << s << endl;
    }
    myfile.close();
}

int main(){
    unordered_map<string, Transaction*> transactions;
    read_csv_file(transactions);


    set<pair<float, Transaction *>, greater<pair<float, Transaction *>>> Set_transaction; 
    set<string> included_trans_set; 
    vector<string> output; 
    for(auto i:transactions){
        Set_transaction.insert({(float)i.second->fees / (float)i.second->weight, i.second});
    }
    int BlockWeight = 0;
    int totalFee = 0;
    while(!Set_transaction.empty() && (BlockWeight < totalBlockWeight)){
        bool found = false;
        for(auto itr = Set_transaction.begin(); itr != Set_transaction.end(); itr++){
            Transaction* currentTransaction = (*itr).second;
            int currFee = currentTransaction->fees;
            int currWeight = currentTransaction->weight;
            if(isParentIncluded(currentTransaction, included_trx_ids) && BlockWeight + currWeight <= totalBlockWeight){
                BlockWeight += currWeight;
                included_trans_set.insert(currentTransaction->transaction_ids);
                output.push_back(currentTransaction->transaction_ids);
                totalFee += currFee;
                Set_transaction.erase(itr);
                found = true;
                break;
            }
        }
        if(!found){
            break;
        }
    }
    cout << "Number of transactions: " << included_trans_set.size() << endl;
    cout << "Total fee: " << totalFee << endl;
    cout << "Total weight: " << BlockWeight << endl;
    PrintOut(output, "block.txt");

    return 0;
}