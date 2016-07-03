#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <vector>
#include "LinearSystem.h"
#include "Componente.h"
#include "Netlist.h"

using namespace std;

Netlist::Netlist(string netlistPath)
{
	string linha;
	ifstream netlistFile;
	unsigned index = 0;
	
	netlistFile.open(netlistPath);
	if (!netlistFile)
	{
		cout << "Nao foi possivel abrir o arquivo" << endl;
	}
	cout << "Lendo Netlist : " << netlistPath << endl;
	getline(netlistFile, linha);
	while (getline(netlistFile, linha))
	{
		netlist.push_back(linha);
		cout << linha << endl;
		stringstream auxiliar(linha);   //initializing stringstream
		vector <string> lineParameters; // array that will hold the each parameter in the netlist line
		string campo;                   //auxiliar variable

		while (auxiliar.good()) //this while push back from the string stream each string in the line and record on the lineParameters array.
		{
			auxiliar >> campo;
			lineParameters.push_back(campo);
		}

		switch (linha.at(0)) //Get the first letter of the netlist, the identifier for each component
		{
		case 'R':
			string nome;
			Resistor *r = new Resistor;
			nome = lineParameters[0];
			(r->addType)(nome.begin);
			nome.erase(nome.begin()); // remove the first letter, the component identifier
			(r->setName)(nome);
			(r->addNode)(stoul(lineParameters[1]));
			(r->addNode)(stoul(lineParameters[2]));
			(r->setValue)(stod(lineParameters[3]));
			componentes.push_back(r);
			break;

		case 'I': // fonte de corrente independente
			string nome;
			CurrentSource *i = new CurrentSource;
			nome = lineParameters[0];
			(i->addType)(nome.begin);
			nome.erase(nome.begin()); // remove the first letter, the component identifier
			(i->setName)(nome);
			(i->addNode)(stoul(lineParameters[1]));
			(i->addNode)(stoul(lineParameters[2]));
			(i->setValue)(stod(lineParameters[3]));
			(i->setPhase)(stod(lineParameters[4]));
			(i->setDCValue)(stod(lineParameters[5]));
			componentes.push_back(i);
			break;

		case 'V': // fonte de corrente independente
			string nome;
			VoltageSource *v = new VoltageSource;
			nome = lineParameters[0];
			(v->addType)(nome.begin);
			nome.erase(nome.begin()); // remove the first letter, the component identifier
			(v->setName)(nome);
			(v->addNode)(stoul(lineParameters[1]));
			(v->addNode)(stoul(lineParameters[2]));
			(v->setValue)(stod(lineParameters[3]));
			(v->setPhase)(stod(lineParameters[4]));
			(v->setDCValue)(stod(lineParameters[5]));
			componentes.push_back(v);
			break;

        case 'E':
            string nome;
            VoltageSrcCntrlVoltage *e = new VoltageSrcCntrlVoltage;
            nome = lineParameters[0];
			(e->addType)(nome.begin);
            nome.erase(nome.begin()); // remove the first letter, the component identifier
            (e->setName)(nome);
            (e->addNode)(stoul(lineParameters[1]));
            (e->addNode)(stoul(lineParameters[2]));
            (e->addNode)(stoul(lineParameters[3]));
            (e->addNode)(stoul(lineParameters[4]));
            (e->setValue)(stod(lineParameters[5]));
            componentes.push_back(e);

        case 'F':
            string nome;
            CurrentSrcCntrlCurrent *f = new CurrentSrcCntrlCurrent;
            nome = lineParameters[0];
			(f->addType)(nome.begin);
            nome.erase(nome.begin()); // remove the first letter, the component identifier
            (f->setName)(nome);
            (f->addNode)(stoul(lineParameters[1]));
            (f->addNode)(stoul(lineParameters[2]));
            (f->addNode)(stoul(lineParameters[3]));
            (f->addNode)(stoul(lineParameters[4]));
            (f->setValue)(stod(lineParameters[5]));
            componentes.push_back(f);
            break;
        case 'G':
            string nome;
            CurrentSrcCntrlVoltage *g = new CurrentSrcCntrlVoltage;
            nome = lineParameters[0];
			(g->addType)(nome.begin);
            nome.erase(nome.begin()); // remove the first letter, the component identifier
            (g->setName)(nome);
            (g->addNode)(stoul(lineParameters[1]));
            (g->addNode)(stoul(lineParameters[2]));
            (g->addNode)(stoul(lineParameters[3]));
            (g->addNode)(stoul(lineParameters[4]));
            (g->setValue)(stod(lineParameters[5]));
            componentes.push_back(g);
            break;

        case 'H':
            string nome;
            VoltageSrcCntrlCurrent *h = new VoltageSrcCntrlCurrent;
            nome = lineParameters[0];
			(h->addType)(nome.begin);
            nome.erase(nome.begin()); // remove the first letter, the component identifier
            (h->setName)(nome);
            (h->addNode)(stoul(lineParameters[1]));
            (h->addNode)(stoul(lineParameters[2]));
            (h->addNode)(stoul(lineParameters[3]));
            (h->addNode)(stoul(lineParameters[4]));
            (h->setValue)(stod(lineParameters[5]));
            componentes.push_back(h);
            break;

		case 'C':
			string nome;
			Capacitor *c = new Capacitor;
			nome = lineParameters[0];
			(c->addType)(nome.begin);
			nome.erase(nome.begin()); // remove the first letter, the component identifier
			(c->setName)(nome);
			(c->addNode)(stoul(lineParameters[1]));
			(c->addNode)(stoul(lineParameters[2]));
			(c->setValue)(stod(lineParameters[3]));
			(c->setInitialValue)(stod(lineParameters[4]));
			componentes.push_back(c);
			break;

  		case 'L':
			string nome;
			Indutor *l = new Indutor;
			nome = lineParameters[0];
			(l->addType)(nome.begin());
			nome.erase(nome.begin()); // remove the first letter, the component identifier
			(l->setName)(nome);
			(l->addNode)(stoul(lineParameters[1]));
			(l->addNode)(stoul(lineParameters[2]));
			(l->setValue)(stod(lineParameters[3]));
			(l->setInitialValue)(lineParameters[4]);
			componentes.push_back(l);
			break;

		case 'K':
			string firstIndutor, secondIndutor,nome;
			Transformador *t = new Transformador;
			(t->addType)(nome.begin());
			unsigned achouAmbos = 0;
			unsigned count,countAuxiliar = 0;
			vector<Componente*> auxiliar;
			unsigned firstIndutorPosition, secondIndutorPosition;

			firstIndutor = stoul(lineParameters[1]);
			secondIndutor = stoul(lineParameters[2]);

			nome.erase(nome.begin()); // remove the first letter, the component identifier
			(t->setName)(nome);

			while (!achouAmbos && count != sizeof(componentes))
			{
				unsigned achou1, achou2 = 0;
				if ((componentes[count]->getName) == firstIndutor)
				{
					achou1 = 1;
					(t->addNode)(componentes[count]->getNode(1));
					(t->addNode)(componentes[count]->getNode(2));
					(t->setValueFirstIndutor)(componentes[count]->getValue);
					(t->setValueM)(stod(lineParameters[3]));
					firstIndutorPosition = count;
				}

				if ((componentes[count]->getName) == secondIndutor)
				{
					achou2 = 1;
					(t->addNode)(componentes[count]->getNode(1));
					(t->addNode)(componentes[count]->getNode(2));
					(t->setValueSecondIndutor)(componentes[count]->getValue);
					secondIndutorPosition = count;
				}
				if (achou1 == 1 && achou2 == 1)
					achouAmbos = 1;

				count = count + 1;
			}

			count = 0;
			countAuxiliar = 0;
			while (count != (sizeof(componentes)-1))
			{
				if (count != firstIndutorPosition && count != secondIndutorPosition)
				{
					auxiliar[countAuxiliar] = componentes[count];
					countAuxiliar = countAuxiliar + 1;
				}

				count = count + 1;
			}

			componentes = auxiliar;

			componentes.push_back(t);
			break;

			case 'O':
				string nome;
				AmpOp *o = new AmpOp;
				nome = lineParameters[0];
				(o->addType)(nome.begin());
				nome.erase(nome.begin()); // remove the first letter, the component identifier
				(o->setName)(nome);
				(o->addNode)(stoul(lineParameters[1]));
				(o->addNode)(stoul(lineParameters[2]));
				(o->addNode)(stoul(lineParameters[3]));
				(o->addNode)(stoul(lineParameters[4]));
				componentes.push_back(o);
				break;

		}
     	index++;
	}

	netlistFile.close();
}


void Netlist::DoConductanceMatrix()
{
	setRowsValue(componentes);
	InitializeG_Matrix();
	unsigned count = 0;
	double value;

	while (count != sizeof(componentes) - 1)
	{
		if (componentes[count]->getType == 'R')
		{
			value = 1/(componentes[count]->getValue);
			G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(0)] += value;
			G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(1)] += value;
			G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(1)] -= value;
			G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(0)] -= value;
		}

		else if (componentes[count]->getType == 'G')
		{
			value = componentes[count]->getValue;
			G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(2)] += value;
			G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(3)] += value;
			G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(3)] -= value;
			G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(2)] -= value;
		
		}

    
		count += 1;
	}
	

}



