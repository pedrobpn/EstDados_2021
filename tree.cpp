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
        }

        ~Tree(){
            
            delete_recursive(root);
            dict.clear();
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
                if(n->leaf)
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
                if(n->leaf){
                    
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
            
            vector<pair<char, int>> serialize(Tree t, bool order){
            	vector<pair<char,int>> *pre;
                vector<pair<char,int>> *sim;





			}
            
};


int main(){

    string text = "Por favor, alguem me ve um pastel de queijo com presunto ao vivo no show do willson zuthan ao year 123! # salve nois krl *** uauauau";
    Tree* t = new Tree(text);
    string enc = t->encode(text);

    cout << "ORIGINAL: " << text << "\n";

    cout << "ENCODED: "<< enc << "\n";

    string dec = t->decode(enc);

    cout <<"DECODED: "<< dec << "\n";

    delete t;


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