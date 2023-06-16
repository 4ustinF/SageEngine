using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.IO;
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
using System.Xml;
using System.Xml.Serialization;
using static System.Net.WebRequestMethods;

namespace SpriteSheetEditor
{
    /// <summary>
    /// Interaction logic for CreateWindow.xaml
    /// </summary>
    public partial class CreateWindow : Window
    {
        private SpriteSheet spriteSheet = new SpriteSheet();
        private List<Image> spritesList = new List<Image>();

        private void UpdateImages()
        {
            spritesList.Clear();
            foreach (String file in spriteSheet.InputPaths)
            {
                Image image = new Image();
                image.Width = 100;
                image.Height = 100;

                if (file != spriteSheet.BlankString)
                {
                    image.Source = new BitmapImage(new Uri(file));
                }

                spritesList.Add(image);
            }

            lbImages.ItemsSource = null;
            lbImages.ItemsSource = spritesList;
        }

        public CreateWindow()
        {
            InitializeComponent();

            spriteSheet.InputPaths = new List<string>();
            spriteSheet.Columns = 1;
        }

        private void New_Click(object sender, RoutedEventArgs e)
        {
            CreateWindow w = new CreateWindow();
            w.Show();
            this.Close();
        }

        private void Exit_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void MiAnimation_Click(object sender, RoutedEventArgs e)
        {
            if(spritesList.Count == 0) { return; }

            Animation a = new Animation();
            a.spritesList = spritesList;
            a.Show();
        }

        private void tbColumns_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (int.TryParse(tbColumns.Text, out int columns))
            {
                if (columns <= 0) 
                { 
                    columns = 1;
                }

                spriteSheet.Columns = columns;
                tbColumns.Text = spriteSheet.Columns.ToString();
                tbColumns.Focus();
                tbColumns.SelectionStart = tbColumns.Text.Length;
                return;
            }

            spriteSheet.Columns = 1;
            tbColumns.Text = String.Empty;
        }

        private void Browse_Click(object sender, RoutedEventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            if (saveFileDialog.ShowDialog() == true)
            {
                // Clear directory
                spriteSheet.OutputDirectory = String.Empty;

                // Split the file to help seperate directory and file
                string[] file = saveFileDialog.FileName.Split('\\');
                for (int i = 0; i < file.Length - 1; ++i)
                {
                    spriteSheet.OutputDirectory += file[i] + '\\';
                }

                // Split the last part of the previous split to check for extension
                string[] path = file[file.Length - 1].Split('.');
                if (path.Length == 1)
                {
                    // Add extension if one isnt provided
                    spriteSheet.OutputFile = file[file.Length - 1] + ".png";
                }
                else
                {
                    // guarantees we save as a png and removes all other cases using '.'
                    spriteSheet.OutputFile = path[0] + ".png";
                }

                tbOutputDir.Text = spriteSheet.OutputDirectory;
                tbOutputFile.Text = spriteSheet.OutputFile;
            }
        }

        private void Add_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Multiselect = true;

            if (openFileDialog.ShowDialog() == true)
            {
                foreach (String file in openFileDialog.FileNames)
                {
                    spriteSheet.InputPaths.Add(file);
                }
                UpdateImages();
            }
        }

        private void AddBlank_Click(object sender, RoutedEventArgs e)
        {
            spriteSheet.InputPaths.Add(spriteSheet.BlankString);
            spriteSheet.BlankCount++;
            UpdateImages();
        }

        private void Remove_Click(object sender, RoutedEventArgs e)
        {
            if (lbImages.SelectedItem == null)
            {
                MessageBox.Show("Select a image to remove. If there are no images to select try adding one first.", "Warning");
                return;
            }

            if (spriteSheet.InputPaths[lbImages.SelectedIndex] == spriteSheet.BlankString)
            {
                spriteSheet.BlankCount--;
            }

            spriteSheet.InputPaths.RemoveAt(lbImages.SelectedIndex);
            UpdateImages();
        }

        private void Clear_Click(object sender, RoutedEventArgs e)
        {
            if (spriteSheet.InputPaths.Count == 0) { return; }

            if (MessageBox.Show("Are you sure you want to clear all images?", "Warning", MessageBoxButton.YesNo, MessageBoxImage.Warning) == MessageBoxResult.Yes)
            {
                spriteSheet.InputPaths.Clear();
                UpdateImages();
            }
        }

        private void Generate_Click(object sender, RoutedEventArgs e)
        {
            if (spriteSheet.InputPaths.Count == 0)
            {
                MessageBox.Show("Please add an image before you try to generate a spritesheet.", "Warning");
                return;
            }

            if (string.IsNullOrEmpty(spriteSheet.OutputDirectory))
            {
                MessageBox.Show("No output directory specified. Please specify an output first.", "Warning");
                return;
            }

            if (spriteSheet.Columns <= 0)
            {
                MessageBox.Show("Please enter a valid number of columns.", "Warning");
                return;
            }

            spriteSheet.GenerateSpriteSheet(true);

            // Sleep so the computer has time to generate sprite sheet / give the user a feeling of the sheet being created.
            System.Threading.Thread.Sleep(500);

            MessageBox.Show("Generated succesfully!", "Success", MessageBoxButton.OK, MessageBoxImage.Information);
        }
    }
}
