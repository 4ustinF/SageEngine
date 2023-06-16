using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace SpriteSheetEditor
{
    /// <summary>
    /// Interaction logic for Animation.xaml
    /// </summary>
    public partial class Animation : Window
    {
        public List<Image> spritesList = new List<Image>();
        private int index = 0;

        public Animation()
        {
            InitializeComponent();

            DispatcherTimer timer = new DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(0.01f);
            timer.Tick += NextFrame;
            timer.Start();
        }


        private void NextFrame(object sender, EventArgs e)
        {
            AnimImage.Source = spritesList[index].Source;
            index++;
            if (index >= spritesList.Count)
            {
                index = 0;
            }
        }
    }
}
