
#include <iostream>
#include <string>
#include <cstdlib>
#include <time.h>
#include "doublelinklist.h"

using namespace std;

struct attack {   // class for pokemon attacks
private:
    string name;
    int pp;
    int accuracy;
    int damage;
    int first;
public:
    attack(const string&, int, int, int, int);
    int get_first();
    int get_accuracy();
    int get_damage();
    int get_pp();
    string get_name();
};

attack::attack(const string& name_in, int p, int a, int d, int f) {
    name = name_in;
    pp = p;
    accuracy = a;
    damage = d;
    first = f;
}

string attack::get_name() {
    return name;
}

int attack::get_pp() {
    return pp;
}

int attack::get_damage() {
    return damage;
}

int attack::get_first() {
    return first;
}

int attack::get_accuracy() {
    return accuracy;
}

struct pokemon {  // class for pokemons
    int pp;
    int hp;
    string name;
    DoublyList<attack*> attacks;
    pokemon(const string& name_in, int, int);
    string get_name();
    void set(char, int to_reduce);
};

pokemon::pokemon(const string& name_in, int p, int h) {
    name = name_in;
    pp = p;
    hp = h;
}


void pokemon::set(char which, int to_reduce) {
    if (which == 'p') {
        pp += to_reduce;
    } else {
        hp -= to_reduce;
    }
}

struct node {  // class for the nodes of the graph
    string ifef;  // attack is efficient or nonefficient
    string curattack;  // attack name
    int num;  // node num
    pokemon* pikachu;  // current pikachu
    pokemon* blastoise;  // current blastoise
    char status;  // whose turn?
    int level;  // level of the node
    bool isleaf;  // node is a leaf or not
    double prob;  // prob of the node happening
	node* parent;

    DoublyList<node*> child;  // children nodes of the node

    //Member functions go here...
    node(string ifef, string curattack, int num, pokemon* pikachu,pokemon* blastoise,char status,int level,bool isleaf,double prob,node* parent) {
       this->ifef= ifef;
       this->curattack= curattack;
       this->num=num;
       this->pikachu=pikachu;
       this->blastoise = blastoise;
       this-> status = status;
       this->level=level;
       this->isleaf=isleaf;
       this->prob=prob;
       this->parent=parent;
    }
    ~node() {
        for (int i = 0; i < child.elemcount; i++) {
            delete child.get(i);
        }
    }

};

struct graph {
    //Graph definitions go here...
    int levelCount = 0;
    node* root;
    graph() {
        root = nullptr;
        levelCount=0;
    }
    void DFS(node*, int);
    void recursive_DFS(node*, int, DoublyList<node*>&);
    void shortPath_BFS(node*, char);
    void BFS(node*, int);
};

float calculProb(node* node, pokemon* pokemon) {
    float  total_attacks= 0;
    float prob_General = node->prob;
    for (int i = 0; i < pokemon->attacks.elemcount; i++) {
        if (pokemon->attacks.get(i)->get_first() <= node->level + 1 && pokemon->attacks.get(i)->get_pp() <= pokemon->pp) total_attacks++;
    }

    return   prob_General / total_attacks;
}


