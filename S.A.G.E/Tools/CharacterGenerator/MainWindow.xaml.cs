using Microsoft.Win32;
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

namespace CharacterGenerator
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private SpriteManager spriteManager = new SpriteManager();

        private List<string> beardTypeIndex = new List<string>();
        private List<string> maleClothTypeIndex = new List<string>();
        private List<string> femaleClothTypeIndex = new List<string>();
        private List<string> glassesTypeIndex = new List<string>();
        private List<string> maleFrontHairTypeIndex = new List<string>();
        private List<string> femaleFrontHairTypeIndex = new List<string>();
        private List<string> maleRearHairTypeIndex = new List<string>();
        private List<string> femaleRearHairTypeIndex = new List<string>();
        private List<string> acc1TypeIndex = new List<string>();
        private List<string> acc2TypeIndex = new List<string>();
        private List<string> kemonomimiTypeIndex = new List<string>();

        private List<string> colorIndex = new List<string>();
        private List<string> skinIndex = new List<string>();

        public MainWindow()
        {
            InitializeComponent();

            spriteManager.Initialize();

            // Beard
            beardTypeIndex.Add("None");
            for (int i = 0; i < spriteManager.BeardList.Count; ++i) { beardTypeIndex.Add($"Type {i + 1}"); }
            cbBeardType.ItemsSource = beardTypeIndex;
            cbBeardType.SelectedItem = beardTypeIndex[0];

            // Cloth
            for (int i = 0; i < spriteManager.FemaleClothList.Count; ++i) { femaleClothTypeIndex.Add($"Type {i + 1}"); }
            for (int i = 0; i < spriteManager.MaleClothList.Count; ++i) { maleClothTypeIndex.Add($"Type {i + 1}"); }
            cbClothType.ItemsSource = maleClothTypeIndex;
            cbClothType.SelectedItem = maleClothTypeIndex[0];

            // Glasses
            glassesTypeIndex.Add("None");
            for (int i = 0; i < spriteManager.GlassesList.Count; ++i) { glassesTypeIndex.Add($"Type {i + 1}"); }
            cbGlassesType.ItemsSource = glassesTypeIndex;
            cbGlassesType.SelectedItem = glassesTypeIndex[0];

            // Front Hair
            maleFrontHairTypeIndex.Add("None");
            femaleFrontHairTypeIndex.Add("None");
            for (int i = 0; i < spriteManager.FemaleFrontHairList.Count; ++i) { femaleFrontHairTypeIndex.Add($"Type {i + 1}"); }
            for (int i = 0; i < spriteManager.MaleFrontHairList.Count; ++i) { maleFrontHairTypeIndex.Add($"Type {i + 1}"); }
            cbFrontHairType.ItemsSource = maleFrontHairTypeIndex;
            cbFrontHairType.SelectedItem = maleFrontHairTypeIndex[0];

            // Rear Hair
            maleRearHairTypeIndex.Add("None");
            femaleRearHairTypeIndex.Add("None");
            for (int i = 0; i < spriteManager.FemaleFrontHairList.Count; ++i) { femaleRearHairTypeIndex.Add($"Type {i + 1}"); }
            for (int i = 0; i < spriteManager.MaleFrontHairList.Count; ++i) { maleRearHairTypeIndex.Add($"Type {i + 1}"); }
            cbRearHairType.ItemsSource = maleRearHairTypeIndex;
            cbRearHairType.SelectedItem = maleRearHairTypeIndex[0];

            // Accessory1
            acc1TypeIndex.Add("None");
            for (int i = 0; i < spriteManager.Accessory1List.Count; ++i) { acc1TypeIndex.Add($"Type {i + 1}"); }
            cbAcc1Type.ItemsSource = acc1TypeIndex;
            cbAcc1Type.SelectedItem = acc1TypeIndex[0];

            // Accessory2
            acc2TypeIndex.Add("None");
            for (int i = 0; i < spriteManager.Accessory2List.Count; ++i) { acc2TypeIndex.Add($"Type {i + 1}"); }
            cbAcc2Type.ItemsSource = acc2TypeIndex;
            cbAcc2Type.SelectedItem = acc2TypeIndex[0];

            // Kimonomimi Hair
            kemonomimiTypeIndex.Add("None");
            for (int i = 0; i < spriteManager.kemonoList.Count; ++i) { kemonomimiTypeIndex.Add($"Type {i + 1}"); }
            cbKemonoType.ItemsSource = kemonomimiTypeIndex;
            cbKemonoType.SelectedItem = kemonomimiTypeIndex[0];

            // Color Index
            for (int i = 0; i < 8; ++i)
            {
                colorIndex.Add($"Color {i + 1}");
            }

            // Skin Color
            for (int i = 0; i < 4; ++i) { skinIndex.Add($"Color {i + 1}"); }
            cbSkinColor.ItemsSource = skinIndex;
            cbSkinColor.SelectedItem = skinIndex[0];

            // Hair Color
            cbHairColor.ItemsSource = colorIndex;
            cbHairColor.SelectedItem = colorIndex[0];

            // Cloth Color
            cbClothColor.ItemsSource = colorIndex;
            cbClothColor.SelectedItem = colorIndex[0];

            // Eyes Type
            cbEyeType.ItemsSource = colorIndex;
            cbEyeType.SelectedItem = colorIndex[0];

            // Glasses Color
            cbGlassesColor.ItemsSource = colorIndex;
            cbGlassesColor.SelectedItem = colorIndex[0];

            // Accessory1 Color
            cbAcc1Color.ItemsSource = colorIndex;
            cbAcc1Color.SelectedItem = colorIndex[0];

            // Accessory2 Color
            cbAcc2Color.ItemsSource = colorIndex;
            cbAcc2Color.SelectedItem = colorIndex[0];

            // These dont start empty
            HeadImage.Fill = InitializeImage(spriteManager.HeadList[0], new Rect(0, 0, 0.25, 1.0)); // Skin
            ClothImage.Fill = InitializeImage(spriteManager.MaleClothList[0], new Rect(0, 0, 0.125, 1.0));
            EyesImage.Fill = InitializeImage(spriteManager.EyesList[0], new Rect(0, 0, 0.125, 1.0));
        }

        private void Gender_Click(object sender, RoutedEventArgs e)
        {
            spriteManager.isMale = !spriteManager.isMale;

            if(spriteManager.isMale)
            {
                miMale.IsEnabled = false;
                miFemale.IsEnabled = true;
                FemaleToMale();
            }
            else
            {
                miMale.IsEnabled = true;
                miFemale.IsEnabled = false;
                MaleToFemale();
            }
        }

        private void MaleToFemale()
        {
            cbBeardType.Visibility = Visibility.Hidden;
            BeardLabel.Visibility = Visibility.Hidden;

            cbClothType.ItemsSource = femaleClothTypeIndex;
            cbClothType.SelectedItem = femaleClothTypeIndex[0];
            ClothImage.Fill = InitializeImage(spriteManager.FemaleClothList[cbClothType.SelectedIndex], new Rect(0.125 * (cbClothColor.SelectedIndex), 0, 0.125, 1.0));

            cbFrontHairType.ItemsSource = femaleFrontHairTypeIndex;
            cbFrontHairType.SelectedItem = femaleFrontHairTypeIndex[0];
            SelectChange(FrontHairImage, cbFrontHairType.SelectedIndex, spriteManager.FemaleFrontHairList);

            cbRearHairType.ItemsSource = femaleRearHairTypeIndex;
            cbRearHairType.SelectedItem = femaleRearHairTypeIndex[0];
            SelectChange(RearHairImage, cbRearHairType.SelectedIndex, spriteManager.FemaleRearHairList);

            cbBeardType.SelectedItem = beardTypeIndex[0];
            SelectChange(BeardImage, cbBeardType.SelectedIndex, spriteManager.BeardList);
        }

        private void FemaleToMale()
        {
            cbBeardType.Visibility = Visibility.Visible;
            BeardLabel.Visibility = Visibility.Visible;

            cbClothType.ItemsSource = maleClothTypeIndex;
            cbClothType.SelectedItem = maleClothTypeIndex[0];
            ClothImage.Fill = InitializeImage(spriteManager.MaleClothList[cbClothType.SelectedIndex], new Rect(0.125 * (cbClothColor.SelectedIndex), 0, 0.125, 1.0));

            cbFrontHairType.ItemsSource = maleFrontHairTypeIndex;
            cbFrontHairType.SelectedItem = maleFrontHairTypeIndex[0];
            SelectChange(FrontHairImage, cbFrontHairType.SelectedIndex, spriteManager.MaleFrontHairList);

            cbRearHairType.ItemsSource = maleRearHairTypeIndex;
            cbRearHairType.SelectedItem = maleRearHairTypeIndex[0];
            SelectChange(RearHairImage, cbRearHairType.SelectedIndex, spriteManager.MaleRearHairList);
        }

        #region ---ImageSelection---

        private ImageBrush InitializeImage(string filePath, Rect rect)
        {
            BitmapImage myBitmapImage = new BitmapImage(new Uri(filePath));
            ImageBrush ib = new ImageBrush();
            ib.ImageSource = myBitmapImage;
            ib.Viewbox = rect;
            ib.ViewboxUnits = BrushMappingMode.RelativeToBoundingBox;
            ib.TileMode = TileMode.None;
            return ib;
        }

        private void Skin_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            HeadImage.Fill = InitializeImage(spriteManager.HeadList[0], new Rect(0.25 * cbSkinColor.SelectedIndex, 0, 0.25, 1.0));
        }

        private void Hair_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // Update anything that uses hair
            SelectChange(BeardImage, cbBeardType.SelectedIndex, spriteManager.BeardList);                   // Beard
            SelectChange(FrontHairImage, cbFrontHairType.SelectedIndex, spriteManager.MaleFrontHairList);   // Front Hair
            SelectChange(RearHairImage, cbRearHairType.SelectedIndex, spriteManager.MaleRearHairList);      // Rear Hair
            SelectChange(KemonoImage, cbKemonoType.SelectedIndex, spriteManager.kemonoList);                // Kemonomimi
        }

        private void Beard_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            SelectChange(BeardImage, cbBeardType.SelectedIndex, spriteManager.BeardList);
        }

        private void Cloth_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if(spriteManager.isMale) {
                ClothImage.Fill = InitializeImage(spriteManager.MaleClothList[cbClothType.SelectedIndex], new Rect(0.125 * (cbClothColor.SelectedIndex), 0, 0.125, 1.0));
            }
            else {
                ClothImage.Fill = InitializeImage(spriteManager.FemaleClothList[cbClothType.SelectedIndex], new Rect(0.125 * (cbClothColor.SelectedIndex), 0, 0.125, 1.0));
            }
        }

        private void Eye_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            EyesImage.Fill = InitializeImage(spriteManager.EyesList[0], new Rect(0.125 * cbEyeType.SelectedIndex, 0, 0.125, 1.0));
        }

        private void Glasses_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if(cbGlassesType.SelectedIndex > 0)
            {
                GlassesImage.Fill = InitializeImage(spriteManager.GlassesList[cbGlassesType.SelectedIndex - 1], new Rect(0.125 * (cbGlassesColor.SelectedIndex), 0, 0.125, 1.0));
                return;
            }
            GlassesImage.Fill = Brushes.Transparent;
        }

        private void FrontHair_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (spriteManager.isMale) {
                SelectChange(FrontHairImage, cbFrontHairType.SelectedIndex, spriteManager.MaleFrontHairList);
            }
            else {
                SelectChange(FrontHairImage, cbFrontHairType.SelectedIndex, spriteManager.FemaleFrontHairList);
            } 
        }

        private void RearHair_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (spriteManager.isMale) {
                SelectChange(RearHairImage, cbRearHairType.SelectedIndex, spriteManager.MaleRearHairList);
            }
            else {
                SelectChange(RearHairImage, cbRearHairType.SelectedIndex, spriteManager.FemaleRearHairList);
            }
        }

        private void Accessory1_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (cbAcc1Type.SelectedIndex > 0)
            {
                Accessory1Image.Fill = InitializeImage(spriteManager.Accessory1List[cbAcc1Type.SelectedIndex - 1], new Rect(0.125 * (cbAcc1Color.SelectedIndex), 0, 0.125, 1.0));
                return;
            }
            Accessory1Image.Fill = Brushes.Transparent;
        }

        private void Accessory2_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (cbAcc2Type.SelectedIndex > 0)
            {
                Accessory2Image.Fill = InitializeImage(spriteManager.Accessory2List[cbAcc2Type.SelectedIndex - 1], new Rect(0.125 * (cbAcc2Color.SelectedIndex), 0, 0.125, 1.0));
                return;
            }
            Accessory2Image.Fill = Brushes.Transparent;
        }

        private void Kemonomimi_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            SelectChange(KemonoImage, cbKemonoType.SelectedIndex, spriteManager.kemonoList);
        }

        private void SelectChange(Rectangle rect, int typeIndex, List<string> fileList)
        {
            if (typeIndex > 0)
            {
                string filePath = fileList[typeIndex - 1];
                rect.Fill = InitializeImage(filePath, new Rect(0.125 * cbHairColor.SelectedIndex, 0, 0.125, 1.0));
                return;
            }
            rect.Fill = Brushes.Transparent;
        }

        #endregion ---ImageSelection---

        #region ---Buttons---

        private void RandomizeButton_Click(object sender, RoutedEventArgs e)
        {
            Random r = new Random();
            cbHairColor.SelectedIndex = r.Next(0, colorIndex.Count);

            cbSkinColor.SelectedIndex = r.Next(0, skinIndex.Count);
            cbBeardType.SelectedIndex = r.Next(0, beardTypeIndex.Count);
            cbClothType.SelectedIndex = r.Next(0, spriteManager.isMale ? maleClothTypeIndex.Count : femaleClothTypeIndex.Count);
            cbClothColor.SelectedIndex = r.Next(0, colorIndex.Count);
            cbEyeType.SelectedIndex = r.Next(0, colorIndex.Count);
            cbGlassesType.SelectedIndex = r.Next(0, glassesTypeIndex.Count);
            cbGlassesColor.SelectedIndex = r.Next(0, colorIndex.Count);
            cbFrontHairType.SelectedIndex = r.Next(0, spriteManager.isMale ? maleFrontHairTypeIndex.Count : femaleFrontHairTypeIndex.Count);
            cbRearHairType.SelectedIndex = r.Next(0, spriteManager.isMale ? maleRearHairTypeIndex.Count : femaleRearHairTypeIndex.Count);
            cbAcc1Type.SelectedIndex = r.Next(0, acc1TypeIndex.Count);
            cbAcc1Color.SelectedIndex = r.Next(0, colorIndex.Count);
            cbAcc2Type.SelectedIndex = r.Next(0, acc2TypeIndex.Count);
            cbAcc2Color.SelectedIndex = r.Next(0, colorIndex.Count);
            cbKemonoType.SelectedIndex = r.Next(0, kemonomimiTypeIndex.Count);
        }

        private void OutputSpriteSheetButton_Click(object sender, RoutedEventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            if (saveFileDialog.ShowDialog() == true)
            {
                spriteManager.HairColorIndex = cbHairColor.SelectedIndex;

                spriteManager.HeadIndex = cbSkinColor.SelectedIndex;
                spriteManager.BeardIndex = cbBeardType.SelectedIndex - 1;
                spriteManager.ClothIndex = cbClothType.SelectedIndex;
                spriteManager.ClothColorIndex = cbClothColor.SelectedIndex;
                spriteManager.EyesIndex = cbEyeType.SelectedIndex;
                spriteManager.GlassesIndex = cbGlassesType.SelectedIndex - 1;
                spriteManager.GlassesColorIndex = cbGlassesColor.SelectedIndex;
                spriteManager.FrontHairIndex = cbFrontHairType.SelectedIndex - 1;
                spriteManager.RearHairIndex = cbRearHairType.SelectedIndex - 1;
                spriteManager.Accessory1Index = cbAcc1Type.SelectedIndex - 1;
                spriteManager.Accessory1ColorIndex = cbAcc1Color.SelectedIndex;
                spriteManager.Accessory2Index = cbAcc2Type.SelectedIndex - 1;
                spriteManager.Accessory2ColorIndex = cbAcc2Color.SelectedIndex;
                spriteManager.kemonoIndex = cbKemonoType.SelectedIndex - 1;

                spriteManager.OutputSpriteSheet(saveFileDialog.FileName);
            }
        }


        #endregion ---Buttons---
    }
}
