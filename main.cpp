#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <math.h>
#include <fstream>
using namespace std;
int c1 = 0, n1 = 0, cn = 0;
vector<vector<string>> paths;
deque <vector<string>> paths_2;

struct Node {
	string name;
	vector<int> ins;
	vector<int> outs;
};
struct transistor {
	string name;
	string type;
	vector<string> drain;
	vector<string> source;
	vector<string> gate;
	int W;
	int L;
	float size = 0;
};
struct operations {
	string optype;
	vector<Node> inp;
	string out;
	Node node;
};

bool validate(string);
vector<operations> exptransform(string);
void invert(vector<transistor>&, operations);
void AND_1(vector<transistor>&, operations, vector<operations>&, int);
void OR_1(vector<transistor>&, operations, vector<operations>&, int);
void AND_1_PDN(vector<transistor>&, operations, vector<operations>&, int);
void OR_1_PDN(vector<transistor>&, operations, vector<operations>&, int);
void connect(vector<transistor>&, string);
vector<operations> generatepdn(string);
bool check_y(transistor);
void recurse(vector <transistor>, string, vector<string>);
void calculate(vector <transistor>&, vector<transistor>&, float, float);
void sizing(vector <transistor>&, vector<transistor>&, float p, float n, string, string);
void inv_1(string, vector<transistor>&, vector<transistor>&);

