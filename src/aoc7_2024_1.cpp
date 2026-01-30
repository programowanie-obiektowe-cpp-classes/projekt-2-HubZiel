#include <algorithm>
#include <fstream>
#include <numeric>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

class Rownanie
{
private:
    long long                cel;
    std::vector< long long > liczby;

    bool SprawdzRekurencyjnie(size_t index, long long biezacyWynik) const
    {
        if (index == liczby.size())
        {
            return biezacyWynik == cel;
        }
        if (biezacyWynik > cel)
        {
            return false;
        }

        long long nastepna = liczby[index];

        // sprawdzenie sciezki dodawania
        if (SprawdzRekurencyjnie(index + 1, biezacyWynik + nastepna))
        {
            return true;
        }

        // sprawdzenie sciezki mnozenia
        if (SprawdzRekurencyjnie(index + 1, biezacyWynik * nastepna))
        {
            return true;
        }

        // jesli zadna sciezka nie dala wyniku
        return false;
    }

public:
    // konstuktor
    Rownanie(long long c, std::vector< long long > l)
    {
        cel    = c;
        liczby = l;
    }

    long long getCel() const { return cel; }

    // metoda uruchamia petle rekurencyjna
    bool CzyDaSieRozwiazac() const
    {
        if (liczby.empty())
        {
            return false;
        }

        // start petli rekurencyjnej. metoda ta jest funkcj¹ bool, wiec wynik bedzie 0 albo 1.
        // argumenty - 1 jako index kolejnej liczby do sprawdzenia, liczby[0] jako liczba pierwsza z danych danego
        // wyniku
        return SprawdzRekurencyjnie(1, liczby[0]);
    }
};

int main()
{
    try
    {
        std::ifstream plik("dane7_2024.txt");
        if (!plik.is_open())
        {
            throw std::runtime_error("Brak pliku");
        }

        // utworzenie wektora stringow na surowe linijki z pliku
        std::vector< std::string > suroweLinie;

        // definicja linijki danych
        std::string linia;

        // std::getline dziala dla pojedynczej linii. konczy swoje dzialanie w momencie znalezienia znaku /n
        // petla while przechodzi przez caly plik, a kazda linijka skanowana jest przez getline
        while (std::getline(plik, linia))
        {
            if (!linia.empty())
            {
                suroweLinie.push_back(linia);
            }
        }

        // algorytm patrzy na kazdy element wektora suroweLinie i sprawdza na ktorej pozycji znajduje sie znak ':'
        auto widokRownan = suroweLinie | std::views::transform([](const std::string& l) {
                               //.find() zwraca index, na ktorym stoi dwukropek
                               auto dwukropek = l.find(':');

                               // wyci¹ganie liczby celu z pocz¹tku linii (indeksy od 0 do dwukropka)
                               long long c = std::stoll(l.substr(0, dwukropek));

                               // nowa zmienna ss, ktora jest reszta linii i w formie stringstream
                               std::stringstream ss(l.substr(dwukropek + 1));

                               // stworzenie wektora dla liczb tworzacych cel
                               std::vector< long long > liczby;

                               // uzycie stringstream i ss>>temp powoduje ze zwracane zostaja dane do wektora liczby
                               // instrukcja ignoruje spacje i bierze tylko liczby jako osobne znaki
                               long long t;
                               while (ss >> t)
                                   liczby.push_back(t);

                               return Rownanie(c, liczby);
                           });

        // tworzenie wektora wszystkich danych z widoku (celu oraz liczb ktore tworza cel)
        std::vector< Rownanie > wszystkieRownania(widokRownan.begin(), widokRownan.end());

        // wybieranie tylko poprawnych rownan, ktore daja sie rozwiazac
        auto poprawneRownania =
            wszystkieRownania | std::views::filter([](const Rownanie& r) { return r.CzyDaSieRozwiazac(); });

        long long suma    = 0;
        int       licznik = 0;

        // iteracja przez poprawne rownania i wypisanie wyniku
        for (const auto& r : poprawneRownania)
        {
            std::println("ok: {}", r.getCel());
            suma    = suma + r.getCel();
            licznik = licznik + 1;
        }

        std::println("Liczba poprawnych: {}", licznik);
        std::println("Total calibration result: {}", suma);
    }
    catch (const std::exception& e)
    {
        std::println(stderr, "Blad: {}", e.what());
        return 1;
    }
    return 0;
}