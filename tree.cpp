#include <iostream>
#include <queue>
#include <unordered_map>
#include <fstream>

#include<bits/stdc++.h>

using namespace std;

class Node{

    public:
        int val;
        int freq;
        bool leaf;
        Node* right;
        Node* left;

        // Construtor completo
        Node(int val, int freq, bool leaf, Node* right, Node* left) : val(val), freq(freq), leaf(leaf), right(right), left(left){}
        // Construtor default
        Node(){
            val = '*';
            freq = 0;
            leaf = false;
            right = nullptr;
            left = nullptr;
        }
        // Construtor para folhas
        Node(int val, int freq) : val(val), freq(freq){

            leaf = true;
            right = nullptr;
            left = nullptr;
        }
        //Construtor para nos intermediarios
        Node(int freq) : freq(freq){

            val = '*';
            leaf = false;
            right = nullptr;
            left = nullptr;
        }
};

/*
http://huffman.ooz.ie/?text=oabbbb
GROUND TRUTH ^^^^
*/

class Tree{
    
    public:
        Node* root;
        Node* tree_itr;
        string internal_text;
        unordered_map<char, string> dict;
        vector<pair<int,int>> pre_order;
        vector<pair<int,int>> sim_order;

        // Constutor padrão para a codificação
        Tree(string text){

            // Armazenar texto
            internal_text = text;

            // Comparar Nodes com base na frequência, para um max-heap,
            // precisamos de uma função great para Node
            auto comp = [](Node* a, Node* b){
                return (a->freq > b->freq);
            };
            priority_queue<Node*, vector<Node*>, decltype(comp)> pq(comp);

            unordered_map<int, int> freq_chars; //map letra -> frequencia

            // Calcular frequencias de cada letra
            for(auto itr = text.begin(); itr != text.end(); ++itr)
                freq_chars[*itr]++;
            
            // Alocar Nodes e colocar na priority_queue
            for(auto itr = freq_chars.begin(); itr != freq_chars.end(); ++itr){

                Node* aux = new Node(itr->first, itr->second);
                pq.push(aux);
            }

            int leafy = 256;
            // Construir a arvore
            while(pq.size() > 1){

                // Pegar os dois menores no heap
                Node* least1 = pq.top();
                pq.pop();
                Node* least2 = pq.top();
                pq.pop();

                // Criar nó intermediario (não é folha)
                Node* inter = new Node(least1->freq + least2->freq);
                inter->val = leafy;
                inter->right = least1;
                inter->left = least2;

                // Colocar no intermediario no heap
                pq.push(inter);
                leafy++;
            }
            
            // O que sobrar no heap será o root
            root = pq.top();
            pq.pop();

            //Construir o dicionario
            dict = unordered_map<char, string>();
            make_dict(root, "");
        }

        Tree(vector<pair<int, int>> pre, vector<pair<int, int>> sim){

            pre_order = vector<pair<int, int>>(pre);
            sim_order = vector<pair<int, int>>(sim);

            deserialize();
            //Construir o dicionario
            dict = unordered_map<char, string>();
            make_dict(root, "");
        }

        ~Tree(){
            
            delete_recursive(root);
            dict.clear();
        }

        // Escrever em vector a pre ordem e ordem simetrica
       void serialize(){

            pre_order.clear();
            sim_order.clear();
            Node* n = root;
            serialize_recursive(n, pre_order, sim_order);
        }

        // Ler vector em pre ordem e ordem simetrica e gerar a arvore
        void deserialize(){

            int pre_order_idx = 0;
            map<pair<int, int>, int> sim_order_map;
            for(int i = 0; i<sim_order.size(); i++){
                sim_order_map[sim_order[i]] =i;
            }
                
            Node* aux = deserialize_recursive(sim_order_map, 0, pre_order.size()-1, pre_order_idx);
            root = aux;
        }

        string encode(string original){
            
            // Para cada letra, adicionar a codificacao
            string res = "";
            for(auto itr = original.begin(); itr != original.end(); ++itr)
                res += dict[*itr];
    
            return res;
        }

        string decode(string codified){

            string res = "";
            dfs_decode(root, codified, res, 0);
            return res;
        }

        private:

            // Sub rotina para montar o dicionario 
            void make_dict(Node* n, string s){

                // Retornar se achar um no nulo
                if(n == nullptr)
                    return ;
                
                // Se for uma folha, colocar no dicionario
                if(n->val <256)
                    dict[n->val] = (s.empty())? "1":s;                    

                // Continuando o dfs
                make_dict(n->left, s + "1");
                make_dict(n->right, s + "0");                
            }
            // Sub rotina para decodificar string
            void dfs_decode(Node* n, string& s, string& t, int i){

                // Retornar se achar no nulo
                if(n == nullptr)
                    return ;
    
                // Achou uma folha
                if(n->val < 256){
                    
                    // Adicionar char na string
                    t += n->val;

                    // Se não tiver terminado, setar n para a raiz novamente
                    if(i==s.length())
                        return ;
                    else
                        dfs_decode(root, s, t, i);
                    
                    return ;
                }
                
                // Escolher se vai para esquerda ou direita
                if(s[i] == '0')
                    dfs_decode(n->right, s, t, i+1);
                else
                    dfs_decode(n->left, s, t, i+1);
            }
            // Apagar todos os nós recursivamente
            void delete_recursive(Node* root){

                if(root == nullptr)
                    return ;
                
                // Apagar um nó somente quando apagarmos todos os filhos (post order)
                delete_recursive(root->left);
                delete_recursive(root->right);
                
                // Apagar root
                delete root;
                root = nullptr;
            }
            
