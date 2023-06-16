using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Data.Common;
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

namespace SpriteSheetEditor
{
    /// <summary>
    /// Interaction logic for StripWindow.xaml
    /// </summary>
    public partial class StripWindow : Window
    {
        private SpriteSheet spriteSheet = new SpriteSheet();
        private bool dimensionsLocked = true;
        private Image spriteSheetImage = new Image();
        private int width, height = 1;
        private int sampleX, sampleY = 0;
        private double windowStartHeight = 550; 
        //private double windowStartWidth = 800; 


        public StripWindow()
        {
            InitializeComponent();

            Height = windowStartHeight;

            tbHeight.Text = "1";
            tbHeight.Text = "1";
        }

        private void New_Click(object sender, RoutedEventArgs e)
        {
            StripWindow w = new StripWindow();
            w.Show();
            this.Close();
        }

        private void Exit_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void Sample_Checked(object sender, RoutedEventArgs e)
        {
            miBGRemove.IsChecked = true;
            miBGNone.IsChecked = false;
            spriteSheet.RemoveBackground = true;

            // Set preview color
            if(spriteSheetImage.Source != null)
            {
                var c = spriteSheet.SampleSpriteSheet(sampleX, sampleY);
                SolidColorBrush mySolidColorBrush1 = new SolidColorBrush();
                mySolidColorBrush1.Color = Color.FromArgb(c.A, c.R, c.G, c.B);
                rPixelPreview.Fill = mySolidColorBrush1;
            }

            Height = 700;
        }

        private void SampleNone_Checked(object sender, RoutedEventArgs e)
        {
            miBGRemove.IsChecked = false;
            miBGNone.IsChecked = true;
            spriteSheet.RemoveBackground = false;

            Height = windowStartHeight;
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

        private void tbWidth_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (int.TryParse(tbWidth.Text, out int w))
            {
                if (w <= 0) { w = 1; }

                width = w;
                height = dimensionsLocked ? width : height;

                tbWidth.Text = width.ToString();
                tbHeight.Text = dimensionsLocked ? width.ToString() : tbHeight.Text;

                tbWidth.Focus();
                tbWidth.SelectionStart = tbWidth.Text.Length;
                return;
            }

            width = 1;
            tbWidth.Text = String.Empty;
            tbHeight.Text = dimensionsLocked ? String.Empty : tbHeight.Text;
            height = dimensionsLocked ? width : height;
        }

        private void tbHeight_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (int.TryParse(tbHeight.Text, out int h))
            {
                if (h <= 0) { h = 1; }

                height = h;
                width = dimensionsLocked ? height : width;

                tbHeight.Text = height.ToString();
                tbWidth.Text = dimensionsLocked ? height.ToString() : tbWidth.Text;

                tbHeight.Focus();
                tbHeight.SelectionStart = tbHeight.Text.Length;
                return;
            }

            height = 1;
            tbHeight.Text = String.Empty;
            tbWidth.Text = dimensionsLocked ? String.Empty : tbWidth.Text;
            width = dimensionsLocked ? height : width;
        }

        private void DimensionsLocked_Checked(object sender, RoutedEventArgs e)
        {
            dimensionsLocked = (bool)cbDimensionsLocked.IsChecked;
        }

        private void cbRows_Checked(object sender, RoutedEventArgs e)
        {
            spriteSheet.Mode = SpriteSheet.SplitMode.Rows;
        }

        private void cbPixels_Checked(object sender, RoutedEventArgs e)
        {
            spriteSheet.Mode = SpriteSheet.SplitMode.Pixels;
        }

        private void Import_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Multiselect = false;
            openFileDialog.Filter = "Png Image (.png)|*.png";

            if (openFileDialog.ShowDialog() == true)
            {
                string fileName = openFileDialog.FileName;

                spriteSheet.InputPath = fileName;

                spriteSheetImage.Source = new BitmapImage(new Uri(fileName));
                SheetImage.Source = spriteSheetImage.Source;
            }

