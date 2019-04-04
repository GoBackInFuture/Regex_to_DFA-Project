#include<bits/stdc++.h>
 	//vectortor macro
#include <iostream>
#include <unordered_map>
#include <string>
#include <stack>


#define pb push_back

using namespace std;

int gpos,fl_pos;		// pentru gasirea  pozitiei si follow position index
vector<int> follow_pos[30],state[30];	// follow_pos stocheaza follow position and state stocheaza nr total de Dstates
map<int,char> alpha_int;
set<char> in_alpha;

vector <int> operator +(vector <int> a,vector <int> b) 	// supraincarcare operator pt adunarea a doi vectori
{
	a.insert(a.end(),b.begin(),b.end());
	sort(a.begin(),a.end());		// necesar pentru a face un unic element
	a.erase(unique(a.begin(),a.end()),a.end()); // pentru gasirea elementului unic dintr-un vector
	return a;
}

struct tree			/// Structura nod
{
	char alpha;
	int pos;
	bool nullable;
	vector <int> fpos;
	vector <int> lpos;
	tree *left,*right;
};

///Verificare daca este caracter operator
bool is_op(char ch)
{
	if(ch == '|'  || ch == '*' || ch == '.')
		return true;
	return false;
}

///Creare nod si initializare
tree *create(char ch,int pos)
{
	tree *node = new tree;
	node->alpha = ch;
	node->pos = pos;
	node->left = node->right = NULL;
	node->lpos.clear();
	node->fpos.clear();
	return node;
}

///Afisare vector
void vector_print(vector <int> v)
{
	for(int i = 0 ; i < v.size() ; i++)
		cout<<v[i]<<" ";
}


///Traversare arbore in ordine postfixa
void postfix(tree *root)
{
	if(root)
	{
		string s("	    	");
		postfix(root->left);
		postfix(root->right);
		cout << root->alpha << s << root->pos << s << root->nullable << s <<"{ ";
		vector_print(root->fpos);
		cout << " }" << s << s << "{ ";
		vector_print(root->lpos);
		cout << " }" << endl;
	}
}

void dfa(tree *root,string input)	/// Gasirea DFA-ului
{
	int num_state = 1,cur_state = 1;
	char ch = 'A';
	vector <int> temp;
	map< vector<int> , char> out_state;	/// Out_state folosit pentru inlaturarea starilor redundante
	map< vector<int> , map< char , vector<int> > >  re_dfa;  ///memorarea DFA-ului final
	state[num_state++] = root->fpos;	/// numarul total de stari, incepand cu radacina ca fiind prima pozitie
	out_state[root->fpos] = ch++;
	while(1)
	{
		for(int i = 0 ; i < input.size() ; i++)
		{
			for(int j = 0 ; j < state[cur_state].size() ; j++)
			{
				if(alpha_int[state[cur_state][j]] == input[i])
					temp = temp + follow_pos[state[cur_state][j] ];
				if(out_state[temp] == 0 && temp.size() > 0)
				{
					out_state[temp] = ch++;
					state[num_state++] = temp;
				}
			}
			re_dfa[state[cur_state]][input[i]] = temp;
			temp.clear();
		}
		if(cur_state == num_state - 1)
			break;
		cur_state++;
	}
	///Afisare tabel DFA
	cout<<"\n\n Tabelul final DFA:\n\n";
	for(auto an : re_dfa)
	{
		cout<<"{ ";
		for(auto jn : an.first)
			cout<<jn<<" ";
		cout<<" } ";
		for(auto jn : an.second)
		{
			cout<<" la : "<<jn.first<<" { ";
			for(auto kn:jn.second)
				cout<<kn<<" ";
			cout<<" } ";
		}
		cout<<endl;
	}
}

int getPrecedence(char c,unordered_map<char,int> pmap){

    auto value = pmap.find(c);
    if(value == pmap.end())
        return 5;
    return value->second;
}

///Sterge parantezele si pune punctele (formateaza REGEX in expresie augumentata)
string formatRegex(string input, unordered_map<char,int> pmap)
{

    string format_regex = "";

    for(int i = 0; i < input.size(); i++)
    {
    char c1 = input[i];
        if( i + 1 < input.size())
        {
            char c2 = input[i + 1];
            format_regex += c1;

            if(c1 != '(' && c2 != ')' && c2 != '|' && c2 != '*' && c1 != '|')
                format_regex += '.';
        }
    }

    format_regex += input[input.size() - 1];

    return format_regex;
}

