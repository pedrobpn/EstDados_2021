#include <iostream>
#include <queue>
#include <unordered_map>
#include <fstream>

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
            val = '*';
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
                inter->right = least1;
                inter->left = least2;

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

        ~Tree(){
            
            delete_recursive(root);
            dict.clear();
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
            
            vector<pair<char, int>> serialize(){
            	vector<pair<char,int>> *pre = new vector<pair<char,int>>;
                vector<pair<char,int>> *sim = new vector<pair<char,int>>;

                Node* n = root;

                *pre = serialize_rec(n, pre, false);
                *sim = serialize_rec(n, sim, true);

                for (auto it = pre->begin(); it != pre->end(); ++it)
                    cout << it->first << "/" << it->second << "-";

                if(n == nullptr) // Retornar se a arvore for vazia
                    return *pre; // Ajeitar return depois
                
                return *pre; // Ajeitar return depois
            }
            
            vector<pair<char, int>> serialize_rec(Node* n, vector<pair<char, int>>* vet, bool order){

                if(n == nullptr) // Retornar se a arvore for vazia
                    return *vet; // Retorna o vector recebido
                
                if(n->leaf)
                    vet->push_back({n->val, n->freq});
                else{
                    serialize_rec(n->left, vet, order);
                    if(!order){ // Realiza a pré-ordem
                        serialize_rec(n->right, vet, order);
                        vet->push_back({n->val, n->freq});
                    }else{ // Realiza a ordem simétrica
                        vet->push_back({n->val, n->freq});
                        serialize_rec(n->right, vet, order);
                    }
                }
                
                return *vet;
            }
};


int main(){

    string text = "Por favor, alguem me ve um pastel de queijo com presunto";
    Tree* t = new Tree(text);

    //t->serialize();

    return 0;
}