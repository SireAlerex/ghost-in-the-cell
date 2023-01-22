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

//typetool
template <typename T>
constexpr auto type_name() {
	std::string_view name, prefix, suffix;
#ifdef __clang__
	name = __PRETTY_FUNCTION__;
  prefix = "auto type_name() [T = ";
  suffix = "]";
#elif defined(__GNUC__)
	name = __PRETTY_FUNCTION__;
	prefix = "constexpr auto type_name() [with T = ";
	suffix = "]";
#elif defined(_MSC_VER)
	name = __FUNCSIG__;
  prefix = "auto __cdecl type_name<";
  suffix = ">(void)";
#endif
	name.remove_prefix(prefix.size());
	name.remove_suffix(suffix.size());
	return name;
}


//TODO
//troop_power : travel time

//focus attaque
//améliorer willBeConquered (ne pas attaquer juste avant d'une attaque qui ne passerait pas normalement)
//troop_power : take prod and travel time into account (travel time can make it very defensive)
//bomb defence protocol
//version plus avancees
//défense + consolidation + pas d'attaque si prod plus faible


//value of targetfactory : value = MAX_DIST*prod*MOD_PROD + MAX_PROD*(MAX_DIST-dist)*MOD_DIST
//constantes
int MAX_DIST = 20;
int MAX_PROD = 3;
float MOD_DIST = 1.5;
float MOD_PROD = 1;
int factory_count;
int no_attck = 0;
int ally_prod = 0;
int enemy_prod = 0;
int POWER_LIMIT_INC = 20;

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

void emptyTroops(list<troop> *troops) {
	while (!troops->empty()) {
		troops->pop_front();
	}
}

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
	for (auto it = l->begin(); it != l->end(); it++)
		if ((*it).alive == false) (*it).~troop();
}

//set alive parameter of all troops to false at the end of a turn
void setAlive(list<troop> *l) {
	for (auto it = l->begin(); it != l->end(); it++) {
		(*it).alive = false;
	}
}

list<link> mergeLink(list<link> left, list<link> right) {
	list<link> result;

	while (!left.empty() && !right.empty()) {
		if (left.front().distance <= right.front().distance) {
			result.push_back(left.front());
			left.pop_front();
		}
		else {
			result.push_back(right.front());
			right.pop_front();
		}
	}

	while (!left.empty()) {
		result.push_back(left.front());
		left.pop_front();
	}
	while (!right.empty()) {
		result.push_back(right.front());
		right.pop_front();
	}

	return result;
}

list<link> merge_sortLink(list<link> l) {
	int list_size = l.size();
	if (list_size <= 1) return(l);

	list<link> left;
	list<link> right;
	int i = 0;
	for (auto it = l.begin(); it != l.end(); it++) {
		if (i < (list_size/2)) left.push_back(*it);
		else right.push_back(*it);
		i++;
	}

	left = merge_sortLink(left);
	right = merge_sortLink(right);

	return(mergeLink(left, right));
}

void sortLinks(list<link> *links, int factory_count) {
	list<link> result[factory_count];
	for (int i = 0; i < factory_count; i++) {
		result[i] = merge_sortLink(links[i]);
	}

	//emptying links
	for (int i = 0; i < factory_count; i++) {
		while (!links[i].empty()) {
			links[i].pop_front();
		}
	}
	for (int i = 0; i < factory_count; i++) {
		for (auto it = result[i].begin(); it != result[i].end(); it++) {
			links[i].push_back(*it);
		}
	}
}

