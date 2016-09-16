// subway.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <list>
#define MAX 10000000
using namespace std;

struct subLine;
struct station {
public:
	string _name;
	bool _isTran;
	vector<subLine*> _lines;
	vector<station*> _nextstations;
	//for search trail
	bool mark = false;
	int cost = MAX;
	station* laststation = nullptr;
	//methods
	//constructor
	station() {}
	station(string name, bool ist) { _name = name; _isTran = ist; }
	~station() {}
	//
	void addline(subLine* subline) { _lines.push_back(subline); }
	void addstation(station* nextstation) { _nextstations.push_back(nextstation); }
};


struct subLine {
public:
	string _name;
	bool _isCir;
	vector<station*> _stations;

	bool mark = false;
	//methods
	subLine(string name, bool isc) { 
		_name = name; _isCir = isc; 
		//cout << "successfully constructed. " << _name << _isCir << endl;
	}

	void addSta(station* sta) { _stations.push_back(sta); }
	void refillSta() {
		auto b = _stations.begin();
		auto e = _stations.end();
		for (; b + 1 != e; b++) {
			(*b)->addline(this);
			(*b)->addstation(*(b+1));
			(*(b + 1))->addstation(*b);
		}
		(*b)->addline(this);
		if (_isCir) {
			auto nb = _stations.begin();
			(*b)->addstation(*nb);
			(*nb)->addstation(*b);
		}
	}
	string println() {
		string s = "";
		auto b = _stations.begin();
		auto e = _stations.end();
		for (; b != e; b++) {
			s += (*b)->_name;
			s += "\n";
		}
		return s;
	}
};


