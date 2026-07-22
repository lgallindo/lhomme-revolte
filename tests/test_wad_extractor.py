import os
import sys
import unittest

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "tools"))
from extract_wad_flat import extract_flat

class TestWADExtractor(unittest.TestCase):
    def setUp(self):
        self.outdir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "samples")
        self.wad_path = os.path.join(self.outdir, "freedoom1.wad")
        self.palette = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "core", "palette.h")
        self.flat_name = "FLOOR4_8"
        self.out_h = os.path.join(self.outdir, "floor4_8.h")
        
        # Cleanup before test
        if os.path.exists(self.out_h):
            os.remove(self.out_h)

    def test_extract_valid_flat(self):
        # Only run if the WAD is available (to prevent CI failures if missing)
        if not os.path.exists(self.wad_path):
            self.skipTest(f"WAD file not found at {self.wad_path}")
            
        extract_flat(self.wad_path, self.flat_name, self.palette, self.outdir)
        
        # Assert file was created
        self.assertTrue(os.path.exists(self.out_h))
        
        # Assert file has expected content
        with open(self.out_h, "r") as f:
            content = f.read()
            self.assertIn("LHR_FLAT_FLOOR4_8", content)
            self.assertIn(".width = 64", content)
            self.assertIn(".height = 64", content)

    def test_extract_invalid_flat(self):
        if not os.path.exists(self.wad_path):
            self.skipTest(f"WAD file not found at {self.wad_path}")
            
        with self.assertRaises(ValueError):
            extract_flat(self.wad_path, "NONEXISTENT", self.palette, self.outdir)

if __name__ == "__main__":
    unittest.main()
