using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace pék_vásárló
{
   
    public partial class Form1 : Form
    {
        kenyérbolt bolt1;
        bool nyitva = false;
        Thread jani_a_pék;
        Thread zoli_a_vevő;
        delegate void kenyérszám_hívás(string s);
        //
        // A megjegyzésekben olvashatók a klasszikus algoritmus hívások (down, up)
        // Ezek a .NET FrameWorkben más névre hallgatnak...
        //
        public void pék_süt()		/* N értéke a „kenyerespolc” mérete */
        {
            //kenyérbolt b = bolt as kenyérbolt;
            int kenyér;
            while (nyitva)
            {
                kenyér = 1; //pék_süt(); 1= kenyér megsült
                //down(&üres);		/* üres csökken, ha előtte>0, mehet tovább*/
                bolt1.üres.WaitOne();
                //down(&szabad);	/* Piszkálhatjuk-e a pékség polcát? */
                bolt1.szabad.WaitOne();
                //kenyér_polcra(kenyér);  /* Igen, betesszük a kenyeret. */
                bolt1.kenyér_polcra();
                // Az ablakban csak a kenyér számot módosítjuk!!!!
                if (this.kenyerek_száma.InvokeRequired)
                {
                    // Másik szál, kell az Invoke hívás
                    kenyérszám_hívás d = new kenyérszám_hívás(kenyérszám_állítás);
                    this.Invoke
                        (d, new object[] { bolt1.Darab.ToString()});
                }
                else
                {
                    // Azonos szál, nem kell Invoke
                    this.kenyerek_száma.Text = bolt1.Darab.ToString() + " (Nincs Invoke)";
                }

                //kenyerek_száma.Text = bolt1.Darab.ToString();
                //up(&szabad);		/* Elengedjük a pékség polcát. */
                bolt1.szabad.Release();
                //up(&tele);		/* Jelezzük vásárlónak, van kenyér. */
                bolt1.tele.Release();
                Thread.Sleep(600); /* cigarettaszünet */
            }
        }
        public void vásárol()		/* vásárló szemaforja a tele */
        {
            int kenyér;

            while (nyitva)
            {
                //down(&tele);		/*tele csökken, ha előtte>0, mehet tovább*/
                bolt1.tele.WaitOne();
                //down(&szabad);	/*Piszkálhatjuk-e a pékség polcát? */
                bolt1.szabad.WaitOne();
                //kenyér = kenyér_polcról();  /* Igen, levesszük a kenyeret. */
                bolt1.kenyér_polcról_le();
                // Az ablakban csak a kenyér számot módosítjuk!!!!
                if (this.kenyerek_száma.InvokeRequired)
                {
                    // Másik szál, kell az Invoke hívás.
                    kenyérszám_hívás d = new kenyérszám_hívás(kenyérszám_állítás);
                    this.Invoke
                        (d, new object[] { bolt1.Darab.ToString()});
                }
                else
                {
                    //  Azonos szál, nem kell Invoke
                    this.kenyerek_száma.Text = bolt1.Darab.ToString() + " (Nincs Invoke)";
                }

                //kenyerek_száma.Text = bolt1.Darab.ToString();
                //up(&szabad);		/* Elengedjük a pékség polcát. */
                bolt1.szabad.Release();
                //up(&üres);		/* Jelezzük péknek, van hely, lehet sütni. */
                bolt1.üres.Release();
                //kenyér_elfogyasztása(kenyér);
                // itt csinálhatnánk valamit
                kenyér = 0; // megettem
                Thread.Sleep(1300);
            }
        }
        public Form1()
        {
            InitializeComponent();
        }
        // This method is passed in to the SetTextCallBack delegate
        // to set the Text property of textBox1.
        private void kenyérszám_állítás(string text)
        {
            this.kenyerek_száma.Text = text;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (!nyitva)
            {
                bolt1 = new kenyérbolt(10);
                jani_a_pék = new Thread(pék_süt);
                zoli_a_vevő = new Thread(vásárol);
                nyitva = true;
                jani_a_pék.Start();
                //jani_a_pék.
                zoli_a_vevő.Start();
                button1.Text = "Bolt bezár!";
            }
            else
            {
                button1.Text = "Bolt kinyit!";
                nyitva = false;
            }
        }

        private void kenyerek_száma_Click(object sender, EventArgs e)
        {

        }
    } 
    
    public class kenyérbolt
    {
        int max_kenyérszám;
        public int Darab { get; set;}
        public Semaphore szabad; 
        public Semaphore tele; 
        public Semaphore üres;
        public void kenyér_polcra()
        {
            Darab++;
        }
        public void kenyér_polcról_le()
        {
            Darab--;
        }
        public kenyérbolt(int darabszam)
        {
            max_kenyérszám = darabszam;
            Darab = 0;          // nincs kenyér
            szabad=new Semaphore(1,1) ;
            tele=new Semaphore(0,max_kenyérszám);
            üres=new Semaphore(max_kenyérszám,max_kenyérszám);
        }
    }
}