            // Samples the bg so its up to date
            if(spriteSheet.RemoveBackground)
            {
                var c = spriteSheet.SampleSpriteSheet(sampleX, sampleY);
                SolidColorBrush mySolidColorBrush1 = new SolidColorBrush();
                mySolidColorBrush1.Color = Color.FromArgb(c.A, c.R, c.G, c.B);
                rPixelPreview.Fill = mySolidColorBrush1;
            }
        }

        private void Strip_Click(object sender, RoutedEventArgs e)
        {
            if (spriteSheetImage.Source == null)
            {
                MessageBox.Show("Please import an image before you try to strip a spritesheet.", "Warning");
                return;
            }

            if (string.IsNullOrEmpty(spriteSheet.OutputDirectory))
            {
                MessageBox.Show("No output directory specified. Please specify an output first.", "Warning");
                return;
            }

            if (width > (int)SheetImage.Source.Width || height > (int)SheetImage.Source.Height)
            {
                MessageBox.Show("Dimensions to large. Please try a smaller dimension", "Warning");
                return;
            }

            // Succesfully stripped sprite sheet
            var sampleColor = spriteSheet.SampleSpriteSheet(sampleX, sampleY);
            if (spriteSheet.StripSpriteSheet(width, height, sampleColor))
            {
                System.Threading.Thread.Sleep(500);
                MessageBox.Show("Sheet stripped succesfully!", "Success!", MessageBoxButton.OK, MessageBoxImage.Information);
            }
        }

        private void tbSampleX_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (spriteSheetImage.Source == null) { return; }

            if (int.TryParse(tbSampleX.Text, out int outX))
            {
                if (outX < 0) { outX = 0; }

                // Set sample
                sampleX = outX;

                // Set sample if out of bounds
                if (sampleX >= (int)SheetImage.Source.Width)
                {
                    sampleX = (int)SheetImage.Source.Width - 1;
                    tbSampleX.Text = sampleX.ToString();
                    tbSampleX.Focus();
                    tbSampleX.SelectionStart = tbSampleX.Text.Length;
                }

                // Set preview color
                var c1 = spriteSheet.SampleSpriteSheet(sampleX, sampleY);
                SolidColorBrush mySolidColorBrush1 = new SolidColorBrush();
                mySolidColorBrush1.Color = Color.FromArgb(c1.A, c1.R, c1.G, c1.B);
                rPixelPreview.Fill = mySolidColorBrush1;
                return;
            }

            // Set sample
            sampleX = 0;
            tbSampleX.Text = String.Empty;

            // Set preview color
            var c2 = spriteSheet.SampleSpriteSheet(sampleX, sampleY);
            SolidColorBrush mySolidColorBrush2 = new SolidColorBrush();
            mySolidColorBrush2.Color = Color.FromArgb(c2.A, c2.R, c2.G, c2.B);
            rPixelPreview.Fill = mySolidColorBrush2;
        }

        private void tbSampleY_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (spriteSheetImage.Source == null) { return; }

            if (int.TryParse(tbSampleY.Text, out int outY))
            {
                if (outY < 0) { outY = 0; }

                // Set sample
                sampleY = outY;

                // Set sample if out of bounds
                if (sampleY >= (int)SheetImage.Source.Height)
                {
                    sampleY = (int)SheetImage.Source.Height - 1;
                    tbSampleY.Text = sampleY.ToString();
                    tbSampleY.Focus();
                    tbSampleY.SelectionStart = tbSampleY.Text.Length;
                }

                // Set preview color
                var c1 = spriteSheet.SampleSpriteSheet(sampleX, sampleY);
                SolidColorBrush mySolidColorBrush1 = new SolidColorBrush();
                mySolidColorBrush1.Color = Color.FromArgb(c1.A, c1.R, c1.G, c1.B);
                rPixelPreview.Fill = mySolidColorBrush1;
                return;
            }

            // Set sample
            sampleY = 0;
            tbSampleY.Text = String.Empty;

            // Set preview color
            var c2 = spriteSheet.SampleSpriteSheet(sampleX, sampleY);
            SolidColorBrush mySolidColorBrush2 = new SolidColorBrush();
            mySolidColorBrush2.Color = Color.FromArgb(c2.A, c2.R, c2.G, c2.B);
            rPixelPreview.Fill = mySolidColorBrush2;
        }
    }
}
