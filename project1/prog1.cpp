#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include <algorithm>
#include <deque>

#define IDDFS_MAXDEPTH 2147483647
#define usageErr "./prog1 <initial state file> <goal state file> <mode> <output file>\n"
int counter = 0;
//allowed modes
char * modes[] = {"bfs",
                  "dfs",
                  "iddfs",
                  "astar"};
char * mode;
//structs representing a riverbank
struct bank_{
  int chickens;
  int wolves;
  int boat;
  int value(){
    return wolves + chickens + boat;
  }
} typedef bank_t;
//struct representing each state
struct state_{
  bank_t leftBank;
  bank_t rightBank;
  struct state_ *parent;
  std::deque<struct state_ *> children;
  int depth;
  int priority;

  inline bool operator==(struct state_ a) {
  	if (a.leftBank.wolves==leftBank.wolves && a.leftBank.chickens==leftBank.chickens && a.leftBank.boat==leftBank.boat && a.rightBank.wolves==rightBank.wolves && a.rightBank.chickens==rightBank.chickens && a.rightBank.boat==rightBank.boat)
    	return true;
    else
    	return false;
  }
} typedef state_t;

struct node_ {
	state_t state;
	state_t * parent;
  std::deque<struct node_ *> children;
  int priority;
  inline bool operator==(struct node_ a){
    if(a.state == state){
      return true;
    } else {
      return false;
    }
  }
} typedef node_t;
//initial and goal states gotten from files
state_t state_i, state_g;

struct action_{
  int c;
  int w;
} typedef action_t;

action_t actions[] = {
  {1, 0},
  {2, 0},
  {0, 1},
  {1, 1},
  {0, 2}
};

//std::deque<state_t*> Expand(state_t*);
void Expand(state_t *);
bool IsIn(std::deque<state_t*>, state_t*);
void PrintSolution(state_t *);
void GraphSearch();
bool IsValidState(state_t *);
void ReadFileIntoStruct(char *,state_t *);
void PrintState(state_t *);
void BFS();
void DFS();
//int RDLS(state_t *,int);
int DLS(int);
void IDDFS();
void ASTAR();
int heuristic(state_t *);
int maxElem(std::deque<state_t*> * d);

std::fstream of;

int main(int argc, char *argv[]) {
  //std::deque<state_t> fringe;
  //check for proper num args
  if(argc < 5){
    printf("Invalid Number of arguments\n");
    printf(usageErr);
    return 1;
  }
  ReadFileIntoStruct(argv[1], &state_i);
  ReadFileIntoStruct(argv[2], &state_g);
  state_i.depth = 0;
  mode = argv[3];
  of.open(argv[4], std::fstream::out);
  if(!strcmp(mode, modes[0])){
    //run bfs
    //GraphSearch();
    BFS();
    of.close();
    return 0;
  }
  if(!strcmp(mode, modes[1])){
    //run dfs
    DFS();
    of.close();
    return 0;
  }
  if(!strcmp(mode, modes[2])){
    //run iddfs
    IDDFS();
    of.close();
    return 0;
  }
  if(!strcmp(mode, modes[3])){
    //run astar
    ASTAR();
    of.close();
    return 0;
  }
  of.close();
  printf("Not a valid mode, choose 1 of the following: ");
  for (size_t i = 0; i < 4; i++) {
    printf("%s, ", modes[i]);
  }
  return 1;
}
void BFS(){
   std::deque<state_t*> fringe;
   std::deque<state_t*> closed;
   state_t* node;
   node = &state_i;
   fringe.push_back(node);
   while(true){
      if(fringe.empty()){printf("Could not find solution!\n");return;}
      node = fringe.front(); fringe.pop_front();
      if(*node == state_g){PrintSolution(node); return;}
        if(!IsIn(closed, node)){
        closed.push_back(node);
        Expand(node);
        counter++;
        //printf("%d\n", counter);
	for(int i=0;i<node->children.size();i++){
          fringe.push_back(node->children[i]);
        }
      }
   }
}
void DFS(){//
   std::deque<state_t*> fringe;
   std::deque<state_t*> closed;
   state_t* node;

   node = &state_i;
   fringe.push_back(node);
   while(true){
      if(fringe.empty()){printf("Could not find solution!\n");return;}
      node = fringe.front(); fringe.pop_front();
      if(*node == state_g){PrintSolution(node); return;}
        if(!IsIn(closed, node)){
	   closed.push_back(node);
	   Expand(node);
	   counter++;
	   //if(!(node->children.empty())){
	   for(int i=node->children.size()-1;i>0;i--){
	      fringe.push_front(node->children[i]);
	   }
        //}
      }
   }
}
int DLS(int lim){
   std::deque<state_t*> fringe;
   std::deque<state_t*> closed;
   state_t* node;

   node = &state_i;
   fringe.push_back(node);
   while(lim--){
      node = fringe.front(); fringe.pop_front();
      if(*node == state_g){PrintSolution(node); return 0;}
        if(!IsIn(closed, node)){
        closed.push_back(node);
        Expand(node);
        counter++;
	//printf("%d\n",counter);
//	if(!(node->children.empty())){
	   for(int i=node->children.size()-1;i>0;i--){
	      fringe.push_front(node->children[i]);
	   }
//       }
      }
   }
   return 1;
}

