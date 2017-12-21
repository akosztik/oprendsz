using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

// Példa a szemafor használatára.

public class szemafor
{
    // Kritikus erőforrás: váró (terem).
    //
    private static Semaphore váró;

    // a kiírás késleltetése
    private static int _padding;

    public static void Main()
    {
        // Párhuzamosan maximum 4 szál érheti el, 
        // kezdőérték 2, azt jelenti, hogy 2 várakozó beléphet a szemaforba 
        // ha a kezdőérték 0, azt jelenti, hogy nem léphet be senki, a waitone megáll
        // a szemafor előtt
        váró = new Semaphore(2, 4);

        // Szálak készítése 
        //
        for(int i = 1; i <= 6; i++)
        {
            //Thread t = new Thread(new ParameterizedThreadStart(Worker));
            // lehetne ThreadStart példánnyal is kezdeni, de ennek nincs paramétere!
            // azaz a worker-nek a threadstart-al nem lehetne paramétere
            // Start the thread, passing the number.
            //
            // vagy így egyszerűbb
            Thread t = new Thread(feladat);
            t.Start(i);
        }

        // 500 ms várakozás
        //
        Thread.Sleep(500);

        //
        Console.WriteLine("A főprogram Release(2)t hív. (2 folyamat továbbmehet)");
        // nem szerencsés a max-nál több folyamatot átengedni,hogy elérjük a max értéket
        // mert SemaphorFullException kivételt kapunk
        int p=váró.Release(2); // kettővel növeli a szemafort.
        // a visszaadott értékben a szemafor korábbi értéke van
        Console.WriteLine("A Release(2) visszaadott értéke: {0}", p);

        Console.WriteLine("Főprogram vége.");
    }

    private static void feladat(object num)
    {
        // Minden szál ránéz a szemaforra, beléphet-e vagy se?
        // Ha a szemafor értéke >0, be lehet lépni, és csökkenti a szemafor értéket
        Console.WriteLine("{0}. szál elindult " +
            "és vár a szemafor előtt. (ha kell)", num);
        váró.WaitOne();

        //Növekvő várakozás.
        int padding = Interlocked.Add(ref _padding, 100);

        Console.WriteLine("{0}. szál belép a szemaforral védett szakaszba.", num);

        Thread.Sleep(1000 + padding);

        Console.WriteLine("{0}. szál elhagyja a szemafor által védett szakaszt.", num);
        // a szemafor értéke azt mondja meg, hogy hány folyamat léphet be, hány folyamatnak
        // mutat szabadot a szemafor
        Console.WriteLine("{0}. szál kilépése előtt a szemafor értéke: {1}",
            num, váró.Release());
    }
}