#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <vector>
#include <complex>
#include <cmath>
#include "LinearSystem.h"
#include "Componente.h"
#include "Netlist.h"

#define I sqrt(-1)
#define DC_RESISTANCE_C  1e9

using namespace std;
typedef complex<double> dcomp;

Netlist::Netlist(string netlistPath)
{
	string linha;
	ifstream netlistFile;
	unsigned index = 0;
   
	SistemaLinear.extraRows = 0;

	netlistFile.open(netlistPath);
	if (!netlistFile)
	{
		cout << "Nao foi possivel abrir o arquivo" << endl;
		system("pause");
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
		{
			string nomeR;
			Resistor *r = new Resistor;
			nomeR = lineParameters[0];
			(r->addType)(nomeR[0]);
			nomeR.erase(nomeR.begin()); // remove the first letter, the component identifier
			(r->setName)(nomeR);
			(r->addNode)(stoul(lineParameters[1]));
			(r->addNode)(stoul(lineParameters[2]));
			for (int i = 1; i < 3; i++)
				checkNewNode(stoul(lineParameters[i]));
			(r->setValue)(stod(lineParameters[3]));
			componentes.push_back(r);
		}
		break;

		case 'L': //nao sei se est� correto
		{
			string nomeL;
			Indutor *l = new Indutor;
			nomeL = lineParameters[0];
			(l->addType)(nomeL[0]);
			nomeL.erase(nomeL.begin()); // remove the first letter, the component identifier
			(l->setName)(nomeL);
			(l->addNode)(stoul(lineParameters[1]));
			(l->addNode)(stoul(lineParameters[2]));
			for (int i = 1; i < 3; i++)
				checkNewNode(stoul(lineParameters[i]));
			(l->setValue)(stod(lineParameters[3]));
			//(l->setInitialValue)(stod(lineParameters[4])); isso nao existe
			componentes.push_back(l);
		}
		break;

		case 'K': //nao esta contando os nos
		{
			string firstIndutor, secondIndutor, nomeK;
			Transformador *t = new Transformador;
			unsigned achouAmbos = 0;
			unsigned count, countAuxiliar = 0;
			vector<Componente*> auxiliar;
			unsigned firstIndutorPosition, secondIndutorPosition;

			firstIndutor = lineParameters[1];
			secondIndutor = lineParameters[2];

			nomeK = lineParameters[0];
			(t->addType)(nomeK[0]);
			nomeK.erase(nomeK.begin()); // remove the first letter, the component identifier
			(t->setName)(nomeK);

			while (!achouAmbos && count != sizeof(componentes))
			{
				unsigned achou1, achou2 = 0;
				if (componentes[count]->getName() == firstIndutor)
				{
					achou1 = 1;
					(t->addNode)(componentes[count]->getNode(1));
					(t->addNode)(componentes[count]->getNode(2));
					(t->setValueFirstIndutor)(componentes[count]->getValue());
					(t->setValueM)(stod(lineParameters[3]));
					firstIndutorPosition = count;
				}

				if ((componentes[count]->getName()) == secondIndutor)
				{
					achou2 = 1;
					(t->addNode)(componentes[count]->getNode(1));
					(t->addNode)(componentes[count]->getNode(2));
					(t->setValueSecondIndutor)(componentes[count]->getValue());
					secondIndutorPosition = count;
				}
				if (achou1 == 1 && achou2 == 1)
					achouAmbos = 1;

				count = count + 1;
			}

			count = 0;
			countAuxiliar = 0;
			while (count != (sizeof(componentes) - 1))
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
		}
		break;

		case 'C':
		{
			string nomeC;
			Capacitor *c = new Capacitor;
			nomeC = lineParameters[0];
			(c->addType)(nomeC[0]);
			nomeC.erase(nomeC.begin()); // remove the first letter, the component identifier
			(c->setName)(nomeC);
			(c->addNode)(stoul(lineParameters[1]));
			(c->addNode)(stoul(lineParameters[2]));
			for (int i = 1; i < 3; i++)
				checkNewNode(stoul(lineParameters[i]));
			(c->setValue)(stod(lineParameters[3]));
			//(c->setInitialValue)(stod(lineParameters[4])); isso aqui nao existe
			componentes.push_back(c);
		}
		break;

		case 'E':
		{
			string nomeE;
			VoltageSrcCntrlVoltage *e = new VoltageSrcCntrlVoltage;
			nomeE = lineParameters[0];
			(e->addType)(nomeE[0]);
			nomeE.erase(nomeE.begin()); // remove the first letter, the component identifier
			(e->setName)(nomeE);
			(e->addNode)(stoul(lineParameters[1]));
			(e->addNode)(stoul(lineParameters[2]));
			(e->addNode)(stoul(lineParameters[3]));
			(e->addNode)(stoul(lineParameters[4]));
			for (int i = 1; i < 5; i++)
				checkNewNode(stoul(lineParameters[i]));
			(e->setValue)(stod(lineParameters[5]));
			SistemaLinear.extraRows += 1;
			(e->SetExtraPosition)(SistemaLinear.extraRows);
			componentes.push_back(e);
		}
		break;

		case 'F':
		{
			string nomeF;
			CurrentSrcCntrlCurrent *f = new CurrentSrcCntrlCurrent;
			nomeF = lineParameters[0];
			(f->addType)(nomeF[0]);
			nomeF.erase(nomeF.begin()); // remove the first letter, the component identifier
			(f->setName)(nomeF);
			(f->addNode)(stoul(lineParameters[1]));
			(f->addNode)(stoul(lineParameters[2]));
			(f->addNode)(stoul(lineParameters[3]));
			(f->addNode)(stoul(lineParameters[4]));
			for (int i = 1; i < 5; i++)
				checkNewNode(stoul(lineParameters[i]));
			(f->setValue)(stod(lineParameters[5]));
			SistemaLinear.extraRows += 1;
			(f->SetExtraPosition)(SistemaLinear.extraRows);
			componentes.push_back(f);
		}
		break;

		case 'G':
		{
			string nomeG;
			CurrentSrcCntrlVoltage *g = new CurrentSrcCntrlVoltage;
			nomeG = lineParameters[0];
			(g->addType)(nomeG[0]);
			nomeG.erase(nomeG.begin()); // remove the first letter, the component identifier
			(g->setName)(nomeG);
			(g->addNode)(stoul(lineParameters[1]));
			(g->addNode)(stoul(lineParameters[2]));
			(g->addNode)(stoul(lineParameters[3]));
			(g->addNode)(stoul(lineParameters[4]));
			for (int i = 1; i < 5; i++)
				checkNewNode(stoul(lineParameters[i]));
			(g->setValue)(stod(lineParameters[5]));
			componentes.push_back(g);
		}
		break;

		case 'H':
		{
			string nomeH;
			VoltageSrcCntrlCurrent *h = new VoltageSrcCntrlCurrent;
			nomeH = lineParameters[0];
			(h->addType)(nomeH[0]);
			nomeH.erase(nomeH.begin()); // remove the first letter, the component identifier
			(h->setName)(nomeH);
			(h->addNode)(stoul(lineParameters[1]));
			(h->addNode)(stoul(lineParameters[2]));
			(h->addNode)(stoul(lineParameters[3]));
			(h->addNode)(stoul(lineParameters[4]));
			for (int i = 1; i < 5; i++)
				checkNewNode(stoul(lineParameters[i]));
			(h->setValue)(stod(lineParameters[5]));
			SistemaLinear.extraRows += 1;
			(h->SetExtraPosition)(SistemaLinear.extraRows);
			SistemaLinear.extraRows += 1;
			(h->SetExtraPosition)(SistemaLinear.extraRows);
			componentes.push_back(h);
		}
		break;

		case 'I': // fonte de corrente independente
		{
			string nomeI;
			CurrentSource *i = new CurrentSource;
			nomeI = lineParameters[0];
			(i->addType)(nomeI[0]);
			nomeI.erase(nomeI.begin()); // remove the first letter, the component identifier
			(i->setName)(nomeI);
			(i->addNode)(stoul(lineParameters[1]));
			(i->addNode)(stoul(lineParameters[2]));
			for (int i = 1; i < 3; i++)
				checkNewNode(stoul(lineParameters[i]));
			(i->setValue)(stod(lineParameters[3]));
			(i->setPhase)(stod(lineParameters[4]));
			(i->setDCValue)(stod(lineParameters[5]));
			componentes.push_back(i);
		}
		break;

		case 'V': // fonte de corrente independente
		{
			string nomeV;
			VoltageSource *v = new VoltageSource;
			nomeV = lineParameters[0];
			(v->addType)(nomeV[0]);
			nomeV.erase(nomeV.begin()); // remove the first letter, the component identifier
			(v->setName)(nomeV);
			(v->addNode)(stoul(lineParameters[1]));
			(v->addNode)(stoul(lineParameters[2]));
			for (int i = 1; i < 3; i++)
				checkNewNode(stoul(lineParameters[i]));
			(v->setValue)(stod(lineParameters[3]));
			
			if (lineParameters.size() == 6) //algumas netlists nao colocam todos os termos. Precisamos tratar
			{
				(v->setPhase)(stod(lineParameters[4]));
				(v->setDCValue)(stod(lineParameters[5]));
			}
			else if (lineParameters.size() == 5)
			{
				(v->setPhase)(stod(lineParameters[4]));
				(v->setDCValue)(0);
			}
			else
			{
				(v->setPhase)(0);
				(v->setDCValue)(0);
			}
				
			
			SistemaLinear.extraRows += 1;
			(v->SetExtraPosition)(SistemaLinear.extraRows);
			componentes.push_back(v);
		}
		break;
		


		case 'O':
		{
			string nomeO;
			AmpOp *o = new AmpOp;
			nomeO = lineParameters[0];
			(o->addType)(nomeO[0]);
			nomeO.erase(nomeO.begin()); // remove the first letter, the component identifier
			(o->setName)(nomeO);
			(o->addNode)(stoul(lineParameters[1]));
			(o->addNode)(stoul(lineParameters[2]));
			(o->addNode)(stoul(lineParameters[3]));
			(o->addNode)(stoul(lineParameters[4]));
			for(int i = 1; i < 5; i++)
				checkNewNode(stoul(lineParameters[i]));
			SistemaLinear.extraRows += 1;
			(o->SetExtraPosition)(SistemaLinear.extraRows);
			componentes.push_back(o);
		}
		break;

		case 'M':
		{
			string nomeM;
			Mosfet *m = new Mosfet;
			nomeM = lineParameters[0];
			(m->addType)(nomeM[0]);
			nomeM.erase(nomeM.begin()); // remove the first letter, the component identifier
			(m->setName)(nomeM);
			(m->addNode)(stoul(lineParameters[1])); //drain
			(m->addNode)(stoul(lineParameters[2])); //gate
			(m->addNode)(stoul(lineParameters[3])); //source
			(m->addNode)(stoul(lineParameters[4])); //bulk
			for (int i = 1; i < 5; i++)
				checkNewNode(stoul(lineParameters[i]));
			(m->mosType) = (lineParameters[5][0]);
			(m->comprimento) = stof(lineParameters[6].substr(2)); //we need to remove 'L='
			(m->largura) = stof(lineParameters[7].substr(2));//we need to remove 'W='
			(m->k) = stof(lineParameters[8]);
			(m->Vt0) = stof(lineParameters[9]);
			(m->lambda) = stof(lineParameters[10]);
			(m->gamma) = stof(lineParameters[11]);
			(m->phi) = stof(lineParameters[12]);
			(m->Ld) = stof(lineParameters[13]);
			(m->inverteu) = false;
			componentes.push_back(m);
		}
		break;
		case '.':
		{
			ParametrosAC.stepType = lineParameters[1][0];
			ParametrosAC.steps = stof(lineParameters[2]);
			ParametrosAC.startFrequency = stof(lineParameters[3]);
			ParametrosAC.endFrequency = stof(lineParameters[4]);
		}

		
		}
     	index++;
	}

	netlistFile.close();
}

