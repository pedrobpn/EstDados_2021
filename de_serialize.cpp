#include <iostream>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <bits/stdc++.h>

#define NODEtoNODE '|'
#define VALtoFREQ '\\'
#define ValNotLeaf '*'

using namespace std;


class Node{

    public:
        char val;
        int freq;
        bool leaf;
        Node* right;
        Node* left;

        // Construtor completo
        Node(char val, int freq, bool leaf, Node* right, Node* left) : val(val), freq(freq), leaf(leaf), right(right), left(left){}
        // Construtor default
        Node(){
            val = ValNotLeaf;
            freq = 0;
            leaf = false;
            right = nullptr;
            left = nullptr;
        }
        // Construtor para folhas
        Node(char val, int freq) : val(val), freq(freq){

            leaf = true;
            right = nullptr;
            left = nullptr;
        }
        //Construtor para nos intermediarios
        Node(int freq) : freq(freq){

            val = ValNotLeaf;
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
        vector<pair<char,int>> pre_order;
        vector<pair<char,int>> sim_order;

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

            unordered_map<char, int> freq_chars; //map letra -> frequencia

            // Calcular frequencias de cada letra
            for(auto itr = text.begin(); itr != text.end(); ++itr)
                freq_chars[*itr]++;
            
            // Alocar Nodes e colocar na priority_queue
            for(auto itr = freq_chars.begin(); itr != freq_chars.end(); ++itr){

                Node* aux = new Node(itr->first, itr->second);
                pq.push(aux);
            }

            // Construir a arvore
            while(pq.size() > 1){

                // Pegar os dois menores no heap
                Node* least1 = pq.top();
                pq.pop();
                Node* least2 = pq.top();
                pq.pop();

                // Criar nó intermediario (não é folha)
                Node* inter = new Node(least1->freq + least2->freq);
                inter->left = least1;
                inter->right = least2;

                // Colocar no intermediario no heap
                pq.push(inter);
            }

            // O que sobrar no heap será o root
            root = pq.top();
            pq.pop();

            //Construir o dicionario
            dict = unordered_map<char, string>();
            make_dict(root, "");
            serialize();
        }

        Tree(vector<pair<char, int>> pre, vector<pair<char, int>> sim){

            pre_order = vector<pair<char, int>>(pre);
            sim_order = vector<pair<char, int>>(sim);
            deserialize();
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
            map<pair<char, int>, int> sim_order_map;
            for(int i = 0; i<sim_order.size(); i++){
                sim_order_map[sim_order[i]] =i; 
            }
                
            Node* aux = deserialize_recursive(sim_order_map, 0, pre_order.size()-1, pre_order_idx);
            root = aux;
        }

        private:

            // Sub rotina para montar o dicionario 
            void make_dict(Node* n, string s){

                // Retornar se achar um no nulo
                if(n == nullptr)
                    return ;
                
                // Se for uma folha, colocar no dicionario
                if(n->leaf)
                    dict[n->val] = (s.empty())? "1":s;                    

                // Continuando o dfs
                make_dict(n->left, s + "1");
                make_dict(n->right, s + "0");                
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
            void serialize_recursive(Node* n, vector<pair<char, int>>& pre_order, vector<pair<char, int>>& sim_order){

                if(n == nullptr) // Retornar se a arvore for vazia
                    return ;
                
                pre_order.push_back({n->val, n->freq});
                serialize_recursive(n->left, pre_order, sim_order);
                sim_order.push_back({n->val, n->freq});
                serialize_recursive(n->right, pre_order, sim_order);
            }

            // Recursao para o deserialize
            Node* deserialize_recursive(map<pair<char, int>, int>& sim_order_map, int left, int right, int& pre_order_idx){

                if(left > right)
                    return nullptr;
                
                pair<char, int> rootValue = pre_order[pre_order_idx++];
                Node* node = new Node(rootValue.first, rootValue.second);

                node->left = deserialize_recursive(sim_order_map, left, sim_order_map[rootValue] - 1, pre_order_idx);
                node->right = deserialize_recursive(sim_order_map, sim_order_map[rootValue] + 1, right, pre_order_idx);
                return node;
            }
};


int main(){

    string text = "aaabbbbbcccdddddddee gh";
    Tree* t = new Tree(text);
    t->serialize();

    Tree* t2 = new Tree(t->pre_order, t->sim_order);
    t2->serialize();


    

 //   Tree* desr = Tree(t->serialize());

    return 0;
}