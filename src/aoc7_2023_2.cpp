#include <algorithm>
#include <fstream>
#include <map>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

int WartoscKarty(char c)
{
    switch (c)
    {
    case '2':
        return 2;
    case '3':
        return 3;
    case '4':
        return 4;
    case '5':
        return 5;
    case '6':
        return 6;
    case '7':
        return 7;
    case '8':
        return 8;
    case '9':
        return 9;
    case 'T':
        return 10;
    case 'J':
        return 1;
    case 'Q':
        return 12;
    case 'K':
        return 13;
    case 'A':
        return 14;
    default:
        return 0;
    }
}

class Reka
{
public:
    std::string karty;
    long long   stawka;
    int         silaUkladu;

    Reka(std::string k, long long s)
    {
        karty      = k;
        stawka     = s;
        silaUkladu = ObliczTypUkladu();
    }

    //funkcja ta oblicza ile jest powtorzen danej karty w zbiorze i identyfikuje zbior
    int ObliczTypUkladu()
    {

        //zliczanie ilosci i wartosci konkretnej karty w zbiorze
        std::map< char, int > licznik;
        for (char c : karty)
        {
            licznik[c] = licznik[c] + 1;
        }

        //czesc 2 - obsluga jokerow

        //zapisanie ilosci jokerow i usuniecie ich z mapy
        int liczbaJokerow = licznik['J'];
        licznik.erase('J');

        //wektor na ilosci wystapien poszczegolnych kart (bez jokerow)
        std::vector< int > ilosci;
        for (auto const& [karta, ile] : licznik)
        {
            ilosci.push_back(ile);
        }

        //sortowanie wektora ilosci wystapienia kart
        std::ranges::sort(ilosci, std::greater< int >());

        //dodanie jokerow do zbioru
        if (ilosci.empty())
        {
            //przypadek specjalny - same jokery w zbiorze
            ilosci.push_back(5);
        }
        else
        {
            //dodanie jokerow do najbardziej licznej karty
            ilosci[0] += liczbaJokerow;
        }

        //poniewaz posortowano po ilosci wystapien danej kart, najbardziej znaczace miejsce w wektorze ma index 0
        if (ilosci[0] == 5)
            return 7; //piatka
        if (ilosci[0] == 4)
            return 6; //kareta
        if (ilosci[0] == 3 && ilosci[1] == 2)
            return 5; //full house
        if (ilosci[0] == 3)
            return 4; //trojka
        if (ilosci[0] == 2 && ilosci[1] == 2)
            return 3; //dwie pary
        if (ilosci[0] == 2)
            return 2; //para
        return 1; // wysoka karta
    }
};

int main()
{
    try
    {
        std::ifstream plik("dane7_2023.txt");
        if (!plik.is_open())
            throw std::runtime_error("Brak pliku");

        //utworzenie wektora stringow na surowe linijki z pliku
        std::vector< std::string > suroweLinie;

        //definicja linijki danych
        std::string linia;

        //std::getline dziala dla pojedynczej linii. konczy swoje dzialanie w momencie znalezienia znaku /n
        //petla while przechodzi przez caly plik, a kazda linijka skanowana jest przez getline
        while (std::getline(plik, linia))
        {
            if (!linia.empty())
            {
                suroweLinie.push_back(linia);
            }
        }

        //dane w postaci 32T3K 765
        //ponizsza instrukcja ma za zadanie pobrac dana oraz rozbic ja na poszczegolne elementy
        auto widokRak = suroweLinie | std::views::transform([](const std::string& l) {
                            std::stringstream ss(l);
                            std::string       k;
                            long long         s;

                            //32T3K oznaczone jest jako k. przerwa pomiedzy zbiorem kart a ich stawka jest sygnalem
                            //zeby przejsc do zmiennej s i zapisac w niej 765
                            ss >> k >> s;
                            return Reka(k, s);
                        });

        //tworzenie wektora danych z widoku
        std::vector< Reka > stol(widokRak.begin(), widokRak.end());

        std::ranges::sort(stol, [](const Reka& a, const Reka& b) {
            //porownanie mocy ukladu kart
            if (a.silaUkladu != b.silaUkladu)
            {
                return a.silaUkladu < b.silaUkladu;
            }

            //gdy sily ukladow sa identyczne sprawdza sie ktora karta po kolei jest wyzsza
            for (size_t i = 0; i < a.karty.size(); ++i)
            {
                int mocA = WartoscKarty(a.karty[i]);
                int mocB = WartoscKarty(b.karty[i]);

                if (mocA != mocB)
                {
                    return mocA < mocB;
                }
            }
            return false; //w przypadku gdy trafi sie na identyczny zbior kart
        });

        long long wynik = 0;
        for (size_t i = 0; i < stol.size(); ++i)
        {
            long long rank = i + 1;
            wynik          = wynik + stol[i].stawka * rank;
        }

        std::println("Calkowita wygrana: {}", wynik);
    }
    catch (const std::exception& e)
    {
        std::println(stderr, "Blad: {}", e.what());
        return 1;
    }
    return 0;
}