int main()
{
	//INPUT
	string input, path = "output.txt";
	float p , n , l;
	cout << "input your expression: y=";
	cin >> input;
	while (!validate(input))
	{
		cout << "invalid input, re-enter: y=";
		cin >> input;
	}

	cout << "enter p,n and L: ";
	cin >> p >> n >> l;

	ofstream outputfile;
	outputfile.open(path);

	if (outputfile.fail())
		cout << "failed to open file" << endl;


	//PUN
	vector<operations> op;
	op = exptransform(input);
	vector<transistor> circuit, circuitPDN;
	if (input.size() == 2)
		inv_1(input, circuit, circuitPDN);

	cout << endl << endl;
	/*
	for (int i = 0; i < op.size(); i++)
	{
	cout << op[i].optype << endl;
	for (int j = 0; j < op[i].inp.size(); j++)
	cout << op[i].inp[j].name << "    ";
	cout << endl;
	cout << op[i].out << endl << endl;
	}
	*/

	for (int s = 0; s < op.size(); s++)
	{
		if (op[s].optype == "and")
			AND_1(circuit, op[s], op, s);
		else
		if (op[s].optype == "or")
			OR_1(circuit, op[s], op, s);
		else
		if (op[s].optype == "inv")
			invert(circuit, op[s]);
	}

	string end_pun, end_pdn;
	connect(circuit, "PUN");

	//PDN
	vector<operations>opPDN;
	//cout << input<<endl;
	opPDN = generatepdn(input);

	/*	for (int i = 0; i < opPDN.size(); i++)
	{
	cout << opPDN[i].optype << endl;
	for (int j = 0; j < opPDN[i].inp.size(); j++)
	cout << opPDN[i].inp[j].name << "    ";
	cout << endl;
	cout << opPDN[i].out << endl << endl;
	}*/
	for (int s = 0; s < opPDN.size(); s++)
	{
		if (opPDN[s].optype == "and")
			AND_1_PDN(circuitPDN, opPDN[s], opPDN, s);
		else
		if (opPDN[s].optype == "or")
			OR_1_PDN(circuitPDN, opPDN[s], opPDN, s);
		else
		if (opPDN[s].optype == "inv")
			invert(circuitPDN, opPDN[s]);
	}
	connect(circuitPDN, "PDN");

	end_pun = circuit[circuit.size() - 1].name;



	sizing(circuit, circuitPDN, p, n, end_pun, end_pdn);


	for (int i = 0; i < circuit.size(); i++)
	if (circuit[i].type == "Pmos")
	{
		if (circuit[i].size < p)
			circuit[i].size = p;
	}
	else
	{
		if (circuit[i].size < n)
			circuit[i].size = n;
	}


	for (int i = 0; i < circuitPDN.size(); i++)
	if (circuitPDN[i].type == "Pmos")
	{
		if (circuitPDN[i].size < p)
			circuit[i].size = p;
	}
	else
	{
		if (circuitPDN[i].size < n)
			circuitPDN[i].size = n;
	}


	cout << endl << endl << "PUN:" << endl;
	outputfile << endl << endl << "PUN:" << endl;
	for (int i = 0; i < circuit.size(); i++)
	{
		cout << "name: " << circuit[i].name << "       type: " << circuit[i].type;
		cout << "       W= " << circuit[i].size*l << "       L= " << l << "       W/L= " << circuit[i].size << endl;
		cout << "source: ";

		outputfile << "name: " << circuit[i].name << "       type: " << circuit[i].type;
		outputfile << "       W= " << circuit[i].size*l << "       L= " << l << "       W/L= " << circuit[i].size << endl;
		outputfile << "source: ";

		for (int j = 0; j < circuit[i].source.size(); j++)
		{
			cout << circuit[i].source[j] << "    ";
			outputfile << circuit[i].source[j] << "    ";
		}
		cout << endl << "drain: ";
		outputfile << endl << "drain: ";
		for (int j = 0; j < circuit[i].drain.size(); j++)
		{
			cout << circuit[i].drain[j] << "    ";
			outputfile << circuit[i].drain[j] << "    ";
		}
		cout << endl << "gate: ";
		outputfile << endl << "gate: ";
		for (int j = 0; j < circuit[i].gate.size(); j++)
		{
			cout << circuit[i].gate[j] << "    ";
			outputfile << circuit[i].gate[j] << "    ";
		}
		cout << endl << "body: ";
		outputfile << endl << "body: ";
		for (int j = 0; j < circuit[i].source.size(); j++)
		{
			cout << circuit[i].source[j] << "    ";
			outputfile << circuit[i].source[j] << "    ";
		}
		cout << endl << endl;
		outputfile << endl << endl;
		end_pun = circuit[i].name;
	}


	cout << endl << endl << "PDN:" << endl;
	for (int i = 0; i < circuitPDN.size(); i++)
	{
		cout << "name: " << circuitPDN[i].name << "       type: " << circuitPDN[i].type;
		cout << "       W= " << circuitPDN[i].size*l << "       L= " << l << "       W/L= " << circuitPDN[i].size << endl;
		cout << "source: ";

		outputfile << "name: " << circuitPDN[i].name << "       type: " << circuitPDN[i].type;
		outputfile << "       W= " << circuitPDN[i].size*l << "       L= " << l << "       W/L= " << circuitPDN[i].size << endl;
		outputfile << "source: ";

		for (int j = 0; j < circuitPDN[i].source.size(); j++)
		{
			cout << circuitPDN[i].source[j] << "    ";
			outputfile << circuitPDN[i].source[j] << "    ";
		}
		cout << endl << "drain: ";
		outputfile << endl << "drain: ";
		for (int j = 0; j < circuitPDN[i].drain.size(); j++)
		{
			cout << circuitPDN[i].drain[j] << "    ";
			outputfile << circuitPDN[i].drain[j] << "    ";
		}
		cout << endl << "gate: ";
		outputfile << endl << "gate: ";
		for (int j = 0; j < circuitPDN[i].gate.size(); j++)
		{
			cout << circuitPDN[i].gate[j] << "    ";
			outputfile << circuitPDN[i].gate[j] << "    ";
		}
		cout << endl << "body: ";
		outputfile << endl << "body: ";
		for (int j = 0; j < circuitPDN[i].source.size(); j++)
		{
			cout << circuitPDN[i].source[j] << "    ";
			outputfile << circuitPDN[i].source[j] << "    ";
		}
		cout << endl << endl;
		outputfile << endl << endl;
		end_pdn = circuit[i].name;
	}

	outputfile.close();
	return 0;
}
vector<operations> exptransform(string input)
{
	vector<operations> op;
	int c1 = 0;
	string exp = ""; int s = 0, s1 = 0;
	for (int i = 0; i < input.size(); i++)
	{
		if (input[i] != '|'&&input[i] != '&'&&input[i] != '^'
			&&input[i] != ')'&&input[i] != '(')
		if (input[i + 1] != '^')
		{
			bool found = false;
			for (int j = 0; j < op.size();j++)
			if (op[j].out == input.substr(i, 1) + "_inv")
				found = true;

			if (!found)
			{
				operations temp;
				Node n;
				temp.optype = "inv";
				n.name = input.substr(i, 1);
				temp.inp.push_back(n);
				temp.out = input.substr(i, 1) + "_inv";
				op.push_back(temp);
			}
		}
	}
	for (int i = 0; i < input.size(); i++)
	{
		if (input[i] == '&')
		{
			int brac = 0;
			for (int j = i; j >= 0; j--)
			{
				if (input[j] == ')') brac++;
				if (input[j] == '('&&brac>0) brac--;
				if (brac == 0)
				{
					if (input[j] == '|' || input[j] == '(' || j == 0)
					{
						if (j != 0)
							input = input.substr(0, j + 1) + '(' + input.substr(j + 1);
						else
							input = '(' + input.substr(j);

						j = -1;
						i++;
					}
				}
			}
			for (int j = i; j < input.size(); j++)
			{
				if (input[j] == '(') brac++;
				if (input[j] == ')'&&brac>0) brac--;
				if (brac == 0)
				{
					if (input[j] == '|' || input[j] == ')' || j == input.size() - 1)
					{
						if (j != input.size() - 1)
							input = input.substr(0, j) + ')' + input.substr(j);
						else
							input = input + ')';
						j = input.size();
					}
				}
			}

		}
	}

	//cout << input << endl;
	for (int i = 0; i < input.size(); i++)
	{
		if (input[i] == '(')
		{
			s = i;

			i++;
			while (input[i] != ')'&&input[i] != '(')
			{
				exp = exp + input[i];
				i++;
			}

			//	cout << exp<<endl;

			if (input[i] == ')')
			{
				Node n;

				operations temp;
				temp.optype = "null";
				for (int k = 0; k < exp.size(); k++)
				{
					if (exp[k] == '|')
					{
						temp.optype = "or";
					}
					else
					if (exp[k] == '&')
					{
						temp.optype = "and";
					}
					else
					{
						string t = "";
						for (int l = k; exp[l] != '|'&&exp[l] != '&'&&l<exp.size(); l++)
						{
							t = t + exp[l]; k = l;
						}

						if (t.size() == 1)
						{
							n.name = t + "_inv";
							temp.inp.push_back(n);
						}
						else
						if (t[t.size() - 1] == '^')
						{
							n.name = t.substr(0, t.size() - 1);
							temp.inp.push_back(n);
						}
						else
						{
							n.name = t;
							temp.inp.push_back(n);
						}

					}
				}
				temp.out = "out_" + to_string(c1);
				if (temp.optype != "null")
				{
					op.push_back(temp);
				}
				else
					c1--;
				input = input.substr(0, s) + "out_" + to_string(c1++) + input.substr(i + 1, input.size() - i - 1);
				i = input.find_last_of(to_string(c1));

				//	cout << exp << endl;
			}
			else
				i--;
		}
		exp = "";
	}

	//	cout << input;
	exp = input;
	Node n;
	operations temp;
	temp.optype = "null";
	for (int k = 0; k < exp.size(); k++)
	{
		if (exp[k] == '|')
		{
			temp.optype = "or";
		}
		else
		if (exp[k] == '&')
		{
			temp.optype = "and";
		}
		else
		{
			string t = "";
			for (int l = k; exp[l] != '|'&&exp[l] != '&'&&l<exp.size(); l++)
			{
				t = t + exp[l]; k = l;
			}

			if (t.size() == 1)
			{
				n.name = t + "_inv";
				temp.inp.push_back(n);
			}
			else
			if (t[t.size() - 1] == '^')
			{
				n.name = t.substr(0, t.size() - 1);
				temp.inp.push_back(n);
				//temp.inp.push_back(t.substr(0, t.size() - 1));
			}
			else
			{
				n.name = t;
				temp.inp.push_back(n);
			}

		}
	}
	temp.out = "out_" + to_string(c1);
	if (temp.optype != "null")
		op.push_back(temp);


	return op;
}
void invert(vector<transistor>& circuit, operations operate)
{
	transistor temp;
	temp.name = "M_" + to_string(n1++);
	temp.type = "Pmos";
	temp.source.push_back("Vdd");
	temp.gate.push_back(operate.inp[0].name);
	temp.drain.push_back(operate.out);
	temp.drain.push_back("M_" + to_string(n1) + "_drain");
	temp.W = 0; temp.L = 0;


	transistor temp1;
	temp1.name = "M_" + to_string(n1++);
	temp1.type = "Nmos";
	temp1.drain.push_back(operate.out);
	temp1.drain.push_back("M_" + to_string(n1 - 2) + "_drain");
	temp1.gate.push_back(operate.inp[0].name);
	temp1.source.push_back("gnd");
	temp1.W = 0; temp1.L = 0;

	bool t1 = true, t2 = true;
	for (int i = 0; i < circuit.size(); i++)
	{
		if (circuit[i].gate[0] == temp.gate[0])
			t1 = false;

		if (circuit[i].gate[0] == temp1.gate[0])
			t2 = false;


	}
	if (t1)circuit.push_back(temp);
	if (t2)circuit.push_back(temp1);

}
void AND_1_PDN(vector<transistor>& circuit, operations operate, vector<operations>& op, int m)
{
	int t = 0, t1 = 0; //cases wires/inputs
	for (int i = 0; i < operate.inp.size(); i++)
	{
		if (operate.inp[i].name.find("out") != std::string::npos)
			t++;
		else
			t1++;
	}

	if (t == 0)
	{
		transistor temp;
		bool t = false;
		temp.name = "M_" + to_string(n1++);
		temp.type = "Nmos";
		temp.drain.push_back(operate.out + "_in");
		temp.gate.push_back(operate.inp[0].name);
		temp.source.push_back("M_" + to_string(n1) + "_drain");
		temp.W = 0; temp.L = 0;
		circuit.push_back(temp);
		op[m].node.ins.push_back(circuit.size() - 1);

		for (int i = 1; i < operate.inp.size(); i++)
		{
			transistor temp;
			temp.name = "M_" + to_string(n1++);
			temp.type = "Nmos";
			if (i != operate.inp.size() - 1)
			{
				temp.source.push_back("M_" + to_string(n1) + "_drain");
			}
			else
			{
				temp.source.push_back(operate.out);
			}
			temp.gate.push_back(operate.inp[i].name);
			temp.drain.push_back("M_" + to_string(n1 - 2) + "_src");
			temp.W = 0; temp.L = 0;
			circuit.push_back(temp);
		}
		op[m].node.name = op[m].out;
		op[m].node.outs.push_back(circuit.size() - 1);
	}
	else
	{
		//updates the inputs and outputs of complex inputs like out_1
		for (int i = 0; i < op.size(); i++)
		{
			for (int k = 0; k < op[m].inp.size(); k++)
			{
				if (op[m].inp[k].name == op[i].out)
				{
					for (int t = 0; t < op[i].node.ins.size(); t++)
						op[m].inp[k].ins.push_back(op[i].node.ins[t]);
					for (int t = 0; t < op[i].node.outs.size(); t++)
						op[m].inp[k].outs.push_back(op[i].node.outs[t]);
				}
			}
		}
		if (t1 > 0 && t > 0)
		{
			bool t = false;
			for (int k = 0; k < op[m].inp.size(); k++)
			{
				if (op[m].inp[k].ins.size() == 0)
				{
					transistor temp;
					bool t = false;
					temp.name = "M_" + to_string(n1++);
					temp.type = "Nmos";
					if (k == 0)
						temp.drain.push_back(operate.out + "_in");
					else
						temp.drain.push_back("M_" + to_string(n1 - 2) + "_src");
					temp.gate.push_back(operate.inp[k].name);
					if (k != operate.inp.size() - 1)
						temp.source.push_back("M_" + to_string(n1) + "_drain");
					else
						temp.source.push_back(operate.out);
					temp.W = 0; temp.L = 0;
					circuit.push_back(temp);
					op[m].inp[k].ins.push_back(circuit.size() - 1);
					op[m].inp[k].outs.push_back(circuit.size() - 1);
				}
			}
		}
		for (int i = 0; i < operate.inp.size() - 1; i++)
		{
			// connecting the source of the inp[i] to drain of inp[i+1]
			for (int j = 0; j < op[m].inp[i].outs.size(); j++)
			{
				if (circuit[op[m].inp[i].outs[j]].source.size() > 0)
					circuit[op[m].inp[i].outs[j]].source.pop_back();
				for (int t = 0; t < op[m].inp[i + 1].ins.size(); t++)
				{
					circuit[op[m].inp[i].outs[j]].source.push_back(circuit[op[m].inp[i + 1].ins[t]].name + "_drain");
				}
			}

			// connecting the drain of the inp[i+1] to source of inp[i]
			for (int j = 0; j < op[m].inp[i + 1].ins.size(); j++)
			{
				if (circuit[op[m].inp[i + 1].ins[j]].drain.size() > 0)
					circuit[op[m].inp[i + 1].ins[j]].drain.pop_back();
				for (int t = 0; t < op[m].inp[i].outs.size(); t++)
				{
					circuit[op[m].inp[i + 1].ins[j]].drain.push_back(circuit[op[m].inp[i].outs[t]].name + "_src");
				}
			}
		}
		//saving the inputs and the outputs of this operation
		for (int d = 0; d < op[m].inp[0].ins.size(); d++)
		{
			op[m].node.ins.push_back(op[m].inp[0].ins[d]);
		}
		for (int d = 0; d < op[m].inp[op[m].inp.size() - 1].outs.size(); d++)
		{
			op[m].node.outs.push_back(op[m].inp[op[m].inp.size() - 1].outs[d]);
		}
	}
}
void OR_1_PDN(vector<transistor>& circuit, operations operate, vector<operations>& op, int m)
{
	int t = 0, t1 = 0; //cases wires/input
	for (int i = 0; i < operate.inp.size(); i++)
	{
		if (operate.inp[i].name.find("out") != std::string::npos)
			t++;
		else
			t1++;
	}

	if (t == 0)
	{
		for (int i = 0; i < operate.inp.size(); i++)
		{
			transistor temp;
			temp.name = "M_" + to_string(n1++);
			temp.type = "Nmos";
			temp.drain.push_back(operate.out + "_in");
			temp.gate.push_back(operate.inp[i].name);
			temp.source.push_back(operate.out);
			temp.W = 0; temp.L = 0;
			circuit.push_back(temp);
			op[m].node.ins.push_back(circuit.size() - 1);
			op[m].node.outs.push_back(circuit.size() - 1);
			op[m].node.name = op[m].out;
		}
	}
	else
	{
		for (int i = 0; i < op.size(); i++)
		{
			for (int k = 0; k < op[m].inp.size(); k++)
			{
				if (op[m].inp[k].name == op[i].out)
				{
					for (int t = 0; t < op[i].node.ins.size(); t++)
						op[m].inp[k].ins.push_back(op[i].node.ins[t]);
					for (int t = 0; t < op[i].node.outs.size(); t++)
						op[m].inp[k].outs.push_back(op[i].node.outs[t]);
				}
			}
		}
		if (t1 > 0 && t > 0)
		{
			for (int k = 0; k < op[m].inp.size(); k++)
			{
				if (op[m].inp[k].ins.size() == 0)
				{
					transistor temp;
					temp.name = "M_" + to_string(n1++);
					temp.type = "Nmos";
					temp.drain.push_back(operate.out + "_in");
					temp.gate.push_back(operate.inp[k].name);
					temp.source.push_back(operate.out);
					temp.W = 0; temp.L = 0;
					circuit.push_back(temp);
					op[m].node.ins.push_back(circuit.size() - 1);
					op[m].node.name = op[m].out;
					op[m].node.outs.push_back(circuit.size() - 1);
				}
			}
		}
		for (int i = 0; i < operate.inp.size(); i++)
		{
			for (int d = 0; d < op[m].inp[i].ins.size(); d++)
			{
				op[m].node.ins.push_back(op[m].inp[i].ins[d]);
			}
			for (int d = 0; d < op[m].inp[i].outs.size(); d++)
			{
				op[m].node.outs.push_back(op[m].inp[i].outs[d]);
			}
		}
	}
}
void AND_1(vector<transistor>& circuit, operations operate, vector<operations>& op, int m)
{
	int t = 0, t1 = 0; //cases wires/inputs
	for (int i = 0; i < operate.inp.size(); i++)
	{
		if (operate.inp[i].name.find("out") != std::string::npos)
			t++;
		else
			t1++;
	}

	if (t == 0)
	{
		transistor temp;
		bool t = false;
		temp.name = "M_" + to_string(n1++);
		temp.type = "Pmos";
		temp.source.push_back(operate.out + "_in");
		temp.gate.push_back(operate.inp[0].name);
		temp.drain.push_back("M_" + to_string(n1) + "_src");
		temp.W = 0; temp.L = 0;
		circuit.push_back(temp);
		op[m].node.ins.push_back(circuit.size() - 1);

		for (int i = 1; i < operate.inp.size(); i++)
		{
			transistor temp;
			temp.name = "M_" + to_string(n1++);
			temp.type = "Pmos";
			if (i != operate.inp.size() - 1)
			{
				temp.drain.push_back("M_" + to_string(n1) + "_src");
			}
			else
			{
				temp.drain.push_back(operate.out);
			}
			temp.gate.push_back(operate.inp[i].name);
			temp.source.push_back("M_" + to_string(n1 - 2) + "_drain");
			temp.W = 0; temp.L = 0;
			circuit.push_back(temp);
		}
		op[m].node.name = op[m].out;
		op[m].node.outs.push_back(circuit.size() - 1);
	}
	else
	{
		//updates the inputs and outputs of complex inputs like out_1
		for (int i = 0; i < op.size(); i++)
		{
			for (int k = 0; k < op[m].inp.size(); k++)
			{
				if (op[m].inp[k].name == op[i].out)
				{
					for (int t = 0; t < op[i].node.ins.size(); t++)
						op[m].inp[k].ins.push_back(op[i].node.ins[t]);
					for (int t = 0; t < op[i].node.outs.size(); t++)
						op[m].inp[k].outs.push_back(op[i].node.outs[t]);
				}
			}
		}
		if (t1 > 0 && t > 0)
		{
			bool t = false;
			for (int k = 0; k < op[m].inp.size(); k++)
			{
				if (op[m].inp[k].ins.size() == 0)
				{
					transistor temp;
					bool t = false;
					temp.name = "M_" + to_string(n1++);
					temp.type = "Pmos";
					if (k == 0)
						temp.source.push_back(operate.out + "_in");
					else
						//if (!t)
						temp.source.push_back("M_" + to_string(n1 - 2) + "_drain");
					temp.gate.push_back(operate.inp[k].name);
					if (k != operate.inp.size() - 1)
						temp.drain.push_back("M_" + to_string(n1) + "_src");
					else
						temp.drain.push_back(operate.out);
					temp.W = 0; temp.L = 0;
					circuit.push_back(temp);
					op[m].inp[k].ins.push_back(circuit.size() - 1);
					op[m].inp[k].outs.push_back(circuit.size() - 1);
				}
			}
		}
		for (int i = 0; i < operate.inp.size() - 1; i++)
		{
			// connecting the drain of the inp[i] to source of inp[i+1]
			for (int j = 0; j < op[m].inp[i].outs.size(); j++)
			{
				if (circuit[op[m].inp[i].outs[j]].drain.size() > 0)
					circuit[op[m].inp[i].outs[j]].drain.pop_back();
				for (int t = 0; t < op[m].inp[i + 1].ins.size(); t++)
				{
					circuit[op[m].inp[i].outs[j]].drain.push_back(circuit[op[m].inp[i + 1].ins[t]].name + "_src");
				}
			}

			// connecting the source of the inp[i+1] to drain of inp[i]
			for (int j = 0; j < op[m].inp[i + 1].ins.size(); j++)
			{
				if (circuit[op[m].inp[i + 1].ins[j]].source.size() > 0)
					circuit[op[m].inp[i + 1].ins[j]].source.pop_back();
				for (int t = 0; t < op[m].inp[i].outs.size(); t++)
				{
					circuit[op[m].inp[i + 1].ins[j]].source.push_back(circuit[op[m].inp[i].outs[t]].name + "_drain");
				}
			}
		}
		//saving the inputs and the outputs of this operation
		for (int d = 0; d < op[m].inp[0].ins.size(); d++)
		{
			op[m].node.ins.push_back(op[m].inp[0].ins[d]);
		}
		for (int d = 0; d < op[m].inp[op[m].inp.size() - 1].outs.size(); d++)
		{
			op[m].node.outs.push_back(op[m].inp[op[m].inp.size() - 1].outs[d]);
		}
	}
}
void OR_1(vector<transistor>& circuit, operations operate, vector<operations>& op, int m)
{
	int t = 0, t1 = 0; //cases wires/input
	for (int i = 0; i < operate.inp.size(); i++)
	{
		if (operate.inp[i].name.find("out") != std::string::npos)
			t++;
		else
			t1++;
	}

	if (t == 0)
	{
		for (int i = 0; i < operate.inp.size(); i++)
		{
			transistor temp;
			temp.name = "M_" + to_string(n1++);
			temp.type = "Pmos";
			temp.source.push_back(operate.out + "_in");
			temp.gate.push_back(operate.inp[i].name);
			temp.drain.push_back(operate.out);
			temp.W = 0; temp.L = 0;
			circuit.push_back(temp);
			op[m].node.ins.push_back(circuit.size() - 1);
			op[m].node.outs.push_back(circuit.size() - 1);
			op[m].node.name = op[m].out;
		}
	}
	else
	{
		for (int i = 0; i < op.size(); i++)
		{
			for (int k = 0; k < op[m].inp.size(); k++)
			{
				if (op[m].inp[k].name == op[i].out)
				{
					for (int t = 0; t < op[i].node.ins.size(); t++)
						op[m].inp[k].ins.push_back(op[i].node.ins[t]);
					for (int t = 0; t < op[i].node.outs.size(); t++)
						op[m].inp[k].outs.push_back(op[i].node.outs[t]);
				}
			}
		}
		if (t1 > 0 && t > 0)
		{
			for (int k = 0; k < op[m].inp.size(); k++)
			{
				if (op[m].inp[k].ins.size() == 0)
				{
					transistor temp;
					temp.name = "M_" + to_string(n1++);
					temp.type = "Pmos";
					temp.source.push_back(operate.out + "_in");
					temp.gate.push_back(operate.inp[k].name);
					temp.drain.push_back(operate.out);
					temp.W = 0; temp.L = 0;
					circuit.push_back(temp);
					op[m].node.ins.push_back(circuit.size() - 1);
					op[m].node.name = op[m].out;
					op[m].node.outs.push_back(circuit.size() - 1);
				}
			}
		}
		for (int i = 0; i < operate.inp.size(); i++)
		{
			for (int d = 0; d < op[m].inp[i].ins.size(); d++)
			{
				op[m].node.ins.push_back(op[m].inp[i].ins[d]);
			}
			for (int d = 0; d < op[m].inp[i].outs.size(); d++)
			{
				op[m].node.outs.push_back(op[m].inp[i].outs[d]);
			}
		}
	}
}
void connect(vector<transistor>& circuit, string type)
{

	if (type == "PUN")
	{
		if (circuit.size() == 1)
			circuit[0].drain[0] = "Y";
		for (int i = 0; i < circuit.size(); i++)
		{
			for (int j = 0; j < circuit[i].source.size(); j++)
			{
				if (circuit[i].source[j].find("_in") != std::string::npos)
				{
					circuit[i].source[j] = "Vdd";
				}
			}

			for (int j = 0; j < circuit[i].drain.size(); j++)
			{
				if (circuit[i].drain[j].find("out") != std::string::npos)
				{
					circuit[i].drain[j] = "Y";
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < circuit.size(); i++)
		{
			for (int j = 0; j < circuit[i].drain.size(); j++)
			{
				if (circuit[i].drain[j].find("_in") != std::string::npos)
				{
					circuit[i].drain[j] = "Y";
				}
			}

			for (int j = 0; j < circuit[i].source.size(); j++)
			{
				if (circuit[i].source[j].find("out") != std::string::npos)
				{
					circuit[i].source[j] = "gnd";
				}
			}
		}
	}
}
vector<operations> generatepdn(string input)
{

	vector<operations> op;
	int c1 = 0;
	//	string input = "A|C&(((D|C)|B&C)&E)";
	string exp = ""; int s = 0, s1 = 0;
	/*	for (int i = 0; i < input.size(); i++)
	{
	if (input[i] != '|'&&input[i] != '&'&&input[i] != '^'
	&&input[i] != ')'&&input[i] != '(')
	if (input[i + 1] != '^')
	{
	operations temp;
	Node n;
	temp.optype = "inv";
	n.name = input.substr(i, 1);
	temp.inp.push_back(n);
	temp.out = input.substr(i, 1) + "_inv";
	op.push_back(temp);
	}
	}*/
	for (int i = 0; i < input.size(); i++)
	{
		if (input[i] == '&')
		{
			int brac = 0;
			for (int j = i; j >= 0; j--)
			{
				if (input[j] == ')') brac++;
				if (input[j] == '('&&brac>0) brac--;
				if (brac == 0)
				{
					if (input[j] == '|' || input[j] == '(' || j == 0)
					{
						if (j != 0)
							input = input.substr(0, j + 1) + '(' + input.substr(j + 1);
						else
							input = '(' + input.substr(j);

						j = -1;
						i++;
					}
				}
			}
			for (int j = i; j < input.size(); j++)
			{
				if (input[j] == '(') brac++;
				if (input[j] == ')'&&brac>0) brac--;
				if (brac == 0)
				{
					if (input[j] == '|' || input[j] == ')' || j == input.size() - 1)
					{
						if (j != input.size() - 1)
							input = input.substr(0, j) + ')' + input.substr(j);
						else
							input = input + ')';
						j = input.size();
					}
				}
			}

		}
	}

	for (int i = 0; i < input.size(); i++)
	{
		if (input[i] == '|')
			input[i] = '&';
		else
		if (input[i] == '&')
			input[i] = '|';
	}
	// I have the pull down expression now

	for (int i = 0; i < input.size(); i++)
	{
		if (input[i] == '(')
		{
			s = i;

			i++;
			while (input[i] != ')'&&input[i] != '(')
			{
				exp = exp + input[i];
				i++;
			}

			//	cout << exp<<endl;

			if (input[i] == ')')
			{
				Node n;

				operations temp;
				temp.optype = "null";
				for (int k = 0; k < exp.size(); k++)
				{
					if (exp[k] == '|')
					{
						temp.optype = "or";
					}
					else
					if (exp[k] == '&')
					{
						temp.optype = "and";
					}
					else
					{
						string t = "";
						for (int l = k; exp[l] != '|'&&exp[l] != '&'&&l<exp.size(); l++)
						{
							t = t + exp[l]; k = l;
						}

						if (t.size() == 1)
						{
							n.name = t + "_inv";
							temp.inp.push_back(n);
						}
						else
						if (t[t.size() - 1] == '^')
						{
							n.name = t.substr(0, t.size() - 1);
							temp.inp.push_back(n);
						}
						else
						{
							n.name = t;
							temp.inp.push_back(n);
						}

					}
				}
				temp.out = "out_" + to_string(c1);
				if (temp.optype != "null")
				{
					op.push_back(temp);
				}
				else
					c1--;
				input = input.substr(0, s) + "out_" + to_string(c1++) + input.substr(i + 1, input.size() - i - 1);
				i = input.find_last_of(to_string(c1));

				//	cout << exp << endl;
			}
			else
				i--;
		}
		exp = "";
	}

	//	cout << input;
	exp = input;
	Node n;
	operations temp;
	temp.optype = "null";
	for (int k = 0; k < exp.size(); k++)
	{
		if (exp[k] == '|')
		{
			temp.optype = "or";
		}
		else
		if (exp[k] == '&')
		{
			temp.optype = "and";
		}
		else
		{
			string t = "";
			for (int l = k; exp[l] != '|'&&exp[l] != '&'&&l<exp.size(); l++)
			{
				t = t + exp[l]; k = l;
			}

			if (t.size() == 1)
			{
				n.name = t + "_inv";
				temp.inp.push_back(n);
			}
			else
			if (t[t.size() - 1] == '^')
			{
				n.name = t.substr(0, t.size() - 1);
				temp.inp.push_back(n);
				//temp.inp.push_back(t.substr(0, t.size() - 1));
			}
			else
			{
				n.name = t;
				temp.inp.push_back(n);
			}

		}
	}
	temp.out = "out_" + to_string(c1);
	if (temp.optype != "null")
		op.push_back(temp);


	return op;

}
void inv_1(string input, vector<transistor>&PUN, vector<transistor>&PDN)
{
	input = input[0];

	transistor temp;
	temp.name = "M_" + to_string(n1++);
	temp.type = "Pmos";
	temp.source.push_back("Vdd");
	temp.gate.push_back(input);
	temp.drain.push_back(input + "_inv");
	temp.drain.push_back("M_" + to_string(n1) + "_drain");
	temp.W = 0; temp.L = 0;


	transistor temp1;
	temp1.name = "M_" + to_string(n1++);
	temp1.type = "Nmos";
	temp1.drain.push_back(input + "_inv");
	temp1.drain.push_back("M_" + to_string(n1 - 2) + "_drain");
	temp1.gate.push_back(input);
	temp1.source.push_back("gnd");
	temp1.W = 0; temp1.L = 0;
	/*
	bool t1 = true, t2 = true;
	for (int i = 0; i < circuit.size(); i++)
	{
	if (circuit[i].gate[0] == temp.gate[0])
	t1 = false;

	if (circuit[i].gate[0] == temp1.gate[0])
	t2 = false;


	}*/

	PUN.push_back(temp);
	PDN.push_back(temp1);


}
bool balance(string in)
{
	stack<char> b;
	bool balanced = true;
	char temp;
	for (int i = 0; i < in.length(); i++)
	{
		if (in.at(i) == '(')
			b.push(in.at(i));
		else
		{
			if (in.at(i) == ')')
			{
				if (b.empty())
				{
					balanced = false;
				}
				else
				{
					temp = b.top();
					b.pop();
					if (temp != '(')
						balanced = false;
				}
			}
		}
	}
	if (!b.empty())
	{
		balanced = false;
	}
	return balanced;
}
bool validate(string input){
	for (int i = 0; i < input.size() - 1; i++)
	{
		if (input[i] == '|' || input[i] == '&' || input[i] == '(')
		{
			if (input[i + 1] == '|' || input[i + 1] == '&' || input[i + 1] == '^')
				return false;
		}
		else
		if (input[i] == '^')
		{
			if (i == 0)
				return false;
			if (input[i - 1] == '|' || input[i - 1] == '&' || input[i - 1] == '^')
				return false;
			if (input[i + 1] != '|' && input[i + 1] != '&' &&input[i + 1] != ')')
				return false;
		}
		else
		if (int(input[i]) >= 65 && int(input[i]) <= 90 || input[i] == ')')
		{
			if (int(input[i + 1]) >= 65 && int(input[i + 1]) <= 90)
				return false;

			if (int(input[i + 1]) != '|' && int(input[i + 1]) != '&' &&int(input[i + 1]) != '^' &&int(input[i + 1]) != ')')
				return false;

			if (input[i] == 'Y')
				return false;
		}
		else
		if (input[i] == ')')
		{
			if (input[i + 1] != '|' && input[i + 1] != '&')
				return false;
		}
		else
			return false;

		if (input[input.size() - 1] == '|' || input[input.size() - 1] == '&' || input[0] == '|' || input[0] == '&')
			return false;
	}
	if (input.size() <= 1)return false;

	return balance(input) && true;

}
bool check_y(transistor for_check)
{
	for (int i = 0; i < for_check.drain.size(); i++)
	{
		string name = for_check.drain[i];
		name = name.substr(0, name.find("src") - 1);
		if (name == "Y")
			return false;
	}
	return true;
}
void recurse(vector <transistor> circuit, string required, vector<string> path)
{
	int index = 0;
	required = required.substr(0, required.find("src") - 1);
	for (int i = 0; i < circuit.size(); i++)
	if (circuit[i].name == required)
	{
		index = i; break;
	}


	for (int j = 0; j <circuit[index].drain.size(); j++)
	{
		string name = circuit[index].drain[j];
		name = name.substr(0, name.find("src") - 1);
		if (check_y(circuit[index]))
		{
			path.push_back(name);

			recurse(circuit, circuit[index].drain[j], path);

			path.pop_back();
		}
		else
		{
			paths.push_back(path);
			path.pop_back();
			break;
		}
	}
}
void calculate(vector <transistor> & circuit, vector<transistor> & circuit_pdn, float size_intial_p, float size_intial_n)
{

	for (int j = 0; j<paths.size(); j++)
	{
		int max = 0;
		int index = 0;
		for (int i = 0; i < paths.size(); i++)
		if (paths[i].size() > max)
		{
			max = paths[i].size();
			index = i;
		}
		paths_2.push_back(paths[index]);
		vector<string> sub;
		paths[index] = sub;
	}

	vector<string> max_path, max_path_2, max_path_3, max_path_4;
	max_path = paths_2.front();
	max_path_2 = max_path_3 = max_path_4 = max_path;
	string required;
	int paths_size = paths.size();
	for (int k = 0; k<paths_size; k++)
	{
		int number_assigned = 0, number_unassigned = 0;
		float total_assgined_size = 0;
		required = max_path.back();
		int path_size = max_path.size();


		for (int i = 0; i<path_size; i++)
		{
			for (int i = 0; i < circuit.size(); i++)
			if (circuit[i].name == required)
			{
				if (circuit[i].size == 0)
				{
					number_unassigned++;
				}
				else
				{
					number_assigned++;
					total_assgined_size += pow(circuit[i].size, -1);
				}
				break;
			}
			if (!max_path.empty())
			{
				max_path.pop_back();
				if (!max_path.empty())
					required = max_path.back();
			}
		}

		float newsize_p = pow(size_intial_p, -1) - total_assgined_size;

		required = max_path_2.back();
		int loop_size2 = max_path_2.size();
		for (int h = 0; h <loop_size2; h++)
		{
			for (int i = 0; i < circuit.size(); i++)
			{
				if (circuit[i].name == required)
				{
					if (circuit[i].size == 0)
					{
						circuit[i].size = pow(newsize_p, -1)*number_unassigned;
					}
					break;
				}
			}
			if (!max_path_2.empty())
			{
				max_path_2.pop_back();
				if (!max_path_2.empty())
					required = max_path_2.back();

			}
		}


		int number_assigned_n = 0, number_unassigned_n = 0;
		float total_assgined_size_n = 0;
		required = max_path_3.back();
		int path_size_n = max_path_3.size();
		for (int i = 0; i<path_size_n; i++)
		{
			for (int i = 0; i < circuit_pdn.size(); i++)
			if (circuit_pdn[i].name == required)
			{
				if (circuit_pdn[i].size == 0)
				{
					number_unassigned_n++;
				}
				else
				{
					number_assigned_n++;
					total_assgined_size_n += pow(circuit_pdn[i].size, -1);
				}
				break;
			}
			if (!max_path_3.empty())
			{
				max_path_3.pop_back();
				if (!max_path_3.empty())
					required = max_path_3.back();
			}
		}

		float newsize_n = pow(size_intial_n, -1) - total_assgined_size_n;

		required = max_path_4.back();
		int loop_size2_n = max_path_4.size();
		for (int h = 0; h <loop_size2_n; h++)
		{
			for (int i = 0; i < circuit_pdn.size(); i++)
			{
				if (circuit_pdn[i].name == required)
				{
					if (circuit_pdn[i].size == 0)
					{
						circuit_pdn[i].size = pow(newsize_n, -1)*number_unassigned_n;
					}
					break;
				}
			}
			if (!max_path_4.empty())
			{
				max_path_4.pop_back();
				if (!max_path_4.empty())
					required = max_path_4.back();
			}
		}

		if (!paths_2.empty())
		{
			paths_2.pop_front();
			if (!paths_2.empty())
			{
				max_path = paths_2.front();
				max_path_2 = max_path_3 = max_path_4 = max_path;
			}
		}
	}

}
void sizing(vector <transistor> & circuit, vector<transistor> & circuit_pdn, float p, float n, string end_pun, string end_pdn)
{
	for (int i = 0; i < circuit.size(); i++)
	{

		for (int j = 0; j < circuit[i].source.size(); j++)
		if (circuit[i].source[j] == "Vdd" && circuit[i].drain[0].find("_in") == std::string::npos&&circuit[i].type == "Pmos")
		{
			vector<string> path;
			path.push_back(circuit[i].name);
			recurse(circuit, circuit[i].name, path);
			break;
		}
	}
	for (int i = 0; i < circuit_pdn.size(); i++)
	{

		for (int j = 0; j < circuit_pdn[i].source.size(); j++)
		if (circuit_pdn[i].source[j] == "gnd" &&circuit_pdn[i].type == "Nmos")
		{
			vector<string> path;
			path.push_back(circuit_pdn[i].name);
			recurse(circuit_pdn, circuit_pdn[i].name, path);
			break;
		}
	}
	//cout << paths.size() << endl;
	//cout << paths[0].size() << endl;
	//cout << paths[0][0] << endl;
	calculate(circuit, circuit_pdn, p, n);
}