void Netlist::checkNewNode(unsigned node)
{
	bool achou = false;

	for (unsigned i = 0; i < netlistNodes.size(); i++)
	{
		if (node == netlistNodes[i])
			achou = true;
	}
	if (!achou)
		netlistNodes.push_back(node);
}

unsigned Netlist::GetNumberOfNodes()
{
	return netlistNodes.size();
}
void Netlist::PrintNodes()
{
	for (unsigned i = 0; i < netlistNodes.size(); i++)
		cout << netlistNodes[i] << endl;
	
}



void Netlist::DoConductanceMatrixDC()
{
	//PrintNodes();
	SistemaLinear.ResetG_Matrix();
	double value;

	for (unsigned count = 0; count < componentes.size(); count++)
	{
		if (componentes[count]->getType() == 'R') //funcionando
		{
			//cout << "R" << endl;
			value = 1/(componentes[count]->getValue());
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(0)] += value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(1)] += value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(1)] -= value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(0)] -= value;

		}
		else if (componentes[count]->getType() == 'L') //deve funcionar
		{
			value = DC_RESISTANCE_C;
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += 1;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] -= 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(0)] -= 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(1)] += 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += value;
		}
		// K
		else if (componentes[count]->getType() == 'C') //funcionando
		{
			//cout << "C" << endl;
			value = 1 / DC_RESISTANCE_C;
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(0)] += value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(1)] += value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(1)] -= value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(0)] -= value;
		}

		else if (componentes[count]->getType() == 'E') //deve funcionar
		{
			value = componentes[count]->getValue();
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += 1;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] -= 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(0)] -= 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(1)] += 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(2)] += value;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(3)] -= value;
		}

		else if (componentes[count]->getType() == 'F') //deve funcionar
		{
			value = componentes[count]->getValue();
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] -= value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(2)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += 1;
			SistemaLinear.G_Matrix[componentes[count]->getNode(3)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] -= 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(2)] -= 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(3)] += 1;
		}

		else if (componentes[count]->getType() == 'G') //deve funcionar
		{
			value = componentes[count]->getValue();
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(2)] += value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(3)] += value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(3)] -= value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(2)] -= value;
		
		}

		else if (componentes[count]->getType() == 'H') //deve funcionar
		{
			value = componentes[count]->getValue();
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(1)] += 1;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(1)] -= 1;
			SistemaLinear.G_Matrix[componentes[count]->getNode(2)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += 1;
			SistemaLinear.G_Matrix[componentes[count]->getNode(3)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] -= 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(1)][componentes[count]->getNode(0)] -= 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(1)][componentes[count]->getNode(1)] += 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(2)] -= 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(3)] += 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(1)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += value;
		}

	    else if (componentes[count]->getType() == 'I') //deve funcionar
		{
			value = componentes[count]->getValue();
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][SistemaLinear.GetRows() + SistemaLinear.extraRows + 1] -= value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][SistemaLinear.GetRows() + SistemaLinear.extraRows + 1] += value;
		}

		else if (componentes[count]->getType() == 'V') //funcionando
		{
			//cout << "V" << endl;
			value = static_cast<VoltageSource *>(componentes[count])->getDCValue(); //precisa desse cast para acessar o m�todo da Voltage Source
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += 1;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] -= 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(0)] -= 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(1)] += 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][SistemaLinear.GetRows() + SistemaLinear.extraRows + 1] -= value;
		}

		else if (componentes[count]->getType() == 'O') //funcionando
		{
			//cout << "o" << endl;
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += 1;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] -= 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(2)] += 1;
			SistemaLinear.G_Matrix[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(3)] -= 1;
		}
		else if (componentes[count]->getType() == 'M') //deve funcionar se deus quiser
		{
			static_cast<Mosfet *>(componentes[count])->setPolarization(SistemaLinear.lastVariables[componentes[count]->getNode(0)],
																	   SistemaLinear.lastVariables[componentes[count]->getNode(1)],
																	   SistemaLinear.lastVariables[componentes[count]->getNode(2)],
																	   SistemaLinear.lastVariables[componentes[count]->getNode(3)]);

			static_cast<Mosfet *>(componentes[count])->setLinearParameters();

			double gmb = static_cast<Mosfet *>(componentes[count])->Gmb;
			double gm = static_cast<Mosfet *>(componentes[count])->Gm;
			double i = static_cast<Mosfet *>(componentes[count])->I0;
			double gds = static_cast<Mosfet *>(componentes[count])->Gds;

			//Gmb transconductance
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(3)] += gmb;
			SistemaLinear.G_Matrix[componentes[count]->getNode(2)][componentes[count]->getNode(2)] += gmb;
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(2)] -= gmb;
			SistemaLinear.G_Matrix[componentes[count]->getNode(2)][componentes[count]->getNode(3)] -= gmb;
			//Gm transconductance
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(1)] += gm;
			SistemaLinear.G_Matrix[componentes[count]->getNode(2)][componentes[count]->getNode(2)] += gm;
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(2)] -= gm;
			SistemaLinear.G_Matrix[componentes[count]->getNode(2)][componentes[count]->getNode(1)] -= gm;
			//I0
			if(static_cast<Mosfet *>(componentes[count])->mosType == 'N' )
			{ 
				SistemaLinear.G_Matrix[componentes[count]->getNode(0)][SistemaLinear.GetRows() + SistemaLinear.extraRows + 1] -= i;
				SistemaLinear.G_Matrix[componentes[count]->getNode(2)][SistemaLinear.GetRows() + SistemaLinear.extraRows + 1] += i;
			}
			else
			{
				SistemaLinear.G_Matrix[componentes[count]->getNode(0)][SistemaLinear.GetRows() + SistemaLinear.extraRows + 1] += i;
				SistemaLinear.G_Matrix[componentes[count]->getNode(2)][SistemaLinear.GetRows() + SistemaLinear.extraRows + 1] -= i;
			}
			// GDS conductance
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(0)] += gds;
			SistemaLinear.G_Matrix[componentes[count]->getNode(2)][componentes[count]->getNode(2)] += gds;
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(2)] -= gds;
			SistemaLinear.G_Matrix[componentes[count]->getNode(2)][componentes[count]->getNode(0)] -= gds;

			
			value = 1 / DC_RESISTANCE_C;
			//CGD
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(0)] += value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(1)] += value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(0)][componentes[count]->getNode(1)] -= value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(0)] -= value;

			//CGS
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(1)] += value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(2)][componentes[count]->getNode(2)] += value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(2)] -= value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(2)][componentes[count]->getNode(1)] -= value;

			//CBG
			SistemaLinear.G_Matrix[componentes[count]->getNode(3)][componentes[count]->getNode(3)] += value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(1)] += value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(1)][componentes[count]->getNode(3)] -= value;
			SistemaLinear.G_Matrix[componentes[count]->getNode(3)][componentes[count]->getNode(1)] -= value;



		}

		//SistemaLinear.PrintG_Matrix();
		//system("pause");
    
	}
	

}