void IDDFS(){
  int result;
  for(int depth=1;depth<IDDFS_MAXDEPTH;depth++){
    //printf("Depth: %d\n\n", depth);
    result = DLS(depth);
    if(result == 0){
       return;
    }
  }
  printf("Could not find solution!\n");
  return;
}
void ASTAR(){
	std::deque<state_t*> closed;
  std::deque<state_t*> open;

  state_i.priority = heuristic(&state_i);
  open.push_front(&state_i);
  closed.push_front(&state_i);

	while(!open.empty()){
		int pos = maxElem(&open);
		state_t * current = open.at(pos);
		open.erase(open.begin() + pos);
		if (*current == state_g) {
			PrintSolution(current);
			return;
		}
		Expand(current);
		counter++;
		for (auto &child : current->children) {
			child->priority = heuristic(child);
			open.push_front(child);
			closed.push_front(child);
		}
  }
}
int maxElem(std::deque<state_t*> *d){
  int maxI = 0;
  int maxV = 0;
  for(int i = 0; i < d->size(); i++){
	  state_t *c = d->at(i);
    if(maxV < c->priority){
      maxV = c->priority;
      maxI = i;
    } else {
      continue;
    }
  }
  return maxI;
}
int heuristic(state_t *s){
  return state_g.leftBank.value() - s->leftBank.value();
}

//
void Expand(state_t* s){
  //std::deque<state_t*> successors;
  for (int i = 0; i < 5; i++) {
    state_t * st = new state_t;
    *st = *s;
    st->parent = s;
    st->depth += 1;

    bank_t *curBank = st->leftBank.boat ? &st->leftBank : &st->rightBank;
    bank_t *otherBank = st->leftBank.boat ? &st->rightBank : &st->leftBank;
    curBank->chickens -= actions[i].c;
    otherBank->chickens += actions[i].c;
    curBank->wolves -= actions[i].w;
    otherBank->wolves += actions[i].w;
    curBank->boat = 0;
    otherBank->boat = 1;

    //do not include parent state
    if(s->parent != NULL){
      if(*s->parent == *st){
        delete st;
        continue;
      }
    }
    st->priority = heuristic(st);

    if(IsValidState(st)){
      s->children.push_back(st);
    } else {
      delete st;
    }
  }
}/**/
/*/
void Expand(state_t *s){
  state_t *arr[5];
  //copy the current state, to preserve the original
  state_t st = *s;
  //get the bank the boat is on
  bank_t *curBank = s->leftBank.boat ? &s->leftBank : &s->rightBank;
  bank_t *otherBank = s->leftBank.boat ? &s->rightBank : &s->leftBank;
  //Put one chicken in the boat/
  curBank->chickens -= 1;
  otherBank->chickens += 1;
  curBank->boat = 0;
  otherBank->boat = 1;
  state_t *a = new state_t;
  *a = *s;
  a->parent = s;
  arr[0] = a;
  *s = st;
  //Put two chickens in the boat/
  curBank->chickens -= 2;
  otherBank->chickens += 2;
  curBank->boat = 0;
  otherBank->boat = 1;
  state_t *b = new state_t;
  *b = *s;
  b->parent = s;
  arr[1] = b;
  *s = st;
  //Put one wolf in the boat/
  curBank->wolves -= 1;
  otherBank->wolves += 1;
  curBank->boat = 0;
  otherBank->boat = 1;
  state_t *c = new state_t;
  *c = *s;
  c->parent = s;
  arr[2] = c;
  *s = st;
  //Put one wolf and one chicken in the boat/
  curBank->wolves -= 1;
  otherBank->wolves += 1;
  curBank->chickens -= 1;
  otherBank->chickens += 1;
  curBank->boat = 0;
  otherBank->boat = 1;
  state_t *d = new state_t;
  *d = *s;
  d->parent = s;
  arr[3] = d;
  *s = st;
  //Put two wolves in the boat/
  curBank->wolves -= 2;
  otherBank->wolves += 2;
  curBank->boat = 0;
  otherBank->boat = 1;
  state_t *e = new state_t;
  *e = *s;
  e->parent = s;
  arr[4] = e;
  *s = st;
  for (size_t i = 0; i < 5; i++) {
    if(IsValidState(arr[i])){
      s->children.insert(s->children.end(),arr[i]);
    } else {
      delete arr[i];
    }
  }
}/**/

