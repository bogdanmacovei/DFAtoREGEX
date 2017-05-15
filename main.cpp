#include <bits/stdc++.h>
#include <cstdio>
#include <cstring>

using namespace std;

/**
                                MATRICE SIMBOLURI

        Nume               |      Variabila    |   Tip
        =========================================================================
        nr stari           |      nrStari      |   int
        -------------------------------------------------------------------------
        starile            |      Q            |   vector <int>
        -------------------------------------------------------------------------
        litere             |      Sigma        |   vector <string>
        -------------------------------------------------------------------------
        stare intiala      |      q0           |   int
        -------------------------------------------------------------------------
        nr starilor finale |      nrFinale     |   int
        -------------------------------------------------------------------------
        starile finale     |      F            |   vector <int>
        -------------------------------------------------------------------------
        tranzitii          |      delta        |   map <pair <int, string>, int>
        -------------------------------------------------------------------------
        lista cuvinte      |      cuvant       |   string

**/

bool nouInit = false, nouFinal = false;

void citireDate (vector<int> &Q, vector<string> &Sigma, vector<int> &F, int &q0, map <pair <int, string>, int> &delta)
{
    ifstream fin ("fa.in");

    /// Citirea numarului de stari

    int nrStari;
    fin >> nrStari;

    /// Citirea starilor

    for(int i = 0; i < nrStari; ++i)
    {
        int q;
        fin >> q;
        Q.push_back(q);
    }

    /// Citirea alfabetului Sigma

    string litera;

    do
    {
        fin >> litera;

        if (litera != ";" && litera != " ")
        {
            Sigma.push_back(litera);
        }
    }
    while(litera != ";");

    /// Citirea starii initiale

    fin >> q0;

    /// Citirea numarului de stari finale

    int nrFinale;
    fin >> nrFinale;

    /// Citirea starilor finale

    for(int i = 0; i < nrFinale; ++i)
    {
        int f;
        fin >> f;
        F.push_back(f);
    }

    /// Citirea tranzitiilor

    int stare1, stare2;
    litera.clear();

    do
    {
        fin >> stare1 >> litera >> stare2;
        if (stare1 == -1)
            break;
        delta[{stare1, litera}] = stare2;
        litera.clear();
    }
    while(stare1 != -1);

    fin.close();
}

bool inStare (int x, vector<int> Q)
{
    for (int st : Q)
        if (x == st)
            return true;
    return false;
}

void initFinal (int &q0, vector<string> &Sigma, vector<int> &Q, map<pair<int, string>, int> &delta, vector<int> &F)
{
    bool ok = true;

    for (map<pair<int, string>, int>::iterator it = delta.begin(); it != delta.end(); it++)
        if(it->second == q0)
        {
            ok = false;
            goto et;
        }
    et:
    if (!ok)
    {
        nouInit = true;
        int nouSt = 0;
        delta[{nouSt, "."}] = q0;
        q0 = nouSt;
        Q.push_back(0);

    } /// stiu sigur ca a intrat ceva in starea 1, si am creat starea initiala 0

    /// daca iese ceva din starea finala, e la fel de nasol, deci adaug o noua stare finala cu o lamnbda-tranzitie

    int cardF = 0;
    for (int i : F)
        cardF ++;

    if (cardF > 1)
    {
        ok = false;
        goto et1;
    }

    ok = true;
    for (int finala : F) // pentru fiecare stare finala
        for (string lit : Sigma) // pentru fiecare litera din alfabet
            // daca pot sa ies cu litera asta din starea finala, e nasol, dau fals
            if (inStare (delta[{finala, lit}], Q) && delta[{finala, lit}] != 0)
            {
//                cout << lit << " ";
//                cout << delta[{finala, lit}] << " ";
                ok = false;
                goto et1;
            }

    et1:
    if (!ok)
    {
        nouFinal = true;
        // daca o stare finala a fost naspa, creez alta
        int last;
        for (int i : Q)
            if (i != 0)
                last = i;
        int nouSt = last + 1;
        for (int finala : F)
            delta[{finala, "."}] = nouSt;
//        cout << "nouSt: " << nouSt << "\n\n";
        F.clear();
        F.push_back(nouSt);
    }

    if (nouInit || nouFinal)
        Sigma.push_back(".");
}

string drum (int nod1, int nod2, map<pair<int, string>, int> delta)
{
    for (map<pair<int, string>, int>::iterator it = delta.begin(); it != delta.end(); it++)
        if(it->first.first == nod1 && it->second == nod2)
            return it->first.second;
    return "";
}