void Netlist::DoConductanceMatrixAC()
{
	SistemaLinear.ResetG_MatrixAC();
	dcomp value;

	for (unsigned count = 0; count < componentes.size(); count++)
	{
		if (componentes[count]->getType() == 'R') 
		{
			value = 1 / (componentes[count]->getValue());
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(0)][componentes[count]->getNode(0)] += value;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(1)][componentes[count]->getNode(1)] += value;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(0)][componentes[count]->getNode(1)] -= value;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(1)][componentes[count]->getNode(0)] -= value;

		}
		else if (componentes[count]->getType() == 'L') //MUDAR
		{
			value = (componentes[count]->getValue())*frequency; //falta colocar o J
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(0)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += 1;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(1)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] -= 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(0)] -= 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(1)] += 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += value;
		}
		// K
		else if (componentes[count]->getType() == 'C') //MUDAR
		{
			value = 1 / DC_RESISTANCE_C;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(0)][componentes[count]->getNode(0)] += value;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(1)][componentes[count]->getNode(1)] += value;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(0)][componentes[count]->getNode(1)] -= value;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(1)][componentes[count]->getNode(0)] -= value;
		}

		else if (componentes[count]->getType() == 'E')
		{
			value = componentes[count]->getValue();
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(0)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += 1;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(1)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] -= 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(0)] -= 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(1)] += 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(2)] += value;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(3)] -= value;
		}

		else if (componentes[count]->getType() == 'F')
		{
			value = componentes[count]->getValue();
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(0)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += value;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(1)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] -= value;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(2)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += 1;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(3)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] -= 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(2)] -= 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(3)] += 1;
		}

		else if (componentes[count]->getType() == 'G')
		{
			value = componentes[count]->getValue();
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(0)][componentes[count]->getNode(2)] += value;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(1)][componentes[count]->getNode(3)] += value;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(0)][componentes[count]->getNode(3)] -= value;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(1)][componentes[count]->getNode(2)] -= value;

		}

		else if (componentes[count]->getType() == 'H') 
		{
			value = componentes[count]->getValue();
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(0)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(1)] += 1;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(1)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(1)] -= 1;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(2)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += 1;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(3)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] -= 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(1)][componentes[count]->getNode(0)] -= 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(1)][componentes[count]->getNode(1)] += 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(2)] -= 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(3)] += 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(1)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += value;
		}

		else if (componentes[count]->getType() == 'I') //MUDAR
		{
			value = componentes[count]->getValue();
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(0)][SistemaLinear.GetRows() + SistemaLinear.extraRows + 1] -= value;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(1)][SistemaLinear.GetRows() + SistemaLinear.extraRows + 1] += value;
		}

		else if (componentes[count]->getType() == 'V') //MUDAR
		{

			value = static_cast<VoltageSource *>(componentes[count])->getDCValue(); //precisa desse cast para acessar o m�todo da Voltage Source
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(0)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += 1;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(1)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] -= 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(0)] -= 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(1)] += 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][SistemaLinear.GetRows() + SistemaLinear.extraRows + 1] -= value;
		}

		else if (componentes[count]->getType() == 'O') 
		{
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(0)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] += 1;
			SistemaLinear.G_MatrixAC[componentes[count]->getNode(1)][SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)] -= 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(2)] += 1;
			SistemaLinear.G_MatrixAC[SistemaLinear.GetRows() + componentes[count]->GetExtraPosition(0)][componentes[count]->getNode(3)] -= 1;
		}

		// M

	}


}


