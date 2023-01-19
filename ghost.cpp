#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <chrono>

using namespace std;
using namespace chrono;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

//data structure idea
//one array of factories with their properties
//one list of troops with their properties
//one array of lists of links (index 0 contains a list of all links of factory 0)
// or 2d array

//initial algo : first turn is simple, all soldiers to closest 3 prod non ally node
//then ? attack will all troops on nearest non ally node

//next version : more attacks + bombs
//amélioration : attaquer plus faible que soit

//FIX : STARTING NODE OF SOURCE (ALLY)

class link {
public:
	int factory;
	int distance;

	link(int f, int d) {
		this->factory = f;
		this->distance = d;
	}

	void showLink() {
		cerr << " factory2:" << factory << " distance:" << distance << endl;
	}
};

class factory {
public:
	int player; //1 for player, -1 for enemy, 0 for neutral
	int cyborgs_count;
	int prod; //production between 0 and 3

	factory(int _player, int _cyborgs, int _prod) {
		this->player = _player;
		this->cyborgs_count = _cyborgs;
		this->prod = _prod;
	}

	void showFactory(int id) {
		cerr << "id:" << id << " player:" << player << " cyborgs:" << cyborgs_count << " prod:" << prod << endl;
	}
};

class troop {
public:
	int id;
	int player; //1 for player, -1 for enemy
	int source; //id of source factory
	int destination; //id of destination factory
	unsigned int power; //number of cyborgs, >0
	unsigned int time; //number of turns before destination, >0
	bool alive;

	troop(int _id, int _player, int _source, int _destination, unsigned int _power, unsigned int _time, bool _alive) {
		this->id = _id;
		this->player = _player;
		this->source = _source;
		this->destination = _destination;
		this->power = _power;
		this->time = _time;
		this->alive = _alive;
	}

	void showTroop() {
		cerr << "id:" << id << " player:" << player << " source:" << source;
		cerr << " destination:" << destination << " power:" << power << " time:" << time << endl;
	}
};

bool isTroopInList(int id, list<troop> *troops, list<troop>::iterator res_it) {
	if (troops->empty()) return false;
	cerr << endl << "not empty " << id << ' ' << troops->begin()->id;
	cerr << ' ' << (troops->begin()->id == troops->end()->id);
	for (auto it = troops->begin(); it != troops->end(); it++) {
		cerr << endl << (*it).id;
		if ((*it).id == id) {
			res_it = it;
			cerr << " before return ";
			return true;
		}
		cerr << "end for istroop" << endl;
	}
	return false;
}

list<troop>::iterator TroopInList(int id, list<troop> *troops) {
	if (troops->empty()) return troops->end();
	for (auto it = troops->begin(); it != troops->end(); it++) {
		if ((*it).id == id) {
			return it;
		}
	}
	return troops->end();
}

//delete all dead troops
void killTroops(list<troop> *l) {
	for (auto it = l->begin(); it != l->end(); it++) {
		if ((*it).alive == false) (*it).~troop();
	}
}

//set alive parameter of all troops to false at the end of a turn
void setAlive(list<troop> *l) {
	for (auto it = l->begin(); it != l->end(); it++) {
		(*it).alive = false;
	}
}

void showFactories(factory *f, int count) {
	for (int i = 0; i < count; i++) {
		f[i].showFactory(i);
	}
}

void showLinks(list<link> *l, int count) {
	for (int i = 0; i < count; i++) {
		for (auto it = l[i].begin(); it != l[i].end(); it++) {
			cerr << "factory1:" << i << ' ';
			(*it).showLink();
		}
	}
}

void showTroops(list<troop> t) {
	if (t.empty()) return;
	for (auto it = t.begin(); it != t.end(); it++) {
		(*it).showTroop();
	}
}

int closestProdNode(int id, list<link> *links, factory *f) {
	int dist_min = 21;
	int id_ret = -1;
	for (auto it = links[id].begin(); it != links[id].end(); it++) {
		if (f[(*it).factory].player != 1 && f[(*it).factory].prod != 0 && (*it).distance < dist_min) {
			dist_min = (*it).distance;
			id_ret = (*it).factory;
		}
	}

	return id_ret;
}

int closestNode(int id, list<link> *links, factory *f) {
	int dist_min = 21;
	int id_ret = -1;
	for (auto it = links[id].begin(); it != links[id].end(); it++) {
		if (f[(*it).factory].player != 1 && (*it).distance < dist_min) {
			dist_min = (*it).distance;
			id_ret = (*it).factory;
		}
	}

	return id_ret;
}

int highestArmyNode(factory *f, int count, int origin) {
	int power_max = -1;
	int id_max = -1;
	for (int i = 1; i < count; i++) {
		if (i == 6) cerr << f[i].player << ' ' << f[i].cyborgs_count << endl;
		if (f[i].player == 1 && f[i].cyborgs_count > power_max) {
			power_max = f[i].cyborgs_count;
			id_max = i;
		}
	}

	return id_max;
}
//better find source + fast source