void sterge(int nod, vector<string> &Sigma, map<pair<int, string>, int> &delta, vector<int> Q)
{
    // acum am o stare initiala si o stare finala, eventual cu lambda-tranzitii
    // cand sterg un nod, ma uit cine intra si unde trebuia sa se ajunga
    // initial, vreau sa formez doi vectori, intrare si iesire, pentru nodul meu

    vector<int> iesire;
    vector<int> intrare;

    for(map<pair<int,string>,int>::iterator it = delta.begin(); it != delta.end(); it++)
    {
        if(it->first.first == nod && it->second != nod)
            iesire.push_back(it->second);

        if(it->first.first != nod && it->second == nod)
            intrare.push_back(it->first.first);
    }

    for (int i : intrare)
        for (int j : iesire)
            if (drum(i, j, delta) != "")
            {
                string nouDrum = drum (i, j, delta);
                nouDrum = nouDrum + " + (" + drum(i, nod, delta) + ") (.+" + drum(nod, nod, delta) + ")* (" + drum(nod, j, delta) + ")";

                for (map<pair<int,string>,int>::iterator it = delta.begin(); it != delta.end();)
                    if (it->first.first == i && it->second == j)
                    {
                        delta.erase(it++);
                    }
                    else it++;

                delta[{i, nouDrum}] = j;

                Sigma.push_back(nouDrum);
            }


            else
            {
                string nouDrum = "(" + drum(i, nod, delta) + ") (.+" + drum(nod, nod, delta) + ")* (" + drum(nod, j, delta) + ")";
                delta[{i, nouDrum}] = j;
                Sigma.push_back(nouDrum);
            }

    for (map<pair<int,string>,int>::iterator it = delta.begin(); it != delta.end();)
        if (it->first.first == nod || it->second == nod)
        {
            delta.erase(it++);
        }
        else it++;
}

void etichetare (int nod1, int nod2, map<pair<int,string>, int> &delta)
{
    int nr = 1;
    string etich = "";
    for(map<pair<int,string>,int>::iterator it = delta.begin(); it != delta.end(); it++)
        if(it->first.first == nod1 && it->second == nod2)
        {
            if (nr == 1)
            {
                etich = it->first.second;
                nr++;
            }
            else
            {
                etich = etich + "+" + it->first.second;
                nr++;
            }
        }

    if (nr != 2)
    {
        for (map<pair<int,string>,int>::iterator it = delta.begin(); it != delta.end();)
            if (it->first.first == nod1 && it->second == nod2)
            {
                delta.erase(it++);
            }
            else it++;

        delta[{nod1, etich}] = nod2;
        delta[{nod1, etich}] = nod2;
    }

}

int main()
{
    /// Datele pentru DFA->REGEX

    vector <int> Q;                                             // multimea starilor initiale
    vector <string> Sigma;                                      // multimea neterminalelor
    vector <int> F;                                             // multmea starilor finale
    int  q0;                                                    // starea intiala
    map <pair <int, string>, int> delta;                        // functia de tranzitie

    citireDate(Q, Sigma, F, q0, delta);

    initFinal(q0, Sigma, Q, delta, F);

    int cardF = 0;
    for (int i : F)
        if(cardF <= i)
            cardF = i;

    for(map<pair<int,string>,int>::iterator it = delta.begin(); it != delta.end(); it++)
        if(it->first.first != it->second)
            etichetare(it->first.first, it->second, delta);

    for (int nod : Q)
    {
        string eticheta = "";
        int nr = 1;
        for(map<pair<int,string>,int>::iterator it = delta.begin(); it != delta.end(); it++)
            if(it->first.first == nod && it->second == nod)
            {
                if(nr == 1)
                {
                    eticheta = it->first.second;
                    nr ++;
                }
                else
                {
                    eticheta = eticheta + "+" + it->first.second;
                    nr ++;
                }
            }

        if (nr >= 2)
        {
            for (map<pair<int,string>,int>::iterator it = delta.begin(); it != delta.end();)
                if (it->first.first == nod && it->second == nod)
                {
                    delta.erase(it++);
                }
                else it++;

            delta[{nod, eticheta}] = nod;
        }
    }

    int inc, fin;
    if (nouInit && nouFinal)
    {
        inc = 1, fin = cardF;
    }

    if (!nouInit && nouFinal)
    {
        inc = 2, fin = cardF;
    }

    if (nouInit && !nouFinal)
    {
        inc = 1, fin = cardF;
    }

    if (!nouInit && !nouFinal)
    {
        inc = 2, fin = cardF;
    }

    for (int i = inc; i < fin; ++i)
        sterge(i, Sigma, delta, Q);

    string result;
    for(map<pair<int,string>,int>::iterator it = delta.begin(); it != delta.end(); it++)
    {
        cout << it->first.second;
        result = it->first.second;
        break;
    }

    ofstream fcout("fa.out");
    fcout << "REGEX for given DFA:\n";

    for (int i = 0; i < result.length(); ++i)
    {
        if (result[i] == '(' && result[i+1] == '.' && result [i+2] == ')' && result[i+3] != '+')
                i+=3;

        else if (result[i] == '(' && result[i+2] == ')' && result[i+3] != '*')
        {
            fcout << result[i+1];
            i+=2;
        }

        else if (result[i] == '(' && result[i+1] == ')' && result[i+2] == '*')
            i+=3;

        else if (result[i]=='(' && result[i+1] == '.' && result[i+2] == '+' && result[i+3] == ')' && result[i+4]=='*')
            i+=4;

        else fcout << result[i];
    }

    fcout << ")";

    fcout.close();


    return 0;
}