            // Recursão para o serialize
            void serialize_recursive(Node* n, vector<pair<int, int>>& pre_order, vector<pair<int, int>>& sim_order){

                if(n == nullptr) // Retornar se a arvore for vazia
                    return ;

                pre_order.push_back({n->val, n->freq});
                serialize_recursive(n->left, pre_order, sim_order);
                sim_order.push_back({n->val, n->freq});
                serialize_recursive(n->right, pre_order, sim_order);
            }

            // Recursao para o deserialize
            Node* deserialize_recursive(map<pair<int, int>, int>& sim_order_map, int left, int right, int& pre_order_idx){

                if(left > right)
                    return nullptr;

                pair<int, int> rootValue = pre_order[pre_order_idx++];
                Node* n = new Node(rootValue.first, rootValue.second);
            
                n->left = deserialize_recursive(sim_order_map, left, sim_order_map[rootValue] - 1, pre_order_idx);
                n->right = deserialize_recursive(sim_order_map, sim_order_map[rootValue] + 1, right, pre_order_idx);
                return n;
            }          
};

string vec_to_string(vector<pair<int, int>>& v){

    string res = "";

    for(auto u: v)
        res = res + to_string(u.first) + "," + to_string(u.second) + ',';
    return res;
}

vector<pair<int, int>> string_to_vec(string& s){

    vector<pair<int, int>> res;

    int pos = 0;
    while ((pos = s.find(",")) != string::npos) {

        pair<int, int> partial;

        string token = s.substr(0, pos);
        partial.first = stoi(token);
        s.erase(0, pos + string(",").length());

        pos = s.find(",");
        token = s.substr(0, pos);
        partial.second = stoi(token);
        s.erase(0, pos + string(",").length());

        res.push_back(partial);
    }

    return res;
}



int main(){

    int option = 0;

    cout << "Digite 1 para codificar e 2 para decodificar: \n";
    cin >> option;

    if(option==1){

        std::ifstream in("input.txt");
        std::stringstream buffer;
        buffer << in.rdbuf();
        string text = buffer.str();
        in.close();

        Tree* t = new Tree(text);
        string enc  = t->encode(text);
        t->serialize();
        vector<pair<int, int>> pre_order = t->pre_order;
        vector<pair<int, int>> sim_order = t->sim_order;

        string pre_s = vec_to_string(pre_order);
        string sim_s = vec_to_string(sim_order);

        ofstream tree_file("arvhuf.txt");
        ofstream text_file("texto.hfm");
        tree_file << pre_s << "\n" << sim_s;
        text_file << enc;
        tree_file.close();
        text_file.close();

        // Escrever no arquivo .ctx
        ofstream Fctx("binary.ctx");

        int i=0;
        for(i=0; i < text.size(); i++)
            Fctx << bitset<8>(text[i]).to_string();
        Fctx.close();

        delete t;
    }
    else if(option==2){

        ifstream tree("arvhuf.txt");
        string pre_s;
        string sim_s;
        
        getline(tree, pre_s);
        getline(tree, sim_s);
        tree.close();

        vector<pair<int, int>> pre_order;
        vector<pair<int, int>> sim_order;

        pre_order = string_to_vec(pre_s);
        sim_order = string_to_vec(sim_s);

        Tree* t = new Tree(pre_order, sim_order);

        ifstream text_file("texto.hfm");
        string enc;
        getline(text_file, enc);
        text_file.close();

        ofstream output_file("saida.txt");
        output_file << t->decode(enc);
        output_file.close();

        delete t;
    }
    else
        return 0;
    
    


}


/*

Métodos de Tree 

serialize() -> gerar dois vetores -> 1) percorrer arvore em ordem simetrica 2) Percorrer em pre ordem 3) vector<pair<char, int>> se 
for nó intermediario, colocar *

deserialize() -> pegar os dois vetores e gerar a arvore -> usar make_dict() para preecnher o dicionario e criar um construtor extra para isso Tree(string inorder, string preorder)

https://www.geeksforgeeks.org/construct-tree-from-given-inorder-and-preorder-traversal/

Tansformar vetor de 1 em:
a\1|b\5|c\3
a\1|b\5|c\3

TRansformar a\1|b\5|c\3 em vetor

Ler as strings do arquivo e fazer a arvore
Escrever string no arquivo


*/