using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media.Media3D;
using System.Windows.Media.TextFormatting;

namespace SpriteSheetEditor
{
    public class SpriteSheet
    {
        public enum SplitMode
        {
            Rows,
            Pixels
        }

        public int Columns { get; set; }
        public String OutputFile { get; set; }
        public String OutputDirectory { get; set; }
        public List<String> InputPaths { get; set; }
        readonly public string BlankString = "Blank";
        public int BlankCount { get; set; }

        // ----------------------

        public SplitMode Mode = SplitMode.Rows;
        public string InputPath { get; set; }
        public bool RemoveBackground { get; set; }

        private void Validate()
        {
            if (InputPaths == null || InputPaths.Count == 0)
            {
                throw new Exception("The input path cannot be empty");
            }

            if (string.IsNullOrEmpty(OutputFile))
            {
                throw new Exception("The output file cannot be empty");
            }

            if (string.IsNullOrEmpty(OutputDirectory))
            {
                throw new Exception("The output directory cannot be empty");
            }

            if (Columns < 1)
            {
                throw new Exception("Column size must be at least 1");
            }
        }

        public void GenerateSpriteSheet(bool overwrite)
        {
            Validate();

            string outputPath = Path.Combine(OutputDirectory, OutputFile);

            if (File.Exists(outputPath))
            {
                if (overwrite) { File.Delete(outputPath); }
                else { throw new Exception("The output file already exists."); }
            }

            int fileCount = InputPaths.Count;
            if (fileCount > 0 && fileCount > BlankCount)
            {
                // Determines the largest sprite and use that as the cell size.
                int maxWidth = 0;
                int maxHeight = 0;
                foreach (string f in InputPaths)
                {
                    if(f == BlankString) { continue; }

                    Image img = Image.FromFile(f);
                    maxWidth = Math.Max(maxWidth, img.Width);
                    maxHeight = Math.Max(maxHeight, img.Height);
                    img.Dispose();
                }

                if (fileCount < Columns)
                {
                    // Single row, reducing column count to match file count.
                    Columns = fileCount;
                }

                int width = Columns * maxWidth;
                int rows = (fileCount / Columns) + ((fileCount % Columns > 0) ? 1 : 0);
                int height = rows * maxHeight;

                //Console.WriteLine($"Output: {rows} rows, {Columns} cols, {width} x {height} resolution.");

                Bitmap sheet = new Bitmap(width, height);
                using (Graphics gfx = Graphics.FromImage(sheet))
                {
                    int col = 0;
                    int row = 0;
                    foreach (string f in InputPaths)
                    {
                        Image img = f == BlankString ? new Bitmap(width, height) : Image.FromFile(f);

                        int x = (col * maxWidth) + (maxWidth / 2 - img.Width / 2);
                        int y = (row * maxHeight) + (maxHeight / 2 - img.Height / 2);

                        Rectangle srcRect = new Rectangle(0, 0, img.Width, img.Height);
                        Rectangle destRect = new Rectangle(x, y, img.Width, img.Height);

                        gfx.DrawImage(img, destRect, srcRect, GraphicsUnit.Pixel);

                        img.Dispose();

                        col++;
                        if (col == Columns)
                        {
                            col = 0;
                            row++;
                        }
                    }
                }
                
                sheet.Save(outputPath);
            }
        }