void constChild(node* Node, pokemon* pikachu, pokemon* blastoise, int maxLevel) {
    if (Node->level == maxLevel) return;
        
    if (Node->status == 'p') {//pikachu attacking
    
         float effectiveProbability = calculProb(Node, Node->pikachu);
        for (int i = 0; i < pikachu->attacks.elemcount; i++) {
            

            if (pikachu->attacks.get(i)->get_first() <= Node->level + 1 && (pikachu->attacks.get(i)->get_pp() * (-1)) <= Node->pikachu->pp && Node->pikachu->hp > 0) {
                
                if (pikachu->attacks.get(i)->get_accuracy() != 100) {
                    pokemon* new_pikachu = new pokemon(*(Node->pikachu));
                    pokemon* new_blas = new pokemon(*(Node->blastoise));
                    
                    node* ineffectiveChild = new node(Node->ifef, Node->curattack, Node->num, new_pikachu,new_blas,Node->status,Node->level,Node->isleaf,Node->prob,Node->parent);
                    ineffectiveChild->level = Node->level + 1;
                    ineffectiveChild->status = 'b';
                    ineffectiveChild->ifef = "not effective";
                   
                    ineffectiveChild->pikachu->pp = Node->pikachu->pp + ineffectiveChild->pikachu->attacks.get(i)->get_pp();
                    
                    ineffectiveChild->curattack = pikachu->attacks.get(i)->get_name();
                     
                    ineffectiveChild->prob = effectiveProbability - (effectiveProbability * pikachu->attacks.get(i)->get_accuracy() / 100);
                    if (Node->level + 1 == maxLevel) ineffectiveChild->isleaf = true;
                    ineffectiveChild->parent = Node;
					Node->child.addBack(ineffectiveChild);
                    constChild(ineffectiveChild, pikachu, blastoise, maxLevel);
                }
                

                node* effectiveChild = new node(Node->ifef, Node->curattack, Node->num, pikachu,blastoise,Node->status,Node->level,Node->isleaf,Node->prob,Node->parent);
                effectiveChild->level = Node->level + 1;
                effectiveChild->status = 'b';
                effectiveChild->ifef = "effective";
                
                effectiveChild->pikachu->pp = Node->pikachu->pp + effectiveChild->pikachu->attacks.get(i)->get_pp();
                
                effectiveChild->blastoise->hp = Node->blastoise->hp - effectiveChild->pikachu->attacks.get(i)->get_damage();
                effectiveChild->curattack = pikachu->attacks.get(i)->get_name();
                if (pikachu->attacks.get(i)->get_accuracy() != 100) effectiveChild->prob = effectiveProbability * pikachu->attacks.get(i)->get_accuracy() / 100;
                else effectiveChild->prob = effectiveProbability;
                if (Node->level + 1 == maxLevel) effectiveChild->isleaf = true;
                effectiveChild->parent = Node;
				Node->child.addBack(effectiveChild);
                constChild(effectiveChild, pikachu, blastoise, maxLevel);
            }
        }
    } else {//blastoise attaacking
        
        float effectiveProbability = calculProb(Node, blastoise);

        for (int i = 0; i < blastoise->attacks.elemcount; i++) {
            
            
            if (blastoise->attacks.get(i)->get_first() <= Node->level + 1 && (blastoise->attacks.get(i)->get_pp() * (-1)) <= Node->blastoise->pp && Node->blastoise->hp > 0) {
                if (blastoise->attacks.get(i)->get_accuracy() != 100) {
                    pokemon* new_pikachu = new pokemon(*(Node->pikachu));
                    pokemon* new_blas = new pokemon(*(Node->blastoise));
                    node* ineffectiveChild = new node(Node->ifef, Node->curattack, Node->num, new_pikachu,new_blas,Node->status,Node->level,Node->isleaf,Node->prob,Node->parent);
                    ineffectiveChild->level = Node->level + 1;
                    ineffectiveChild->ifef = "not effective";
                    ineffectiveChild->status = 'p';
                    ineffectiveChild->blastoise->pp = Node->blastoise->pp - ineffectiveChild->blastoise->attacks.get(i)->get_pp();
                    ineffectiveChild->curattack = blastoise->attacks.get(i)->get_name();
                    ineffectiveChild->prob = effectiveProbability - (effectiveProbability * blastoise->attacks.get(i)->get_accuracy() / 100);
                    if (Node->level + 1 == maxLevel) ineffectiveChild->isleaf = true;
                    ineffectiveChild->parent = Node;
					Node->child.addBack(ineffectiveChild);
                    constChild(ineffectiveChild, pikachu, blastoise, maxLevel);
                }
                
                node* effectiveChild = new node(Node->ifef, Node->curattack, Node->num, pikachu,blastoise,Node->status,Node->level,Node->isleaf,Node->prob,Node->parent);
                effectiveChild->level = Node->level + 1;
                effectiveChild->ifef = "not effective";
                effectiveChild->status = 'p';
                effectiveChild->blastoise->pp = Node->blastoise->pp - effectiveChild->blastoise->attacks.get(i)->get_pp();
                effectiveChild->pikachu->hp = Node->pikachu->hp - effectiveChild->blastoise->attacks.get(i)->get_damage();
                effectiveChild->curattack = blastoise->attacks.get(i)->get_name();
                if (blastoise->attacks.get(i)->get_accuracy() != 100) effectiveChild->prob = effectiveProbability * blastoise->attacks.get(i)->get_accuracy() / 100;
                effectiveChild->prob = effectiveProbability;
                if (Node->level + 1 == maxLevel) effectiveChild->isleaf = true;
                effectiveChild->parent = Node;
				Node->child.addBack(effectiveChild);
                constChild(effectiveChild, pikachu, blastoise, maxLevel);
            }
        }
    }
}
void buildGraph(graph* graph, pokemon* pikachu, pokemon* blastoise, int graphLevel) {
    node* Node = new node("", "", 0, pikachu,blastoise,'p',0,0,1,NULL);
    
    graph->root = Node;
    while(Node->pikachu->hp >=0 && Node->blastoise->hp >=0 && Node->blastoise->pp >=0 && Node->pikachu->pp >=0 )
     {constChild(Node , pikachu, blastoise, graphLevel);
    graph->levelCount = graphLevel;}
}

void p1output(node* node, int maxLevel) {
    if (node == nullptr)
			return;
    if (maxLevel == 0) cout << "P_HP:" << node->pikachu->pp << " P_PP:" << node->pikachu->pp << " B_HP:" << node->blastoise->hp << " B_PP:" << node->blastoise->pp << " PROB:" << node->prob << endl;
    else {
        for (int i = 0; i < node->child.elemcount; i++)
            p1output(node->child.get(i), maxLevel-1);
    } 
}