void Netlist::NewtonRaphson()
{
	unsigned attempts = 0;
	convergiu = false;
	cout << "LAST VARIABLES" << endl;
	for (unsigned i = 1; i < SistemaLinear.lastVariables.size(); i++)
		cout << SistemaLinear.lastVariables[i]<<endl;


	while (attempts < NR_ATTEMPTS && !convergiu)
	{
		NewtonRaphsonPrint();
		for (int i = 0; (i < 40 && !convergiu); i++)
		{
			DoConductanceMatrixDC();
			SistemaLinear.SolveLinearSystem();
			NewtonRaphsonError();
			NewtonRaphsonPrint();
			system("pause");
			if (fabs(SistemaLinear.maxError) < NR_TOLERANCE)
			{
				convergiu = true;
			}
			SistemaLinear.lastVariables = SistemaLinear.variables;
			SistemaLinear.lastVariables[0] = 0;
		}
		if (!convergiu) //randomize lastVariables
		{
			attempts++;
			NewtonRaphsonRandomizeVariables();
		}

	}
}

void Netlist::NewtonRaphsonError()
{
	SistemaLinear.maxError = 0;

	for (unsigned i = 1; i < SistemaLinear.variables.size(); i++)
	{
		SistemaLinear.error[i] = SistemaLinear.variables[i] - SistemaLinear.lastVariables[i];
		if (fabs(SistemaLinear.variables[i]) > NR_RELATIVE_ABSOLUTE_TRESHOLD)
		{
			SistemaLinear.error[i] = fabs(SistemaLinear.error[i]) / SistemaLinear.variables[i];

			if (fabs(SistemaLinear.error[i]) > fabs(SistemaLinear.maxError))
				SistemaLinear.maxError = SistemaLinear.error[i];

		}
		else
			if (fabs(SistemaLinear.error[i]) > fabs(SistemaLinear.maxError))
				SistemaLinear.maxError = SistemaLinear.error[i];
		//cout << "maxError: " << SistemaLinear.maxError << endl;

	}

}

