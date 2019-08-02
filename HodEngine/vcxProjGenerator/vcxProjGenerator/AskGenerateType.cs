using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace vcxProjGenerator
{
    public class WindowWrapper : System.Windows.Forms.IWin32Window
    {
        public WindowWrapper(IntPtr handle)
        {
            _hwnd = handle;
        }

        public IntPtr Handle
        {
            get { return _hwnd; }
        }

        private IntPtr _hwnd;
    }

    public partial class AskGenerateType : Form
    {
        [DllImport("user32.dll")]
        static extern IntPtr GetForegroundWindow();

        public AskGenerateType()
        {
            InitializeComponent();
        }

        public void AddGenerateChoice(string _choice)
        {
            this.comboBox1.Items.Add(_choice);
        }

        public int GetChoice()
        {
            IntPtr locHandle = GetForegroundWindow();

            this.comboBox1.SelectedIndex = 0;

            this.StartPosition = FormStartPosition.CenterParent;

            ShowDialog(new WindowWrapper(locHandle));

            return this.comboBox1.SelectedIndex;
        }


        private void button1_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
