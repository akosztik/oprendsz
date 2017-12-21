using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
namespace mutex
{
    class Program
    {
  // Készítsünk új Mutex-et. A szál nem tartalmazza a Mutex-et.
    private static Mutex mut = new Mutex();
    private const int szálak_száma = 3;

    static void Main()
    {
        // Készítünk néhány szálat.
        for(int i = 0; i < szálak_száma; i++)
        {
            Thread myThread = new Thread(new ThreadStart(UseResource));
            myThread.Name = String.Format("Szál{0}", i + 1);
            myThread.Start();
        }

        // A főprogram befejeződik, de az alkalmazás még fut, mert a szálak még futnak
        // AZ alkalmazás is befejeződik a szálak befejeződése során.
    }

        // Ez az eljárás végzi a szikronizációt, kölcsönös kizárást biztosító
        // lépéseket, tehát egyszerre csak egy folyamat (szál) "éli túl",
        // jut túl a WaitOne hívást. Megjegyezzük, hogy több alakja is van.
    private static void UseResource()
    {
        // Várakozás, amíg nem biztonságos a belépés. Első hívás "mutex_lock-ot csinál
        mut.WaitOne();

        Console.WriteLine("{0} belépett a kölcsönös kizárással védett területre!", 
            Thread.CurrentThread.Name);

        // Itt szerepelhet a szükséges védettséget kívánó utasítássor.
        Console.WriteLine("{0} most komoly munkát végez!",
            Thread.CurrentThread.Name);

        // A komoly munka most nem más mint egy kis várakozás.
        Thread.Sleep(500);

        Console.WriteLine("{0} kilép a védett területről\r\n", 
            Thread.CurrentThread.Name);

        // A Mutex elengedése.
        mut.ReleaseMutex();
    }
}
}