int main()
{
	int factory_count; // the number of factories
	cin >> factory_count; cin.ignore();
	factory *factories = (factory*)malloc(sizeof(factory)*factory_count);
	list<int> alliedFactories;

	int link_count; // the number of links between factories
	cin >> link_count; cin.ignore();
	list<link> links[factory_count];
	for (int i = 0; i < link_count; i++) {
		int factory_1;
		int factory_2;
		int distance;
		cin >> factory_1 >> factory_2 >> distance; cin.ignore();

		links[factory_1].push_back(link(factory_2, distance));
		links[factory_2].push_back(link(factory_1, distance));
	}

	bool firstTurn = true;
	list<troop> troops;
	int startingNode;
	int startingEnemyNode;
	int startingCyborgs;
	int bomb_count = 2;
	int current_turn = 1;

	// game loop
	while (1) {
		auto start_loop = steady_clock::now();
		current_turn++;
		cerr << "new loop" << endl;
		bool fin = false;
		//int ally_cyborg_count = 0;
		//int enemy_cyborg_count = 0;

		//search for dead troops and delete
		killTroops(&troops);

		int entity_count; // the number of entities (e.g. factories and troops)
		cin >> entity_count; cin.ignore();
		//int troop_count = entity_count - factory_count;
		cerr << "start data input" << endl;
		for (int i = 0; i < entity_count; i++) {
			int entity_id;
			string entity_type;
			int arg_1;
			int arg_2;
			int arg_3;
			int arg_4;
			int arg_5;
			cin >> entity_id >> entity_type >> arg_1 >> arg_2 >> arg_3 >> arg_4 >> arg_5; cin.ignore();

			//data for factories
			if (entity_type == "FACTORY") {
				if (!firstTurn) {
					factories[entity_id].player = arg_1;
					factories[entity_id].cyborgs_count = arg_2;
				}
				else {
					factories[entity_id] = factory(arg_1, arg_2, arg_3);
					if (arg_1 == 1) {
						startingNode = entity_id;
						startingCyborgs = arg_2;
						//alliedFactories.push_back(entity_id);
					}
					if (arg_1 == -1) startingEnemyNode = entity_id;
				}
				/*
				if (arg_1 == 1) ally_cyborg_count += arg_2;
				else if (arg_1 == -1) enemy_cyborg_count += arg_2;
				cerr << "factory" << arg_1 << ' ' << arg_2 << endl;*/
			}

			//data for troops
			if (entity_type == "TROOP") {
				list<troop>::iterator it = TroopInList(entity_id, &troops);
				if (it != troops.end()) {
					(*it).time = arg_5;
					if (arg_5 == 1) (*it).alive = false;
				}
				else {
					troop *current_troop = new troop(entity_id, arg_1, arg_2, arg_3, arg_4, arg_5, true);
					troops.push_back(*current_troop);
				}

				/*
				if (arg_1 == 1) ally_cyborg_count += arg_4;
				else enemy_cyborg_count += arg_4;
				cerr << "troop" << arg_1 << ' ' << arg_2 << endl;*/
			}
		}
		cerr << "end data input" << endl;

		//showFactories(factories, factory_count);
		//showTroops(troops);

		bool skip = false;
		if (firstTurn) {
			cout << "WAIT" << ';';
			firstTurn = false;
		}
		else {
			if (bomb_count == 2) {
				cout << "BOMB " << startingNode << ' ' << startingEnemyNode << ';';
				bomb_count--;
			}
			int sourceNode = highestArmyNode(factories, factory_count, startingNode);
			if (sourceNode == -1) {
				cout << "WAIT" << ';';
				fin = true;
				skip = true;
			}
			cerr << "source" << sourceNode;
			if (!skip) {
				int destinationNode = closestProdNode(sourceNode, links, factories);
				cerr << "destination (try1):" << destinationNode << endl;
				if (destinationNode == -1) destinationNode = closestNode(sourceNode, links, factories);
				cerr << "destination (try2):" << destinationNode << endl;
				if (destinationNode == -1) {
					cout << "WAIT" << ';';
					fin = true;
				}
				cerr << sourceNode << ' ' << destinationNode << endl;

				if(!fin) cout << "MOVE " << sourceNode << ' ' << destinationNode << ' ' << factories[sourceNode].cyborgs_count << ';';
			}
		}

		//set all troops to not alive
		setAlive(&troops);

		auto end_loop = steady_clock::now();
		auto time_loop = duration_cast<milliseconds>(end_loop - start_loop);
		cout << "MSG " << time_loop.count() << "ms; turn=" << current_turn  << endl;
	}
}