int closest3ProdNode(int id, list<link> *links, factory *f) {
	int dist_min = 21;
	int id_ret = -1;
	int player = f[id].player;
	for (auto it = links[id].begin(); it != links[id].end(); it++) {
		if (f[(*it).factory].player != player && f[(*it).factory].prod == 3 && (*it).distance < dist_min) {
			dist_min = (*it).distance;
			id_ret = (*it).factory;
		}
	}

	return id_ret;
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

int targetAvoid(int source, list<link> *links, factory *factories, int toAvoid) {
	int target = -1, value_max = -1, target_zero = -1;
	for (auto i = links[source].begin(); i != links[source].end(); i++) {
		int cible = (*i).factory;
		int value = MAX_DIST*factories[cible].prod*MOD_PROD + MAX_PROD*(MAX_DIST-(*i).distance)*MOD_DIST;
		if (value > value_max && factories[cible].player != factories[source].player && cible != toAvoid) {
			if (factories[cible].prod == 0) {
				value_max = value;
				target_zero = cible;
			}
			else {
				value_max = value;
				target = cible;
			}
		}
	}

	return value_max != -1 ? target : target_zero;
}

bool isPartOfArray(int i, int *array, int size) {
	for (int k = 0; k < size; k++) {
		//cerr << "isPart" << ' ' << array[k] << ' ' << i << endl;
		if (array[k] == i) return true;
	}
	return false;
}

int closestTargetAvoid(int source, list<link> *links, factory *factories, int *avoid) {
	int target = -1, target_zero = -1;
	bool found = false;
	auto cible = links[source].begin();
	while (!found && cible != links[source].end()) {
		if (factories[cible->factory].player != factories[source].player && !isPartOfArray(cible->factory, avoid, factory_count)) {
			if (cible->factory == 5) //cerr <<endl<< isPartOfArray(5, avoid, factory_count) <<endl;
				if (factories[cible->factory].prod != 0) {
					target = cible->factory;
					found = true;
				}
				else {
					target_zero = cible->factory;
				}
		}
		cible++;
	}

	return (target != -1)? target : target_zero;
}

int closestTarget(int source, list<link> *links, factory *factories) {
	int target = -1, target_zero = -1;
	bool found = false;
	auto cible = links[source].begin();
	while (!found && cible != links[source].end()) {
		if (factories[cible->factory].player != factories[source].player) {
			if (factories[cible->factory].prod != 0) {
				target = cible->factory;
				found = true;
			}
			else {
				target_zero = cible->factory;
			}
		}
		cible++;
	}

	return (target != -1)? target : target_zero;
}

int target(int source, list<link> *links, factory *factories) {
	int target = -1, value_max = -1, target_zero = -1;
	for (auto i = links[source].begin(); i != links[source].end(); i++) {
		int cible = (*i).factory;
		int value = MAX_DIST*factories[cible].prod*MOD_PROD + MAX_PROD*(MAX_DIST-(*i).distance)*MOD_DIST;
		if (value > value_max && factories[cible].player != factories[source].player) {
			if (factories[cible].prod == 0) target_zero = cible;
			else {
				value_max = value;
				target = cible;
			}
		}
	}

	return value_max != -1 ? target : target_zero;
}

int highestArmyNode(factory *f, int count, int origin) {
	int power_max = -1;
	int id_max = -1;
	for (int i = 1; i < count; i++) {
		if (f[i].player == 1 && f[i].cyborgs_count > power_max) {
			power_max = f[i].cyborgs_count;
			id_max = i;
		}
	}

	return id_max;
}

int bombStrategy(factory *factories, list<link> *links, int allyStart, int enemyStart, int *bomb_count) {
	//first bomb on enemyStart
	if ((*bomb_count) == 2) {
		cout << "BOMB " << allyStart << ' ' << enemyStart << ';';
		(*bomb_count)--;
	}

	int bomb_target = targetAvoid(enemyStart, links, factories, allyStart);
	if (bomb_target != -1) {
		cout << "BOMB " << allyStart << ' ' << bomb_target << ';';
		(*bomb_count)--;
	}

	return bomb_target;
}

int troopPower(factory *factories, int source, int destination) {
	int ally_power = factories[source].cyborgs_count;
	int enemy_power = factories[destination].cyborgs_count;
	if (enemy_power <= 1) return 2;
	return ally_power <= 1.5*enemy_power ? ally_power : 1.5*enemy_power;
}

bool canAttackSucceed(factory *factories, int source, int destination) {
	if (source == -1 || destination == -1) return false;
	cerr << endl << factories[source].cyborgs_count << ' ' << factories[destination].cyborgs_count << ' ';
	return factories[source].cyborgs_count > factories[destination].cyborgs_count ? true : false;
}

bool isTargeted(int target,list<troop> troops) {
	for (auto it = troops.begin(); it != troops.end(); it++) {
		if ((*it).destination == target && (*it).player == -1) {
			//(*it).showTroop();
			return true;
		}
	}
	return false;
}

bool willBeConquered(int target, list<troop> troops, factory *factories) {
	int target_power = factories[target].cyborgs_count;
	for (auto it = troops.begin(); it != troops.end(); it++) {
		if ((*it).destination == target) {
			factories[target].player != (*it).player ? target_power -= (*it).power : target_power += (*it).power;
		}
	}
	return (target_power > 0) ? false : true;
}

int enemy_attack_count(list<troop> troops) {
	int enemy_attack_count;
	for (auto it = troops.begin(); it != troops.end(); it++) {
		if ((*it).player == -1) enemy_attack_count++;
	}

	return enemy_attack_count;
}

void firstAttack(factory *factories, list<link> *links, int factory_count, list<troop> troops, int lim, int source) {
	int target = closestTarget(source, links, factories);
	int avoid[factory_count];
	int avoid_index = 0;
	while (target != -1 && avoid_index < factory_count &&
		   ((factories[source].cyborgs_count)-lim) <= factories[target].cyborgs_count) {
		avoid[avoid_index] = target;
		avoid_index++;
		target = closestTargetAvoid(source, links, factories, avoid);
	}
	if (target != -1) {
		int power = troopPower(factories, source, target);
		cout << "MOVE " << source << ' ' << target << ' ' << power << ';';
	}
}

int neutralzero(factory *factories, list<link> *links, int source)  {
	for (auto it = links[source].begin(); it != links[source].end(); it++) {
		if (factories[(*it).factory].player == 0 && factories[(*it).factory].prod == 0) return (*it).factory;
	}
	return -1;
}

int enemyzero(factory *factories, list<link> *links, int source)  {
	for (auto it = links[source].begin(); it != links[source].end(); it++) {
		if (factories[(*it).factory].player == -1 && factories[(*it).factory].prod == 0) return (*it).factory;
	}
	return -1;
}

int fullAttack(factory *factories, list<link> *links, int factory_count, list<troop> troops) {
	int attack_count = 0, avoid_index = 0;
	int avoid[factory_count];
	bool attacked = false;
	for (int source = 0; source < factory_count; source++) {
		if(factories[source].player == 1) //cerr << "isTarget?"<<source << " : " << isTargeted(source, troops) << '\n';
			if (factories[source].player == 1 && !willBeConquered(source, troops, factories)) {
				int target_attack = closestTarget(source, links, factories);
				attacked = false;
				//cerr << source << " initial target : " << target_attack << endl;
				//cerr << "canAttack? : "<< source << ' ' << target_attack << ' ' << canAttackSucceed(factories, source, target_attack) << endl;
				//if cant attack, recalculate target with avoid (array ?)
				while (willBeConquered(target_attack, troops, factories) && !canAttackSucceed(factories, source, target_attack) && target_attack != -1) {
					//cerr << "target:" << target_attack << ' ';
					avoid[avoid_index] = target_attack;
					//cerr << "avoid set " << avoid[avoid_index] << ' ' << avoid_index;
					avoid_index++;

					target_attack = closestTargetAvoid(source, links, factories, avoid);
					//cerr << " end while\n";
				}
				//cerr << " ; new target : " << target_attack;
				//cerr << "success? " << canAttackSucceed(factories, source, target_attack) << endl;

				if (target_attack != -1) {
					if (canAttackSucceed(factories, source, target_attack)) {
						int power = troopPower(factories, source, target_attack);
						cout << "MOVE " << source << ' ' << target_attack << ' ' << power << ';';
						attack_count++;
						attacked = true;
					}
					else if (enemy_attack_count(troops) == 0) {
						cerr << "zero attack" << endl;
						int target_zero = neutralzero(factories, links, source);
						if (target_zero != -1) {
							int power = troopPower(factories, source, target_zero);
							cout << "MOVE " << source << ' ' << target_zero << ' ' << power << ';';
							attack_count++;
							attacked = true;
						}
						else {
							target_zero = enemyzero(factories, links, source);
							if (target_zero != -1) {
								int power = troopPower(factories, source, target_zero);
								cout << "MOVE " << source << ' ' << target_zero << ' ' << power << ';';
								attack_count++;
								attacked = true;
							}
						}
					}
				}
				if (!attacked) {
					if (factories[source].cyborgs_count > POWER_LIMIT_INC && factories[source].prod < 3) {
						cout << "INC " << source << ';';
					}
				}
			}
	}
	return attack_count;
}

void incProtocol(factory *factories, int factory_count, list<troop> troops) {
	for (int i = 0; i < factory_count; i++) {
		if (factories[i].player == 1 && !willBeConquered(i, troops, factories)
			&& factories[i].cyborgs_count > POWER_LIMIT_INC && factories[i].prod < 3) {
			cout << "INC " << i << ';';
		}
	}
}

int closestZeroProd(int source, factory *factories, list<link> *links) {
	for (auto it = links[source].begin(); it != links[source].end(); it++) {
		int cible = (*it).factory;
		if (factories[cible].player == 1 && factories[cible].prod == 0) return cible;
	}
	return -1;
}

void stalemateStrategy(factory *factories, int factory_count, list<link> *links, list<troop> troops) {
	for (int i = 0; i < factory_count; i++) {
		if (factories[i].player == 1 && !willBeConquered(i, troops, factories)) {
			int target_zero = closestZeroProd(i, factories, links);
			if (target_zero != -1) {
				int power_needed = POWER_LIMIT_INC - factories[target_zero].cyborgs_count+1;
				if (factories[i].cyborgs_count > power_needed) {
					cerr << "stalemate" << endl;
					cout << "MOVE " << i << ' ' << target_zero << ' ' << power_needed << ';';
					return;
				}
			}
		}
	}
}

int main()
{
	auto startMain = steady_clock::now();
	// the number of factories
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
	sortLinks(links, factory_count);
	//showLinks(links, factory_count);

	bool firstTurn = true;
	list<troop> troops;
	int startingNode;
	int startingEnemyNode;
	int startingCyborgs;
	int bomb_count = 2;
	int current_turn = 1;
	const int BOMB_ATTACK_POWER = 2;
	int bomb_target = -1;
	int bombs_turn;
	bool bombs_launched = false;
	bool bombs_attack_launched = false;

	// game loop
	while (1) {
		auto start_loop = steady_clock::now();
		current_turn++;
		cerr << "new loop" << endl;
		bool fin = false;
		//int ally_cyborg_count = 0;
		//int enemy_cyborg_count = 0;

		//search for dead troops and delete
		//killTroops(&troops);

		int entity_count; // the number of entities (e.g. factories and troops)
		cin >> entity_count; cin.ignore();
		//int troop_count = entity_count - factory_count;
		ally_prod = 0;
		enemy_prod = 0;
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
					factories[entity_id].prod = arg_3;
					if (arg_1 == 1) ally_prod += arg_3;
					if (arg_1 == -1) enemy_prod += arg_3;
				}
				else {
					factories[entity_id] = factory(arg_1, arg_2, arg_3);
					if (arg_1 == 1) {
						startingNode = entity_id;
						startingCyborgs = arg_2;
						//alliedFactories.push_back(entity_id);
						ally_prod += arg_3;
					}
					if (arg_1 == -1) {
						startingEnemyNode = entity_id;
						enemy_prod += arg_3;
					}
				}
				/*
				if (arg_1 == 1) ally_cyborg_count += arg_2;
				else if (arg_1 == -1) enemy_cyborg_count += arg_2;
				cerr << "factory" << arg_1 << ' ' << arg_2 << endl;*/
			}

			//data for troops
			if (entity_type == "TROOP") {
				/*
				list<troop>::iterator it = TroopInList(entity_id, &troops);
				if (it != troops.end()) {
					(*it).time = arg_5;
					if (arg_5 == 1) (*it).alive = false;
				}
				else {
					troop *current_troop = new troop(entity_id, arg_1, arg_2, arg_3, arg_4, arg_5, true);
					troops.push_back(*current_troop);
				}*/

				troop *current_troop = new troop(entity_id, arg_1, arg_2, arg_3, arg_4, arg_5, true);
				troops.push_back(*current_troop);

				/*
				if (arg_1 == 1) ally_cyborg_count += arg_4;
				else enemy_cyborg_count += arg_4;
				cerr << "troop" << arg_1 << ' ' << arg_2 << endl;*/
			}
		}

		bool skip = false;
		if (firstTurn) {
			cout << "WAIT" << ';';
			firstTurn = false;
		}
		else {
			/*
			if (bomb_count == 2) {
				cout << "BOMB " << startingNode << ' ' << startingEnemyNode << ';';
				bomb_count--;
			}
			if (current_turn == second_bomb_launch_turn) {
				cerr << "bomb2 launch turn" << endl;
				if (factories[second_bomb_target].player != 1 && second_bomb_target != -1
					&& factories[startingNode].player == 1) {
					cout << "BOMB " << startingNode << ' ' << second_bomb_target << ';';
					bomb_count--;
				}
				else second_bomb_launch_turn++;
			}*/
			if (!bombs_launched) {
				bomb_target = bombStrategy(factories, links, startingNode, startingEnemyNode, &bomb_count);
				bombs_launched = true;
				bombs_turn = current_turn;
			}
			if (!bombs_attack_launched && current_turn != bombs_turn) {
				cout << "MOVE " << startingNode << ' ' << startingEnemyNode << ' ' << BOMB_ATTACK_POWER << ';';
				if (bomb_target != -1) {
					cout << "MOVE " << startingNode << ' ' << bomb_target << ' ' << BOMB_ATTACK_POWER << ';';
				}
				bombs_attack_launched = true;
			}

			//if current_turn = bombs_turn : save 2*BOMB_ATTACK_POWER for next turn (int
			if (current_turn != bombs_turn) {
				if (current_turn == bombs_turn+1) firstAttack(factories, links, factory_count, troops, 4, startingNode);
				int attack_count;
				attack_count = fullAttack(factories, links, factory_count, troops);
				if (attack_count == 0) {
					if (enemy_attack_count(troops) == 0) stalemateStrategy(factories, factory_count, links, troops);
					else cout << "WAIT;";

				}
				cerr << "attack_count : " << attack_count << endl;
				/*
				int sourceNode = highestArmyNode(factories, factory_count, startingNode);
				if (sourceNode == -1) {
					cout << "WAIT" << ';';
					fin = true;
					skip = true;
				}
				cerr << "source " << sourceNode;
				if (!skip) {
					int destinationNode = closestTarget(sourceNode, links, factories);
					cerr << " destination " << destinationNode;
					if (destinationNode == -1) {
						cout << "WAIT" << ';';
						fin = true;
					}

					if(!fin) cout << "MOVE " << sourceNode << ' ' << destinationNode << ' ' << factories[sourceNode].cyborgs_count << ';';
				}*/
			}

		}
		emptyTroops(&troops);
		cerr << "no_attack:" << no_attck << endl;

		auto end_loop = steady_clock::now();
		auto time_loop = duration_cast<milliseconds>(end_loop - start_loop);
		cout << "MSG " << time_loop.count() << "ms; turn=" << current_turn << "; prod=" << ally_prod - enemy_prod << endl;
	}
}