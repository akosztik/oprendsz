namespace pék_vásárló
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.pékcimke = new System.Windows.Forms.Label();
            this.kenyérbolt = new System.Windows.Forms.Label();
            this.vásárlócimke = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.pékállapot = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.kenyerek_száma = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // pékcimke
            // 
            this.pékcimke.AutoSize = true;
            this.pékcimke.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.pékcimke.Location = new System.Drawing.Point(33, 54);
            this.pékcimke.Name = "pékcimke";
            this.pékcimke.Size = new System.Drawing.Size(45, 24);
            this.pékcimke.TabIndex = 0;
            this.pékcimke.Text = "Pék";
            // 
            // kenyérbolt
            // 
            this.kenyérbolt.AutoSize = true;
            this.kenyérbolt.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.kenyérbolt.Location = new System.Drawing.Point(122, 54);
            this.kenyérbolt.Name = "kenyérbolt";
            this.kenyérbolt.Size = new System.Drawing.Size(278, 24);
            this.kenyérbolt.TabIndex = 1;
            this.kenyérbolt.Text = "Kenyérbolt, kenyerek száma:";
            // 
            // vásárlócimke
            // 
            this.vásárlócimke.AutoSize = true;
            this.vásárlócimke.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.vásárlócimke.Location = new System.Drawing.Point(428, 54);
            this.vásárlócimke.Name = "vásárlócimke";
            this.vásárlócimke.Size = new System.Drawing.Size(80, 24);
            this.vásárlócimke.TabIndex = 2;
            this.vásárlócimke.Text = "Vásárló";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(205, 204);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 3;
            this.button1.Text = "Start";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // pékállapot
            // 
            this.pékállapot.AutoSize = true;
            this.pékállapot.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.pékállapot.Location = new System.Drawing.Point(33, 133);
            this.pékállapot.Name = "pékállapot";
            this.pékállapot.Size = new System.Drawing.Size(78, 24);
            this.pékállapot.TabIndex = 4;
            this.pékállapot.Text = "Alszom";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label1.Location = new System.Drawing.Point(428, 133);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(108, 24);
            this.label1.TabIndex = 5;
            this.label1.Text = "Vásárolok.";
            // 
            // kenyerek_száma
            // 
            this.kenyerek_száma.AutoSize = true;
            this.kenyerek_száma.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.kenyerek_száma.Location = new System.Drawing.Point(231, 133);
            this.kenyerek_száma.Name = "kenyerek_száma";
            this.kenyerek_száma.Size = new System.Drawing.Size(21, 24);
            this.kenyerek_száma.TabIndex = 6;
            this.kenyerek_száma.Text = "0";
            this.kenyerek_száma.Click += new System.EventHandler(this.kenyerek_száma_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(531, 261);
            this.Controls.Add(this.kenyerek_száma);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.pékállapot);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.vásárlócimke);
            this.Controls.Add(this.kenyérbolt);
            this.Controls.Add(this.pékcimke);
            this.Name = "Form1";
            this.Text = "Pék - Vásárló probléma";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label pékcimke;
        private System.Windows.Forms.Label kenyérbolt;
        private System.Windows.Forms.Label vásárlócimke;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label pékállapot;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label kenyerek_száma;
    }
}