class subNet {
private:
	vector<subLine*> _sublines;
	vector<station*> _stations;
	//
	void readFile();
	void getTranSta(vector<station*>& stas, subLine* line1, subLine* line2);
	void getTranSta(vector<station*>& stas, subLine* line);
	int calcDis(station* sta1, station* sta2, subLine* line);
public:
	subNet() { readFile(); }
	~subNet() {
		for (unsigned int i = 0; i < _stations.size(); i++) {
			delete(_stations[i]);
		}
		for (unsigned int j = 0; j < _sublines.size(); j++) {
			delete(_sublines[j]);
		}
	} 
	station* findStation(string name) const;
	bool inLine(station* sta, subLine* line) const;
	bool inLine(station* sta1, station* sta2) const;
	subLine* getLine(station* sta1, station* sta2);
	//function0
	string printLine(string lineNam) const;
	//functionB
	string findTrailB(string initial, string target);
	//Ã»ÓÐ½«markÖÃ»Øfalse,Ö»ÄÜ½øÐÐÒ»´ÎÑ°Â·
	//functionC
	string findTrailC(string initial, string target);
	string completeStation(station* sta1, station* sta2);
	string printTrailC(station* iniSta, station* tarSta);
	int calcStaNum(string stationList) {
		unsigned int i;
		int num = 0;
		for (i = 0; i < stationList.size(); i++) {
			if (stationList[i] == '\n')
				num++;
		}
		return  num;
	}
};
void subNet::readFile() {
	string filename = "beijing-subway.txt";
	string line;
	bool isCircle = false;
	bool isTrans = false;
	fstream fileread;
	fileread.open(filename);
	if (!fileread) {
		cerr << "can't open file." << endl;
		exit;
	}
	while (getline(fileread, line)) {
		isCircle = (line[0] == '#');
		if (isCircle) {
			line = line.substr(1);
		}
		subLine* newline = new subLine(line, isCircle);
		//cout << newline._name;
		_sublines.push_back(newline);
		while (getline(fileread, line) && line != "") {
			isTrans = (line[0] == '#');
			if (isTrans) {
				line = line.substr(1);
				auto b = _stations.begin();
				auto e = _stations.end();
				bool flag = false;
				for (; b != e; b++) {
					if ((*b)->_name == line) {
						flag = true;
						break;
					}
				}
				if (flag) {
					newline->addSta(*b);
				}
				else {
					station* newstation = new station(line, isTrans);
					newline->addSta(newstation);
					_stations.push_back(newstation);
				}
			}
			else {
				station* newstation = new station(line, isTrans);
				newline->addSta(newstation);
				_stations.push_back(newstation);
			}
		}
		newline->refillSta();

	}
}
void subNet::getTranSta(vector<station*>& stas, subLine* line1, subLine* line2) {
	auto b = line1->_stations.begin();
	auto e = line1->_stations.end();
	for (; b != e; b++) {
		if (inLine(*b, line2))
			stas.push_back(*b);
	}
}
void subNet::getTranSta(vector<station*>& stas, subLine* line) {
	auto b = line->_stations.begin();
	auto e = line->_stations.end();
	for (; b != e; b++) {
		if ((*b)->_isTran)
			stas.push_back(*b);
	}
}
int subNet::calcDis(station* sta1, station* sta2, subLine* line) {
	if ((!inLine(sta1, line)) || (!inLine(sta2, line))) {
		cerr << "stations are not in a line." << endl;
		if (!inLine(sta1, line))
			cerr << sta1->_name << "not in line." << endl;
		if (!inLine(sta2, line))
			cerr << sta2->_name << "not in line." << endl;
		return -1;
	}
	int num1 = 0, num2 = 0;
	auto b = line->_stations.begin();
	auto e = line->_stations.end();
	int i = 0;
	for (; b != e; b++, i++) {
		if ((*b) == sta1)
			num1 = i;
		if ((*b) == sta2)
			num2 = i;
	}
	if (line->_isCir) {//?
		int length = line->_stations.size();
		int dis1 = (num1 - num2) % length;
		int dis2 = (num2 - num1) % length;
		return (dis1 > dis2) ? dis1 : dis2;
	}
	else {
		return (num1 > num2) ? num1 - num2 : num2 - num1;
	}
}
station* subNet::findStation(string name) const {
	auto b = _stations.begin();
	auto e = _stations.end();
	for (; b != e; b++) {
		if ((*b)->_name == name)
			return (*b);
	}
	return nullptr;
}
bool subNet::inLine(station* sta, subLine* line) const {
	bool flag = false;
	if (!sta->_isTran)
		return sta->_lines[0] == line;
	auto b = sta->_lines.begin();
	auto e = sta->_lines.end();
	for (; b != e; b++) {
		if (*b == line) {
			flag = true;
			break;
		}
	}
	return flag;
}
bool subNet::inLine(station* sta1, station* sta2) const {
	bool flag = false;
	auto b = sta1->_lines.begin();
	auto e = sta1->_lines.end();
	for (; b != e; b++) {
		if (inLine(sta2, *b)) {
			flag = true;
			break;
		}
	}
	return flag;
}
subLine* subNet::getLine(station* sta1, station* sta2) {
	bool flag = false;
	int dis = MAX;
	subLine* line = nullptr;
	auto b = sta1->_lines.begin();
	auto e = sta1->_lines.end();
	for (; b != e; b++) {
		if (inLine(sta2, *b) && (calcDis(sta1, sta2, *b) < dis)) {
			flag = true;
			dis = calcDis(sta1, sta2, *b);
			line = *b;
		}
	}
	if (flag)
		return line;
	else
		return nullptr;
}
string subNet::printLine(string lineNam) const {
	auto b = _sublines.begin();
	auto e = _sublines.end();
	for (; b != e; b++) {
		//cout << (*b)->_name;
		if ((*b)->_name == lineNam) {
			return (*b)->println();
		}
	}
	cerr << "No such line;" << endl;
	return "";
}
string subNet::findTrailB(string initial, string target) {
	if (initial == target) {
		return initial + "\n";
	}
	station* iniSta = findStation(initial);
	station* tarSta = findStation(target);
	if (iniSta == nullptr || tarSta == nullptr) {
		cerr << "Station not exists." << endl;
		return "";
	}
	list<station*> list;
	vector<station*> trail;
	station* sta;
	bool flag = false;
	list.push_back(tarSta);
	while (!list.empty()) {
		sta = *(list.begin());
		if (sta == nullptr) break;//
		list.pop_front();
		if (sta == iniSta) {
			flag = true;
			break;
		}
		else {
			auto b = sta->_nextstations.begin();
			auto e = sta->_nextstations.end();
			for (; b != e; b++) {
				if (!(*b)->mark) {
					(*b)->mark = true;
					(*b)->laststation = sta;
					list.push_back(*b);
				}
			}
		}
	}
	if (!flag) {
		cerr << "Can't find the way" << endl;
		return "";
	}
	sta = iniSta;
	while (sta != tarSta) {
		trail.push_back(sta);
		sta = sta->laststation;
	}
	trail.push_back(sta);
	string subTrail;
	auto b = trail.begin();
	auto e = trail.end();
	if (b == e) {
		subTrail = (*b)->_name;
	}
	else {
		subTrail += (*b)->_name;
		subTrail += "\n";
		for (++b, --e; b != e; b++) {
			subTrail += (*b)->_name;
			if ((*b)->_isTran && !inLine(*(b - 1), *(b + 1))) {
				subTrail += "»»³Ë";
				subTrail += getLine(*b, *(b + 1))->_name;
			}
			subTrail += "\n";
		}
		subTrail += (*b)->_name;
		subTrail += "\n";
	}
	return subTrail;
}
string subNet::findTrailC(string initial, string target) {
	if (initial == target) {
		return initial + "\n";
	}
	station* iniSta = findStation(initial);
	station* tarSta = findStation(target);
	if (iniSta == nullptr || tarSta == nullptr) {
		cerr << "Station not exists." << endl;
		return "";
	}
	if (inLine(iniSta, tarSta)) {
		iniSta->laststation = tarSta;
		return printTrailC(iniSta, tarSta);
	}
	tarSta->cost = 0;
	vector<subLine*> lineList;
	vector<station*> stas;
	auto b = (tarSta->_lines).begin();
	auto e = (tarSta->_lines).end();
	for (; b != e; b++) {
		lineList.push_back(*b);
		(*b)->mark = true;
		stas.clear();
		getTranSta(stas, (*b));
		auto be = stas.begin();
		auto en = stas.end();
		for (; be != en; be++) {
			(*be)->cost = calcDis((*be), tarSta, (*b));
			(*be)->laststation = tarSta;
		}
	}
	bool flag = false;
	subLine* line;
	while (true) {
		auto b = lineList.begin();
		auto e = lineList.end();
		for (; b != e; b++) {
			if (inLine(iniSta, *b)) {
				line = (*b);
				flag = true;
				break;
			}
			//(*b)->mark = true;
		}
		if (flag)	break;
		vector<subLine*> newList;

		b = lineList.begin();
		e = lineList.end();
		for (; b != e; b++) {
			stas.clear();
			getTranSta(stas, *b);
			auto be = stas.begin();
			auto en = stas.end();
			for (; be != en; be++) {
				auto beg = (*be)->_lines.begin();
				auto end = (*be)->_lines.end();
				for (; beg != end; beg++) {
					if (!(*beg)->mark) {
						newList.push_back(*beg);
						(*beg)->mark = true;
					}
				}
			}
		}
		b = lineList.begin();
		e = lineList.end();
		for (; b != e; b++) {
			auto nb = newList.begin();
			auto ne = newList.end();
			for (; nb != ne; nb++) {
				stas.clear();
				getTranSta(stas, *b, *nb);
				auto stab = stas.begin();
				auto stae = stas.end();
				for (; stab != stae; stab++) {
					vector<station*> transtas;
					getTranSta(transtas, *b);
					auto tranb = transtas.begin();
					auto trane = transtas.end();
					for (; tranb != trane; tranb++) {
						if (((*tranb)->cost + calcDis(*stab, *tranb, *b)) < ((*stab)->cost)) {
							(*stab)->laststation = *tranb;
							(*stab)->cost = (*tranb)->cost + calcDis(*stab, *tranb, *b);
						}
					}
				}
			}
		}
		lineList = newList;
	}
	for (unsigned int i = 0; i < lineList.size(); i++) {
		if (inLine(iniSta, lineList[i])) {
			vector<station*> transtas;
			getTranSta(transtas, lineList[i]);
			for (unsigned int j = 0; j < transtas.size(); j++) {
				if (((transtas[j])->cost + calcDis(iniSta, transtas[j], lineList[i])) < (iniSta->cost)) {
					iniSta->laststation = transtas[j];
					iniSta->cost = (transtas[j])->cost + calcDis(iniSta, transtas[j], lineList[i]);
				}
			}
		}
	}
	return printTrailC(iniSta, tarSta);
}
string subNet::completeStation(station* sta1, station* sta2) {
	subLine* line = getLine(sta1, sta2);
	string stas;
	int i, j;
	for (i = 0; i < (int(line->_stations.size())); i++) {
		if (sta1 == line->_stations[i])
			break;
	}
	for (j = 0; j < (int(line->_stations.size())); j++) {
		if (sta2 == line->_stations[j])
			break;
	}
	if (line->_isCir) {
		int length = line->_stations.size();
		if (i > j) {
			swap(i, j);
		}
		if (j - i < length / 2) {
			for (; ++i < j;) {
				stas += line->_stations[i]->_name;
				stas += "\n";
			}
		}
		else {
			for (; ++j < i + length;) {
				stas += line->_stations[j%length]->_name;
				stas += "\n";
			}
		}
	}
	else {
		if (i > j) {
			swap(i, j);
		}
		for (; ++i < j;) {
			stas += line->_stations[i]->_name;
			stas += "\n";
		}
	}
	return stas;
}
 string subNet::printTrailC(station* iniSta, station* tarSta) {
	station* sta = iniSta;
	vector<station*> trail;
	while (sta != tarSta) {
		trail.push_back(sta);
		sta = sta->laststation;
	}
	trail.push_back(sta);
	string subTrail;
	auto b = trail.begin();
	auto e = trail.end();
	if (b == e) {
		subTrail = (*b)->_name;
	}
	else {
		subTrail += (*b)->_name;
		subTrail += "\n";
		subTrail += completeStation(*b, *(b + 1));
		for (++b, --e; b != e; b++) {
			subTrail += (*b)->_name;
			if ((*b)->_isTran && !inLine(*(b - 1), *(b + 1))) {
				subTrail += "»»³Ë";
				subTrail += getLine(*b, *(b + 1))->_name;
			}
			subTrail += "\n";
			subTrail += completeStation(*b, *(b + 1));
		}
		subTrail += (*b)->_name;
		subTrail += "\n";
	}
	return subTrail;
}
int main(int argc, char* argv[])
{
	subNet subnet;
	if (argc == 1) {
		string line;
		while (getline(cin, line)) {
			cout << subnet.printLine(line) << endl;
		}
	}
	else if (argc == 4) {
		string mode = argv[1], station1 = argv[2], station2 = argv[3];
		string trail;
		if (mode == "-b") {
			if (station1 == "2ºÅº½Õ¾Â¥") {
				station1 = "3ºÅº½Õ¾Â¥";
				cout << "´Ó2ºÅº½Õ¾Â¥³ö·¢Ç°ÍùÊÐÄÚµÄ³Ë¿Í£¬Çë´Ó2ºÅº½Õ¾Â¥³ö·¢¡£" << endl;
			}
			if (station2 == "2ºÅº½Õ¾Â¥") {
				station2 = "3ºÅº½Õ¾Â¥";
				cout << "µ½´ï2ºÅº½Õ¾Â¥µÄ³Ë¿Í£¬Çë´Ó3ºÅº½Õ¾Â¥¼ÌÐø³Ë×ø»ú³¡Ïßµ½´ï2ºÅº½Õ¾Â¥¡£" << endl;
				
			}
			trail = subnet.findTrailB(station1, station2);
			cout << subnet.calcStaNum(trail) << endl;
			cout << trail ;
		}
		else if (mode == "-c") {
			if (station1 == "2ºÅº½Õ¾Â¥") {
				station1 = "3ºÅº½Õ¾Â¥";
				cout << "´Ó2ºÅº½Õ¾Â¥³ö·¢Ç°ÍùÊÐÄÚµÄ³Ë¿Í£¬Çë´Ó2ºÅº½Õ¾Â¥³ö·¢¡£" << endl;
			}
			if (station2 == "2ºÅº½Õ¾Â¥") {
				station2 = "3ºÅº½Õ¾Â¥";
				cout << "µ½´ï2ºÅº½Õ¾Â¥µÄ³Ë¿Í£¬Çë´Ó3ºÅº½Õ¾Â¥¼ÌÐø³Ë×ø»ú³¡Ïßµ½´ï2ºÅº½Õ¾Â¥¡£" << endl;
			}
			trail = subnet.findTrailC(station1, station2);
			cout << subnet.calcStaNum(trail) << endl;
			cout << trail ;
		}
		else {
			cout << "Illegal input." << endl;
		}
	}
	else {
		cout << "This function is unable to use." << endl;
	}
	
    return 0;
}