bool IsValidState(state_t *state){
  //if any bank has negative animals it is invalid
  if( state->leftBank.chickens < 0 ||
      state->leftBank.wolves < 0 ||
      state->rightBank.chickens < 0 ||
      state->rightBank.wolves < 0){
      return false;
  }
  //apply rule: chickens must be greater than wolves if chickens exist
  if(state->leftBank.chickens > 0){
    if(state->leftBank.wolves > state->leftBank.chickens && state->leftBank.boat == 0){
      return false;
    }
  }
  if(state->rightBank.chickens > 0){
    if(state->rightBank.wolves > state->rightBank.chickens && state->rightBank.boat == 0){
      return false;
    }
  }
  return true;
}
void ReadFileIntoStruct(char *fpath, state_t *s){
  //check for file access
  std::fstream f;
  f.open(fpath, std::fstream::in);
  if(!f.is_open()){
    printf("Could not open file\n");
    printf(usageErr);
    exit(1);
  }
  //read files contents
  std::string line;
  int val[6];
  int i = 0;
  while (getline(f,line)) {
    std::stringstream ls(line);
    std::string v;
    while(getline(ls,v, ',')){
      val[i++] = atoi(v.c_str());
    }
  }
  f.close();
  //pass contents to struct
  *s = {{val[0],val[1],val[2]},
        {val[3],val[4],val[5]},
        NULL};
}
void PrintState(state_t *s){
  printf("%d,%d,%d\n%d,%d,%d\n\n",
        s->leftBank.chickens,s->leftBank.wolves,s->leftBank.boat,
        s->rightBank.chickens,s->rightBank.wolves,s->rightBank.boat);

    of << s->leftBank.chickens << "," << s->leftBank.wolves << "," << s->leftBank.boat << "\n" << s->rightBank.chickens << "," << s->rightBank.wolves << "," << s->rightBank.boat << "\n\n";
}

//check if a node is in the closed list
bool IsIn(std::deque<state_t*> c, state_t *n){
  for(int i=0;i<c.size();i++){
    if(*n == *c[i]){return true;}
  }
  return false;
}

void PrintSolution(state_t *s){
  int sCounter = 0;
  std::deque<state_t*> sol;
  while(!(*s == state_i)){
    sCounter++;
    sol.push_front(s);
    s = s->parent;
  }
  sol.push_front(s);
  std::for_each(sol.begin(), sol.end(), PrintState);
  printf("Number of nodes in Solution: %d\n", sCounter);
  of << "Number of nodes in Solution: " << sCounter << "\n";
  printf("Total number of nodes expanded: %d\n", counter);
  of << "Total number of nodes expanded: " << counter << "\n";

  return;
}

void GraphSearch(){
   std::deque<state_t*> fringe;
   std::deque<state_t*> closed;
   state_t* node;

   node = &state_i;
   fringe.push_back(node);
   while(true){
      if(fringe.empty()){printf("Could not find solution!\n");return;}
      node = fringe.front(); fringe.pop_front();
      if(*node == state_g){PrintSolution(node); return;}
        if(!IsIn(closed, node)){
        closed.push_back(node);
        Expand(node);
        counter++;
	for(int i=0;i<node->children.size();i++){
          fringe.push_back(node->children[i]);
        }
      }
   }
}
