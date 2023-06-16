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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace OfflineConverter
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void UpdateFormat(object sender, SelectionChangedEventArgs e)
        {

        }

        private void UpdateFrom(object sender, SelectionChangedEventArgs e)
        {

        }

        private void UpdateTo(object sender, SelectionChangedEventArgs e)
        {

        }

        private void ConvertButton_Click(object sender, RoutedEventArgs e)
        {

        }

        private void InputBrowse_Click(object sender, RoutedEventArgs e)
        {
            //SaveFileDialog saveFileDialog = new SaveFileDialog();
            //if (saveFileDialog.ShowDialog() == true)
            //{
            //    // Clear directory
            //    spriteSheet.OutputDirectory = String.Empty;

            //    // Split the file to help seperate directory and file
            //    string[] file = saveFileDialog.FileName.Split('\\');
            //    for (int i = 0; i < file.Length - 1; ++i)
            //    {
            //        spriteSheet.OutputDirectory += file[i] + '\\';
            //    }

            //    // Split the last part of the previous split to check for extension
            //    string[] path = file[file.Length - 1].Split('.');
            //    if (path.Length == 1)
            //    {
            //        // Add extension if one isnt provided
            //        spriteSheet.OutputFile = file[file.Length - 1] + ".png";
            //    }
            //    else
            //    {
            //        // guarantees we save as a png and removes all other cases using '.'
            //        spriteSheet.OutputFile = path[0] + ".png";
            //    }

            //    tbOutputDir.Text = spriteSheet.OutputDirectory;
            //    tbOutputFile.Text = spriteSheet.OutputFile;
            //}
        }

        private void OutputBrowse_Click(object sender, RoutedEventArgs e)
        {

        }

    }
}