void graph::BFS(node* Node, int maxLevel) {
    DoublyList<node*> remains;
    DoublyList<node*> visited;
    
    bool MaxLevelReached = false;

    
    visited.addBack(Node);
    for (int i = 0; i < Node->child.elemcount; i++) {
        remains.addBack(Node->child.get(i));
    }

    while (remains.elemcount != 0 && MaxLevelReached == false) {
        for (int i = 0; i < remains.get(0)->child.elemcount; i++) {
            remains.addBack(remains.get(0)->child.get(i));
            if (remains.get(0)->child.get(i)->level > maxLevel) {
                MaxLevelReached = true;
                break;
            }
        }

        if (MaxLevelReached == false) {
            visited.addBack(remains.get(0));
            remains.removeFront();
        }
    }
    
    cout << "Number of nodes: " << visited.elemcount <<endl;
}





void graph::shortPath_BFS(node* Node, char turn) {
    DoublyList<node*> remains;
    DoublyList<node*> attackSequence;
    node* required = Node;
    node* temp = required;

    if (Node->status == 'p') {
        for (int i = 0; i < Node->child.elemcount; i++) {
            remains.addBack(Node->child.get(i));
        }
        while (remains.elemcount != 0) {
            if (remains.get(0)->blastoise->hp <= 0) {
                required = remains.get(0);
                break;
            }
            for (int i = 0; i < remains.get(0)->child.elemcount; i++)
                remains.addBack(remains.get(0)->child.get(i));

            remains.removeFront();
        }
    }

    else {
        for (int i = 0; i < Node->child.elemcount; i++) {
            remains.addBack(Node->child.get(i));
        }
        while (remains.elemcount != 0) {
            if (remains.get(0)->pikachu->hp <= 0) {
                required = remains.get(0);
                break;
            }
            for (int i = 0; i < remains.get(0)->child.elemcount; i++)
                remains.addBack(remains.get(0)->child.get(i));

            remains.removeFront();
        }
    }

    attackSequence.addBack(required);
    temp = required;
    for (int i = 0; i < required->level; i++) {
        attackSequence.addBack(temp->parent);
        temp = temp->parent;
    }
    unsigned long size = attackSequence.elemcount;
    attackSequence.removeBack();
    for (int i = 1; i < size; i++) {
        node* print = attackSequence.get(attackSequence.elemcount - 1);
        string attacker;
        string isEffective;
        if (print->status == 'b') attacker = "Pikachu";
        else attacker = "Blastoise";
        if (print->ifef == "effective") isEffective = "effective";
        else isEffective = "not effective";
        cout << attacker << " used " << print->curattack << ": " << isEffective << endl;
        attackSequence.removeBack();
    }
    cout << "Level Count: " << required->level << endl << "Probability: " << required->prob << endl;
}

int main(int argc, char** argv) {
   string part = argv[1];
    int max;
    string which;
    if (part == "part1") {
        which = argv[2];
        max = 10;
    } else if (part == "part2") {
        char* level = argv[2];
        max = atoi(level);
        which = argv[3];
    }
    else
        return -1;

    pokemon* pikachu = new pokemon("pikachu", 100, 200);
    pokemon* blastoise = new pokemon("blastoise", 100, 200);

    attack* thundershock = new attack("thundershock", -10, 100, 40, 0);
    attack* skullbash = new attack("skullbash", -15, 70, 50, 0);
    attack* slam = new attack("slam", -20, 80, 60, 0);
    attack* pskip = new attack("skip", 100, 100, 0, 3);
    pikachu->attacks.addBack(thundershock);
    pikachu->attacks.addBack(skullbash);
    pikachu->attacks.addBack(slam);
    pikachu->attacks.addBack(pskip);

    attack* tackle = new attack("tackle", -10, 100, 30, 0);
    attack* watergun = new attack("watergun", -20, 100, 40, 0);
    attack* bite = new attack("bite", -25, 100, 60, 0);
    attack* bskip = new attack("skip", 100, 100, 0, 3);
    blastoise->attacks.addBack(tackle);
    blastoise->attacks.addBack(watergun);
    blastoise->attacks.addBack(bite);
    blastoise->attacks.addBack(bskip);

    // Output the last layer's node information
    graph* myGraph = new graph();
    //buildGraph(myGraph, pikachu, blastoise, 8);
    //p1output(myGraph->root, 4);
    if (part == "part2") {
        buildGraph(myGraph, pikachu, blastoise, max);
        myGraph->shortPath_BFS(myGraph->root, myGraph->root->status);
    }
    
    else {
        buildGraph(myGraph, pikachu, blastoise, max);
        if (part == "part1") p1output(myGraph->root, max);
            
       
}

    return 0;
}