        public bool StripSpriteSheet(int width, int height, Color sampleColor)
        {
            // No point removing nothing
            if(sampleColor.A == 0) { RemoveBackground = false; }

            if (Mode == SplitMode.Rows)
            {
                Image sheetImage = Image.FromFile(InputPath);
                int spriteWidth = sheetImage.Width / width;
                int spriteHeight = sheetImage.Height / height;
                int count = 0;

                for (int x = 0; x < height; ++x)
                {
                    for (int y = 0; y < width; ++y)
                    {
                        string name = Path.GetFileNameWithoutExtension(OutputFile) + $"_{count}";
                        string outputPath = Path.Combine(OutputDirectory, name + Path.GetExtension(OutputFile));

                        Bitmap sprite = new Bitmap(spriteWidth, spriteHeight);
                        using (Graphics gfx = Graphics.FromImage(sprite))
                        {
                            Rectangle srcRect = new Rectangle(y * (spriteWidth), x * (spriteHeight), spriteWidth, spriteHeight);
                            Rectangle destRect = new Rectangle(0, 0, spriteWidth, spriteHeight);

                            gfx.DrawImage(sheetImage, destRect, srcRect, GraphicsUnit.Pixel);
                        }

                        // Remove background color
                        if (RemoveBackground) 
                        {
                            uint removeCount = 0;
                            for (int i = 0; i < sprite.Height; ++i) {
                                for (int j = 0; j < sprite.Width; ++j) {
                                    if (sprite.GetPixel(j, i) == sampleColor) {
                                        removeCount++;
                                        sprite.SetPixel(j, i, Color.Transparent);
                                    }
                                }
                            }

                            // No need to reiterate over all the pixels to see if its blank if we removed all the pixels
                            if(removeCount == sprite.Height * sprite.Width) {
                                count++;
                                continue;
                            }
                        }

                        if(IsBlankImage(sprite)) { continue; }
                        count++;

                        sprite.Save(outputPath);
                    }
                }
            }
            else if (Mode == SplitMode.Pixels)
            {
                Image sheetImage = Image.FromFile(InputPath);
                int count = 0;

                int columns = sheetImage.Height / height;
                int rows = sheetImage.Width / width;

                for (int x = 0; x < columns; ++x)
                {
                    for (int y = 0; y < rows; ++y)
                    {
                        string name = Path.GetFileNameWithoutExtension(OutputFile) + $"_{count}";
                        string outputPath = Path.Combine(OutputDirectory, name + Path.GetExtension(OutputFile));

                        Bitmap sprite = new Bitmap(width, height);
                        using (Graphics gfx = Graphics.FromImage(sprite))
                        {
                            Rectangle srcRect = new Rectangle(y * width, x * height, width, height);
                            Rectangle destRect = new Rectangle(0, 0, width, height);

                            gfx.DrawImage(sheetImage, destRect, srcRect, GraphicsUnit.Pixel);
                        }

                        // Remove background color
                        if (RemoveBackground)
                        {
                            uint removeCount = 0;
                            for (int i = 0; i < sprite.Height; ++i) {
                                for (int j = 0; j < sprite.Width; ++j) {
                                    if (sprite.GetPixel(j, i) == sampleColor) {
                                        removeCount++;
                                        sprite.SetPixel(j, i, Color.Transparent);
                                    }
                                }
                            }

                            // No need to reiterate over all the pixels to see if its blank if we removed all the pixels
                            if (removeCount == sprite.Height * sprite.Width) {
                                count++;
                                continue;
                            }
                        }

                        if (IsBlankImage(sprite)) { continue; }
                        count++;

                        sprite.Save(outputPath);
                    }
                }
            }

            return true;
        }

        public Color SampleSpriteSheet(int sampleX, int sampleY)
        {
            Image sheetImage = Image.FromFile(InputPath);

            Bitmap sheet = new Bitmap(sheetImage.Width, sheetImage.Height);
            using (Graphics gfx = Graphics.FromImage(sheet))
            {
                Rectangle rect = new Rectangle(0, 0, sheetImage.Width, sheetImage.Height);
                gfx.DrawImage(sheetImage, rect, rect, GraphicsUnit.Pixel);
            }
            return sheet.GetPixel(sampleX, sampleY);
        }

        private bool IsBlankImage(Bitmap sprite)
        {
            // Early check
            int w = sprite.Width / 2;
            int h = sprite.Height / 2;
            if (sprite.GetPixel(w, h).A != 0) { return false; }

            // Check whole image
            for (int y = 0; y < sprite.Height; ++y)
            {
                for (int x = 0; x < sprite.Width; ++x)
                {
                    if (sprite.GetPixel(x, y).A != 0)
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        public void Reset()
        {
            Columns = 0;
            OutputFile = String.Empty;
            OutputDirectory = String.Empty;
            InputPaths.Clear();
            BlankCount = 0;

            Mode = SplitMode.Rows;
            InputPath = String.Empty;
            RemoveBackground = false;
        }
    }
}
