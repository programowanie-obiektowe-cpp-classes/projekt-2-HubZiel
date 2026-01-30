#include <algorithm>
#include <fstream>
#include <iostream>
#include <print>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

//klasa reprezentujaca pojedynczy przedzial
class Przedzial
{
private:
    long long start;
    long long koniec;

public:
    Przedzial(long long s, long long k)
    {
        start  = s;
        koniec = k;
    }

    bool Zawiera(long long liczba) const
    {
        if (liczba >= start && liczba <= koniec)
        {
            return true;
        }
        return false;
    }

    long long getStart() const { 
        return start; 
    }
    long long getKoniec() const { 
        return koniec; 
    }
    void setKoniec(long long k) { 
        koniec = k; 
    }

    long long Dlugosc() const { 
        return (koniec - start + 1); 
    }
};

int main()
{

    int licznik = 0;
    long long suma = 0;

    try
    {
        //czesc 1 - wczytanie danych i sprawdzenie swiezosci w poszczegolnych zakresach
        std::ifstream plik("dane5_2025.txt");
        if (!plik.is_open())
        {
            throw std::runtime_error("Nie mozna otworzyc pliku");
        }

        //dane w formacie
        //94797035470525-96066978979415
        //179130084251318

        //utworzenie wektora stringow na surowe dane z pliku
        std::vector< std::string > surowe;

        //utworzenie zmiennej do przechowywania pojedynczego slowa z pliku
        std::string slowo;

        //wczytanie danych z pliku do wektora surowe slowo po slowie (oddzielone spacjami lub nowa linia)
        while (plik >> slowo)
        {
            surowe.push_back(slowo);
        }

        //algorytm patrzy na kazdy element wektora surowe i sprawdza czy zawiera znak '-'
        auto widokZakresow = surowe |
                             std::views::filter([](const std::string& s) { return s.find('-') != std::string::npos; })

                           //po wybraniu tylko tych elementow, ktore sa przedzialami (zawieraja znak '-')
                           //dzieli je na faktyczny przedzial (rozdziela poczatek i koniec)

                           | std::views::transform([](const std::string& s) {
                                 auto myslnik = s.find('-');
                                 //.find() zwraca index, na ktorym stoi myslnik
                                 //podzial zakresu na 2x substring w danych zakresow indeksów

                                 long long poczatek = std::stoll(s.substr(0, myslnik));
                                 long long koniec   = std::stoll(s.substr(myslnik + 1));
                                 return Przedzial(poczatek, koniec);
                             });

        //zapisywanie widoku do wektora przedzialow
        std::vector< Przedzial > zakresy(widokZakresow.begin(), widokZakresow.end());

        //algorytm powtarza sie dla ID (liczb bez myslnika)
        auto widokID = surowe

            //warunek - s.find('-') == npos (czyli brak myslnika w napisie) oraz sprawdzenie czy pierwszy znak to cyfra
            | std::views::filter([](const std::string& s) { return s.find('-') == std::string::npos && isdigit(s[0]); })

            //zamiana stringow na liczbe
            | std::views::transform([](const std::string& s) { return std::stoll(s); });

        //zapisywanie widoku do wektora ID
        std::vector< long long > ID(widokID.begin(), widokID.end());

        //petla sprawdzajaca kazde ID czy nalezy do jakiegos przedzialu
        for (long long id : ID)
        {

            //intrukcja std::ranges::find_if szuka pierwszego przedzialu, ktory zawiera dany ID
            //zmienna it wskazuje na znaleziony przedzial
            auto it = std::ranges::find_if(zakresy, [&](const Przedzial& z) { return z.Zawiera(id); });

            // jesli zmienna it wskazuje na cos innego niz koniec zakresy, to znaczy, ze znaleziono przedzial
            // zawierajacy ID
            if (it != zakresy.end())
            {
                licznik = licznik + 1;
                std::println("liczba {} - SWIEZE (zakres {}-{})", id, it->getStart(), it->getKoniec());
            }
            else
            {
                std::println("liczba {} - ZEPSUTE", id);
            }
        }
        std::println("Laczna liczba SWIEZYCH: {}", licznik);

        //czesc 2 - scalanie przedzialow i sumowanie dlugosci
        if (zakresy.empty())
            return 0;

        //sortowanie przedzialow rosnaco po ich poczatkach w zmiennej "zakresy"
        std::ranges::sort(zakresy, std::ranges::less{}, &Przedzial::getStart);

        //scalanie przedzialow
        //nowy wektor oznaczajacy scalone przedzialy
        std::vector< Przedzial > scalone;

        //zekresy[0] jako pierwszy element pustego wektora "scalone"
        scalone.push_back(zakresy[0]);

        //petla po zakresach. i = 1, poniewaz pierwsza para jest juz w wektorze
        for (size_t i = 1; i < zakresy.size(); ++i)
        {

            //pobiera ostatni scalony zakres oraz aktualny zakres ktory jest wskazywany przez i
            //dla pierwszej iteracji pobiera zakresy[0] oraz zakresy[1]
            auto& obecny  = zakresy[i];
            auto& ostatni = scalone.back();

            //sprawdzenie czy przedzialy na siebie nachodza
            //dodatkowo trzeba sprawdzic dodatkowy aspekt. zakresy 5-10 (ostatni) i 11-20 (obecny) to jeden ciag,
            //pomimo ze na siebie nie nachodza
            if (obecny.getStart() <= ostatni.getKoniec() + 1)
            {

                // dodatkowy warunek. sprawdzenie czy obecny zakres nie zawiera sie w ostatnim
                if (obecny.getKoniec() > ostatni.getKoniec())
                {
                    ostatni.setKoniec(obecny.getKoniec());
                }
            }
            else
            {
                //jesli nie nachodz¹, obecny dodany jako nowy, osobny zakres
                scalone.push_back(obecny);
                //w tym przypadku kolejna iteracja porownuje juz do tego nowego, osobnego zakresu
            }
        }

        //po scaleniu przedzialow, obliczenie dlugosci pojedynczego scalonego przedzialu
        auto dlugosci = scalone | std::views::transform([](const Przedzial& z) { return z.Dlugosc(); });

        //sumowanie dlugosci scalonych przedzialow
        for (long long d : dlugosci)
        {
            suma = suma + d;
        }

        std::println("czesc 2 - suma: {}", suma);
    }
    catch (const std::exception& e)
    {
        //wyjatek w momencie, gdy cos nie wyjdzie
        std::println(stderr, "BLAD: {}", e.what());
        return 1;
    }

    return 0;
}