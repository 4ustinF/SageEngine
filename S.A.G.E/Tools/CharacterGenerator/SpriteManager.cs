using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Media3D;

namespace CharacterGenerator
{
    internal class SpriteManager
    {
        public bool isMale = true;

        // Order to stack layers
        public List<string> kemonoList = new List<string>();
        public List<string> Accessory2List = new List<string>();
        public List<string> Accessory1List = new List<string>();
        public List<string> MaleRearHairList = new List<string>();
        public List<string> FemaleRearHairList = new List<string>();
        public List<string> MaleFrontHairList = new List<string>();
        public List<string> FemaleFrontHairList = new List<string>();
        public List<string> GlassesList = new List<string>();
        public List<string> EyesList = new List<string>();
        public List<string> MaleClothList = new List<string>();
        public List<string> FemaleClothList = new List<string>();
        public List<string> BeardList = new List<string>();
        public List<string> HeadList = new List<string>();

        public int kemonoIndex = 0;
        public int Accessory2Index = 0;
        public int Accessory1Index = 0;
        public int RearHairIndex = 0;
        public int FrontHairIndex = 0;
        public int GlassesIndex = 0;
        public int EyesIndex = 0;
        public int ClothIndex = 0;
        public int BeardIndex = 0;
        public int HeadIndex = 0;

        public int HairColorIndex = 0;
        public int ClothColorIndex = 0;
        public int GlassesColorIndex = 0;
        public int Accessory1ColorIndex = 0;
        public int Accessory2ColorIndex = 0;

        public void Initialize()
        {
            string dir = System.AppDomain.CurrentDomain.BaseDirectory;
            for (int i = 0; i < 5; ++i)
            {
                var p = System.IO.Directory.GetParent(dir);
                dir = p.FullName;
            }
            dir += "\\Assets\\CharacterGenerator";

            HeadList = Initialize(dir + "\\Head");
            BeardList = Initialize(dir + "\\Male\\Beard");
            MaleClothList = Initialize(dir + "\\Male\\Cloth");
            FemaleClothList = Initialize(dir + "\\Female\\Cloth");
            EyesList = Initialize(dir + "\\Eyes");
            GlassesList = Initialize(dir + "\\Glasses");
            MaleFrontHairList = Initialize(dir + "\\Male\\FrontHair");
            FemaleFrontHairList = Initialize(dir + "\\Female\\FrontHair");
            MaleRearHairList = Initialize(dir + "\\Male\\RearHair");
            FemaleRearHairList = Initialize(dir + "\\Female\\RearHair");
            Accessory1List = Initialize(dir + "\\Accessories_01");
            Accessory2List = Initialize(dir + "\\Accessories_02");
            kemonoList = Initialize(dir + "\\Kemono");
        }

        private List<string> Initialize(string filePath)
        {
            // Create a filepath list of all the items within the filePath
            var list = Directory.GetFiles(filePath).ToList();

            // Remove any items that are not pngs
            for (int i = list.Count - 1; i >= 0; i--)
            {
                var extension = System.IO.Path.GetExtension(list[i]);
                if (extension != ".png")
                {
                    list.RemoveAt(i);
                }
            }

            // Files were deleted / moved
            if (list.Count == 0)
            {
                throw new ArgumentException("List cannot be empty, sprites may have been moved or deleted!", nameof(filePath));
            }

            return list;
        }

        public void OutputSpriteSheet(string filePath)
        {
            int width = 96;
            int height = 128;

            Bitmap sheet = new Bitmap(width, height);
            using (Graphics gfx = Graphics.FromImage(sheet))
            {
                Rectangle destRect = new Rectangle(0, 0, width, height);

                // Head
                Image img = Image.FromFile(HeadList[0]);
                Rectangle srcRect = new Rectangle(width * HeadIndex, 0, width, img.Height);
                gfx.DrawImage(img, destRect, srcRect, GraphicsUnit.Pixel);

                // Beard
                if (isMale && BeardIndex >= 0)
                {
                    img = Image.FromFile(BeardList[BeardIndex]);
                    srcRect = new Rectangle(width * HairColorIndex, 0, width, img.Height);
                    gfx.DrawImage(img, destRect, srcRect, GraphicsUnit.Pixel);
                }

                // Cloth
                img = Image.FromFile(isMale ? MaleClothList[ClothIndex] : FemaleClothList[ClothIndex]);
                srcRect = new Rectangle(width * ClothColorIndex, 0, width, img.Height);
                gfx.DrawImage(img, destRect, srcRect, GraphicsUnit.Pixel);

                // Eyes
                img = Image.FromFile(EyesList[0]);
                srcRect = new Rectangle(width * EyesIndex, 0, width, img.Height);
                gfx.DrawImage(img, destRect, srcRect, GraphicsUnit.Pixel);

                // Glasses
                if (GlassesIndex >= 0)
                {
                    img = Image.FromFile(GlassesList[GlassesIndex]);
                    srcRect = new Rectangle(width * GlassesColorIndex, 0, width, img.Height);
                    gfx.DrawImage(img, destRect, srcRect, GraphicsUnit.Pixel);
                }

                // FrontHair
                if (FrontHairIndex >= 0)
                {
                    img = Image.FromFile(isMale ? MaleFrontHairList[FrontHairIndex] : FemaleFrontHairList[FrontHairIndex]);
                    srcRect = new Rectangle(width * HairColorIndex, 0, width, img.Height);
                    gfx.DrawImage(img, destRect, srcRect, GraphicsUnit.Pixel);
                }

                // RearHair
                if (RearHairIndex >= 0)
                {
                    img = Image.FromFile(isMale ? MaleRearHairList[RearHairIndex] : FemaleRearHairList[RearHairIndex]);
                    srcRect = new Rectangle(width * HairColorIndex, 0, width, img.Height);
                    gfx.DrawImage(img, destRect, srcRect, GraphicsUnit.Pixel);
                }

                // Accessory1
                if (Accessory1Index >= 0)
                {
                    img = Image.FromFile(Accessory1List[Accessory1Index]);
                    srcRect = new Rectangle(width * Accessory1ColorIndex, 0, width, img.Height);
                    gfx.DrawImage(img, destRect, srcRect, GraphicsUnit.Pixel);
                }

                // Accessory2
                if (Accessory2Index >= 0)
                {
                    img = Image.FromFile(Accessory2List[Accessory2Index]);
                    srcRect = new Rectangle(width * Accessory2ColorIndex, 0, width, img.Height);
                    gfx.DrawImage(img, destRect, srcRect, GraphicsUnit.Pixel);
                }

                // Kemono
                if (kemonoIndex >= 0)
                {
                    img = Image.FromFile(kemonoList[kemonoIndex]);
                    srcRect = new Rectangle(width * HairColorIndex, 0, width, img.Height);
                    gfx.DrawImage(img, destRect, srcRect, GraphicsUnit.Pixel);
                }

                img.Dispose();
            }

            string path = Path.GetDirectoryName(filePath) + "\\" + Path.GetFileNameWithoutExtension(filePath);
            sheet.Save(path + ".png");
        }
    }
}