void Netlist::NewtonRaphsonRandomizeVariables() //only randomize the big errors
{
	int i = 0;
	for (; i < SistemaLinear.rows; i++) // tens�es de -10 a 10
		if (fabs(SistemaLinear.error[i]) > NR_TOLERANCE)
			SistemaLinear.lastVariables[i] = (((rand() % 2000) / 100.0) - 10);
	for (unsigned j = i; j < SistemaLinear.lastVariables.size(); j++) // correntes de - 1 a 1
		if (fabs(SistemaLinear.error[i]) > NR_TOLERANCE)
			SistemaLinear.lastVariables[i] = (((rand() % 200) / 100.0) - 1);
}
void Netlist::NewtonRaphsonPrint()
{
	for (unsigned i = 1; i < SistemaLinear.variables.size(); i++)
	{
		cout << "Tensao ";
		cout << i;
		cout << ": ";
		cout << SistemaLinear.variables[i];
		cout << "	";
		cout << SistemaLinear.lastVariables[i];
		cout << "	";
		cout << SistemaLinear.error[i] << endl;
	}
	

}

void Netlist::ACSweep()
{
	SistemaLinear.SaveDC();
	SistemaLinear.InitializeG_MatrixAC();
	// for inicio ate o final
	//monta estampa AC
	//Resolve sistema
	//salva variaveis

}