///Inversare din infix in postfix
string infixToPostfix(string input)
{

    unordered_map<char,int> precedence;
    precedence.insert( make_pair('(',1) );
    //precedence.insert( make_pair(')',1) );
    precedence.insert( make_pair('|', 2) );
    precedence.insert( make_pair('.', 3) );
    precedence.insert( make_pair('*',4) );

    stack<char> stiva;

    string formRegex = formatRegex(input, precedence);
    string postfix = "";

    for(auto c: formRegex){
        switch(c){
        case '(':
            stiva.push(c);
            break;
        case ')':
            while(stiva.top() != '(')
            {
                 postfix += stiva.top();
                 stiva.pop();
            }
            stiva.pop();
            break;
        default:
            while(stiva.size() > 0){
                char topStiva = stiva.top();

                int precTop = getPrecedence(topStiva,precedence);
                int prec = getPrecedence(c,precedence);

                if(precTop >= prec){
                    postfix += stiva.top();
                    stiva.pop();
                }
                else
                    break;

            }
            stiva.push(c);
            break;
        }
    }
    while(stiva.size() > 0)
    {
        postfix += stiva.top();
        stiva.pop();
    }
    return postfix;
}


int main()
{
	tree *temp;
	stack<tree *> s;
	string str,sp("		"),input;
	cout<<"\n Introduceti expresia regulata = ";
	cin >> str;

	str = infixToPostfix(str);
	cout << str << '\n';

///Crearea arborelui
	for(int i = 0 ; i < str.size() ; i++)
	{
		if(!is_op(str[i]))
		{
			gpos++;
			if(str[i] != '#')
			{
				fl_pos++;
				alpha_int[fl_pos] = str[i];
				in_alpha.insert(str[i]);
			}
			temp = create(str[i],gpos);
			temp->nullable = false;
			temp->fpos.pb(gpos);
			temp->lpos.pb(gpos);
		}
		else if(str[i] == '*')
		{
			temp = create(str[i],0);
			temp->left = s.top() , s.pop();
			temp->nullable = true;
			temp->fpos = temp->left->fpos;
			temp->lpos = temp->left->lpos;
			for(int i = 0 ; i < temp->lpos.size() ; i++)
				follow_pos[temp->lpos[i]] = follow_pos[temp->lpos[i]] + temp->fpos;

		}
		else if(str[i] == '.')
		{
			temp = create(str[i],0);
			temp->right = s.top() , s.pop();
			temp->left = s.top() , s.pop();
			temp->nullable = temp->right->nullable && temp->left->nullable;
			if(temp->left->nullable)
				temp->fpos = temp->right->fpos + temp->left->fpos;
			else
				temp->fpos = temp->left->fpos;
			if(temp->right->nullable)
				temp->lpos = temp->right->lpos + temp->left->lpos;
			else
				temp->lpos = temp->right->lpos;
			for(int i = 0 ; i < temp->left->lpos.size() ; i++)
				follow_pos[temp->left->lpos[i]] = follow_pos[temp->left->lpos[i]] + temp->right->fpos;

		}
		else
		{
			temp = create(str[i],0);
			temp->right = s.top() , s.pop();
			temp->left = s.top() , s.pop();
			temp->nullable = temp->right->nullable && temp->left->nullable;
			temp->fpos = temp->right->fpos + temp->left->fpos;
			temp->lpos = temp->right->lpos + temp->left->lpos;
		}
		s.push(temp);
	}
	for(auto temp:in_alpha)
		input.pb(temp);
	cout << "\n\nNOD" << sp << "Pozitie" << "       " << "Nullable" << "        " << "First position" << "		   " << "Last position" <<endl;
	postfix(temp);
	cout<<"\n\nFollow Position"<<endl;
	for(int i = 1 ; i <= fl_pos ; i++)	/// Afisarea Follow Position
	{
		cout << i << sp << alpha_int[i] << sp << "{ ";
		for(int j = 0; j < follow_pos[i].size() ; j++)
		{
			cout<<follow_pos[i][j]<<" ";
		}
		cout<<" }\n";
	}
	dfa(temp,input);
	return 0;
}
